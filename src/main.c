#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"

#include "geometry.c"
#include "graphics.c"

typedef struct model {
  u32    s_vertices;
  Vec3D* vertices;
  u32    s_indices;
  u32*   indices;
  Color* colors; // maybe add vertex colors to add gradients
} Model;

Model* model_create(const u32 s_vertices, const u32 s_indices) {
  assert(s_vertices > 0);

  Model* model = (Model*)malloc(sizeof(struct model));
  assert(model != NULL);

  Vec3D* vertices = (Vec3D*)malloc(s_vertices * sizeof(struct vector3d));
  assert(vertices != NULL);

  u32* indices = (u32*)malloc(s_indices * sizeof(u32));
  assert(indices != NULL);

  Color* colors = (Color*)malloc(s_vertices * sizeof(struct color));
  assert(colors != NULL);

  *model = (Model){
    .s_vertices = s_vertices,
    .vertices   = vertices,
    .s_indices  = s_indices,
    .indices    = indices,
    .colors     = colors
  };

  return model;
}

void model_free(Model* model) {
  if (model == NULL)
    return;

  if (model->vertices != NULL)
    free(model->vertices);
  if (model->indices != NULL)
    free(model->indices);
  if (model->colors != NULL)
    free(model->colors);

  free(model);
}

void graphics_clipper(const Camera* cam, const Model* model, RenderQueueSoA* queue) {
  const f32 near_plane = cam->near_plane;

  for (u32 i = 0; i < model->s_indices; i += 3) {
    const Vec3D 
      v1 = geometry_camera_transform(cam, model->vertices[model->indices[i]]),
      v2 = geometry_camera_transform(cam, model->vertices[model->indices[i + 1]]),
      v3 = geometry_camera_transform(cam, model->vertices[model->indices[i + 2]]);

    const Color 
      c1 = model->vertex_colors[model->indices[i]],
      c2 = model->vertex_colors[model->indices[i + 1]],
      c3 = model->vertex_colors[model->indices[i + 2]];

    // 2. Basic Clipping Check (Simple Discard for now)
    // If all vertices are behind near plane, skip
    if (v1.z < near_plane && v2.z < near_plane && v3.z < near_plane)
      continue;

    // 3. Push to SoA Queue
    u32 idx = queue->count;
    queue->v1x[idx] = v1.x; queue->v1y[idx] = v1.y; queue->v1z[idx] = v1.z;
    queue->v1r[idx] = c1.r; queue->v1g[idx] = c1.g; queue->v1b[idx] = c1.b;
    
    queue->v2x[idx] = v2.x; queue->v2y[idx] = v2.y; queue->v2z[idx] = v2.z;
    queue->v2r[idx] = c2.r; queue->v2g[idx] = c2.g; queue->v2b[idx] = c2.b;

    queue->v3x[idx] = v3.x; queue->v3y[idx] = v3.y; queue->v3z[idx] = v3.z;
    queue->v3r[idx] = c3.r; queue->v3g[idx] = c3.g; queue->v3b[idx] = c3.b;

    queue->count++;
  }
}

void graphics_render(
  Window* window,
  const Triangle3D* queue, const u32 s_queue,
  const Vec3D unit_vector, const Vec3D origin
) {
  for (u32 i = 0; i < count; i++) {
    const Triangle2D tri2d = {
      .v1 = geometry_vec3d_to_2d(queue[i].vertices[0], unit_vector, origin),
      .v2 = geometry_vec3d_to_2d(queue[i].vertices[1], unit_vector, origin),
      .v3 = geometry_vec3d_to_2d(queue[i].vertices[2], unit_vector, origin)
    };

    graphics_draw_triangle_2d(window, tri2d, queue[i].color, queue[i].color, 255);
  }
}

void event_poll(SDL_Event* event, bool* running) {
  while (SDL_PollEvent(event)) {
    if (event->type == SDL_EVENT_QUIT)
      *running = false;

    if (event->type == SDL_EVENT_KEY_DOWN) {
      if (event->key.key == SDLK_ESCAPE)
        *running = false;
    }
  }
}

typedef struct platform {
  const f32 width, length;
  const u32 tiles;
  Model* model;
} Platform;

void platform_build_model(const Camera* cam, Platform* platform) {
  const f32 
    sqrt_tiles  = sqrtf(platform->tiles),
    tile_width  = platform->width / sqrt_tiles,
    tile_length = platform->length / sqrt_tiles,
    start_x = -platform->width / 2.0f,
    start_z = -platform->length / 2.0f;
  
  const u32 tiles_per_side = (u32)sqrt_tiles;

  platform->model = model_create(
    4 * tiles_per_side * tiles_per_side,
    6 * tiles_per_side * tiles_per_side
  );
  assert(platform->model != NULL);
  
  for (i32 row = tiles_per_side - 1; row >= 0; row--) {
    for (i32 col = tiles_per_side - 1; col >= 0; col--) {
      const f32 
        x1 = start_x + col * tile_width,
        x2 = start_x + (col + 1) * tile_width,
        z1 = start_z + row * tile_length,
        z2 = start_z + (row + 1) * tile_length,
        y = 0.0f;
      
      const Vec3D corners[4] = {
        { x1, y, z1 },
        { x2, y, z1 },
        { x2, y, z2 },
        { x1, y, z2 }
      };

      const u32 index = 4 * (row * tiles_per_side + col);
      platform->model->vertices[index]     = corners[0];
      platform->model->vertices[index + 1] = corners[1];
      platform->model->vertices[index + 2] = corners[2];
      platform->model->vertices[index + 3] = corners[3];
    }
  }
}

i32 main(const i32 argc, const char* argv[]) {
  const u32 
    width  = 16 * 90,
    height = 9  * 90;

  Window* window = graphics_init("Engine", width, height);
  if (window == NULL)
    return 1; 

  Camera camera = {
    .position = { 0, 20, 0 },
    .pitch = -0.8f, // -M_PI / 2.0f,
    .yaw = 0.0f,
    .fov = M_PI / 3.0f
  };

  const Vec3D 
    origin = { width / 2.0f, height / 2.0f, .0f },
    unit_vector = { 90, 90, 90 };

  const Platform platform = {
    .width  = 1000.f,
    .length = 1000.f,
    .tiles  = 100
  };

  platform_build_model(&camera, &platform);

  bool running = true;
  SDL_Event event;

  f32 dt = 0.f;
  while (running) {
    event_poll(&event, &running);
    graphics_clear(window, ColorBlue);

    camera.position.x = unit_vector.x * sinf(M_PI / 360.0f * dt);
    camera.position.y = 20.f + 0.10 * unit_vector.y * cosf(M_PI / 360.0f * dt);

    dt += 0.016f;
    if (dt >= 720.f)
      dt = 0;

    graphics_clipper(camera, models, s_models);

    for (u32 i = 0; i < s_models; i++)
      graphics_render(window, &models[i], unit_vector, origin);

    graphics_present(window);
    graphics_delay(60);
  }

  graphics_close(window);
  
  return 0;
}
