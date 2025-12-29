#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "utils.h"
#include "geometry.h"

typedef struct window Window;

typedef struct {
  u32 count;      // Number of triangles currently in the queue
  u32 capacity;   // Max triangles allocated

  f32 *v1x, *v1y, *v1z;
  f32 *v1r, *v1g, *v1b;

  f32 *v2x, *v2y, *v2z;
  f32 *v2r, *v2g, *v2b;

  f32 *v3x, *v3y, *v3z;
  f32 *v3r, *v3g, *v3b;
} RenderQueueSoA;

typedef struct color {
  f32 r, g, b;
} Color;

Window* graphics_init              (const char*, const u32, const u32);

void    graphics_draw_points       (Window*, const Vec2D*, const u64, const Color, const u8);

void    graphics_draw_line_2d      (Window*, const Line2D, const Color, const u8);
void    graphics_draw_lines_2d     (Window*, const Line2D*, const u64, const Color, const u8);
void    graphics_draw_line_3d      (Window*, const Camera*, const Vec3D, const Vec3D, 
                                    const Vec3D, const Vec3D, const Color, const u8);

void    graphics_draw_triangle_2d  (Window*, const Triangle2D, const Color, const Color, const u8);
void    graphics_draw_triangles_2d (Window*, const Triangle2D*, const u64, const Color, const Color, const u8);

void    graphics_draw_cube_3d      (Window*, const Camera*, const Cube3D*, const Vec3D, const Vec3D, const Color, const u8);

void    graphics_delay             (const u32);
void    graphics_clear             (Window*, const Color);
void    graphics_present           (Window*);

void    graphics_close             (Window*);

#endif /* __GRAPHICS_H__ */
