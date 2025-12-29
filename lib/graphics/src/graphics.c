#include "graphics.h"

struct window {
  u32 width, height;
  const char* title;
  SDL_Window* window;
  SDL_Renderer* renderer;
};

struct ColorRGB {
  u8 red, green, blue;
};

struct ColorRGB color_map(const Color);

static f32 interpolate(const f32 y, const f32 y1, const f32 x1, const f32 y2, const f32 x2);
static void swap(f32* x, f32* y);

Window* graphics_init(const char* title, const u32 width, const u32 height) {
  assert(title != NULL);
  
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return NULL;
  }
  
  Window* window = (Window*)malloc(sizeof(struct window));
  assert(window != NULL);
  
  u64 flags = 0; // Remove SDL_WINDOW_FULLSCREEN for testing
  SDL_Window* sdl_window = SDL_CreateWindow(title, width, height, flags);
  if (!sdl_window) {
    fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    free(window);
    SDL_Quit();
    return NULL;
  }

  SDL_SetWindowPosition(sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  SDL_Renderer* renderer = SDL_CreateRenderer(sdl_window, NULL);
  if (!renderer) {
    fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(sdl_window);
    free(window);
    SDL_Quit();
    return NULL;
  }
  
  *window = (Window){
    .width    = width,
    .height   = height,
    .title    = title,
    .window   = sdl_window,
    .renderer = renderer
  };
  
  return window;
}

void graphics_draw_points(
  Window* window,
  const Vec2D* points, const u64 s_points,
  const Color color, const u8 alpha
) {
  struct ColorRGB rgb = COLORS[color];
  SDL_SetRenderDrawColor(window->renderer, rgb.red, rgb.green, rgb.blue, alpha);
  SDL_RenderPoints(window->renderer, (SDL_FPoint*)(points), s_points);
}


void graphics_draw_line_2d(
  Window* window, const Line2D line,
  const Color color, const u8 alpha
) {
  struct ColorRGB rgb = COLORS[color];
  SDL_SetRenderDrawColor(window->renderer, rgb.red, rgb.green, rgb.blue, alpha);
  
  SDL_RenderLine(
    window->renderer, 
    line.start.x, line.start.y,
    line.end.x,   line.end.y
  );
}

void graphics_draw_lines_2d(
  Window* window,
  const Line2D* lines, const u64 s_lines,
  const Color color, const u8 alpha
) {
  struct ColorRGB rgb = COLORS[color];
  SDL_SetRenderDrawColor(window->renderer, rgb.red, rgb.green, rgb.blue, alpha);
  
  for (u64 i = 0; i < s_lines; i++)
    SDL_RenderLine(
      window->renderer, 
      lines[i].start.x, lines[i].start.y,
      lines[i].end.x,   lines[i].end.y
    );
}

void graphics_draw_line_3d(
  Window* window, const Camera* cam,
  const Vec3D v1, const Vec3D v2, const Vec3D unit_vector, const Vec3D origin,
  const Color color, const u8 alpha
) {
  Line2D edge = (Line2D){
    .start = geometry_vec3d_to_2d(geometry_camera_transform(cam, v1), unit_vector, origin),
    .end   = geometry_vec3d_to_2d(geometry_camera_transform(cam, v2), unit_vector, origin)
  };
  graphics_draw_line_2d(window, edge, color, alpha);
}

void graphics_draw_triangle_2d(
  Window* window,
  const Triangle2D triangle,
  const Color fill_color,
  const Color border_color,
  const u8 alpha
) {
  struct ColorRGB fill = COLORS[fill_color];
  
  f32 
    x1 = triangle.v1.x, y1 = triangle.v1.y,
    x2 = triangle.v2.x, y2 = triangle.v2.y,
    x3 = triangle.v3.x, y3 = triangle.v3.y;
  
  if (y2 < y1) {
    swap(&x1, &x2);
    swap(&y1, &y2);
  }
  if (y3 < y1) {
    swap(&x1, &x3);
    swap(&y1, &y3);
  }
  if (y3 < y2) {
    swap(&x2, &x3);
    swap(&y2, &y3);
  }
  
  SDL_SetRenderDrawColor(window->renderer, fill.red, fill.green, fill.blue, alpha);
  
  // Top half
  for (f32 y = y1; y <= y2; y++) {
    f32
      xa = interpolate(y, y1, x1, y3, x3),
      xb = interpolate(y, y1, x1, y2, x2);
    if (xa > xb)
      swap(&xa, &xb);

    SDL_RenderLine(window->renderer, xa, y, xb, y);
  }
  
  // Bottom half
  for (f32 y = y2 + 1; y <= y3; y++) {
    f32
      xa = interpolate(y, y1, x1, y3, x3),
      xb = interpolate(y, y2, x2, y3, x3);
    if (xa > xb)
      swap(&xa, &xb);

    SDL_RenderLine(window->renderer, xa, y, xb, y);
  }
}

void graphics_draw_triangles_2d(
  Window* window,
  const Triangle2D* triangles, const u64 s_triangles,
  const Color color, const Color border_color, const u8 alpha
) {
  for (u64 i = 0; i < s_triangles; i++)
    graphics_draw_triangle_2d(window, triangles[i], color, border_color, alpha);
}

void graphics_draw_cube_3d(
  Window* window, const Camera* cam,
  const Cube3D* c, const Vec3D unit_vector, const Vec3D origin,
  const Color color, const u8 alpha
) {
  Vec3D vertices[8];

  const f32 s = c->half_s_edge;
  const Vec3D
    scaled_right1   = geometry_vec3d_mul(c->right, s),
    scaled_right2   = geometry_vec3d_mul(c->right, -s),
    scaled_up1      = geometry_vec3d_mul(c->up, s),
    scaled_up2      = geometry_vec3d_mul(c->up, -s),
    scaled_forward1 = geometry_vec3d_mul(c->forward, s),
    scaled_forward2 = geometry_vec3d_mul(c->forward, -s);
  
  Vec3D offsets[8] = {
    geometry_vec3d_add(geometry_vec3d_add(scaled_right1, scaled_up1), scaled_forward1),
    geometry_vec3d_add(geometry_vec3d_add(scaled_right2, scaled_up1), scaled_forward1),
    geometry_vec3d_add(geometry_vec3d_add(scaled_right2, scaled_up2), scaled_forward1),
    geometry_vec3d_add(geometry_vec3d_add(scaled_right1, scaled_up2), scaled_forward1),
    geometry_vec3d_add(geometry_vec3d_add(scaled_right1, scaled_up1), scaled_forward2),
    geometry_vec3d_add(geometry_vec3d_add(scaled_right2, scaled_up1), scaled_forward2),
    geometry_vec3d_add(geometry_vec3d_add(scaled_right2, scaled_up2), scaled_forward2),
    geometry_vec3d_add(geometry_vec3d_add(scaled_right1, scaled_up2), scaled_forward2)
  };

  for (int i = 0; i < 8; i++)
    vertices[i] = geometry_vec3d_add(c->center, offsets[i]);

  for (u8 i = 0; i < 4; i++)
    graphics_draw_line_3d(window, cam, vertices[i], vertices[(i + 1) % 4], unit_vector, origin, color, alpha);
  for (u8 i = 0; i < 4; i++)
    graphics_draw_line_3d(window, cam, vertices[4 + i], vertices[4 + ((i + 1) % 4)], unit_vector, origin, color, alpha);
  for (u8 i = 0; i < 4; i++)
    graphics_draw_line_3d(window, cam, vertices[i], vertices[4 + i], unit_vector, origin, color, alpha);
}

void graphics_delay(const u32 fps) {
  SDL_Delay((u32)(1 / ((float)(fps))));
}

void graphics_clear(Window* window, const Color color) {
  const struct ColorRGB rgb = color_map(color);
  SDL_SetRenderDrawColor(window->renderer, rgb.red, rgb.green, rgb.blue, 255);
  SDL_RenderClear(window->renderer);
}

void graphics_present(Window* window) {
  SDL_RenderPresent(window->renderer);
}

void graphics_close(Window* window) {
  if (window) {
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
    free(window);
  }
  SDL_Quit();
}

struct ColorRGB color_map(const Color c) {
  struct ColorRGB rgb;

  f32 r = c.r * 255.0f;
  if (r < 0)
    r = 0;
  if (r > 255)
    r = 255;
  rgb.red = (u8)r;

  f32 g = c.g * 255.0f;
  if (g < 0)
    g = 0;
  if (g > 255)
    g = 255;
  rgb.green = (u8)g;

  f32 b = c.b * 255.0f;
  if (b < 0)
    b = 0;
  if (b > 255)
    b = 255;
  rgb.blue = (u8)b;

  return rgb;
}

static f32 interpolate(const f32 y, const f32 y1, const f32 x1, const f32 y2, const f32 x2) {
  if (y2 == y1)
    return x1;
  return x1 + (x2 - x1) * (y - y1) / (y2 - y1);
}

static void swap(f32* x, f32* y) {
  f32 temp = *x;
  *x = *y;
  *y = temp;
}
