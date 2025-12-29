#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

typedef struct vector2d {
  f32 x, y;
} Vec2D;

typedef struct vector3d {
  f32 x, y, z;
} Vec3D;

typedef struct line2d {
  Vec2D start, end;
} Line2D;

typedef struct triangle2d {
  Vec2D v1, v2, v3;
} Triangle2D;

typedef struct triangle3d {
  Vec3D v1, v2, v3;
} Triangle3D;

typedef struct cube3D {
  Vec3D 
    center,
    right,   // X-axis direction
    up,      // Y-axis direction
    forward; // Z-axis direction
  f32 half_s_edge;
} Cube3D;

typedef struct camera {
  Vec3D position;
  f32
    pitch,
    yaw,
    fov
    near_plane;
} Camera;

f32   geometry_camera_horizon   (const Camera*, const f32);
Vec3D geometry_camera_transform (const Camera*, const Vec3D);

f32   geometry_scalar_abs     (const f32);
bool  geometry_scalar_equals  (const f32, const f32, const f32);

bool  geometry_vec2d_equals   (const Vec2D, const Vec2D, const f32);
Vec2D geometry_vec2d_add      (const Vec2D, const Vec2D);
Vec2D geometry_vec2d_mul      (const Vec2D, const f32);
Vec2D geometry_vec2d_mirror   (const Vec2D, const u8);

bool  geometry_vec3d_equals   (const Vec3D, const Vec3D, const f32);
Vec3D geometry_vec3d_add      (const Vec3D, const Vec3D);
Vec3D geometry_vec3d_mul      (const Vec3D, const f32);
Vec3D geometry_vec3d_mirror   (const Vec3D, const u8);
Vec2D geometry_vec3d_to_2d    (const Vec3D, const Vec3D, const Vec3D);

void  geometry_cube_rotate_xy (Cube3D*, const f32, const f32);
void  geometry_cube_rotate_yz (Cube3D*, const f32, const f32);
void  geometry_cube_rotate_xz (Cube3D*, const f32, const f32);

#endif /* __GEOMETRY_H__ */
