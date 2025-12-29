#include "geometry.h"

f32 graphics_camera_horizon(const Camera* cam, const f32 screen_height) {
  return (screen_height / 2.0f) * (1 + tan(cam->pitch));
}

f32 geometry_scalar_abs(const f32 scalar) {
  return fabsf(scalar);
}

bool geometry_scalar_equals(const f32 lhs, const f32 rhs, const f32 epsi) {
  assert(epsi > 0.f);
  return geometry_scalar_abs(lhs - rhs) < epsi;
}

bool geometry_vec2d_equals(const Vec2D lhs, const Vec2D rhs, const f32 epsi) {
  return 
    geometry_scalar_equals(lhs.x, rhs.x, epsi) &&
    geometry_scalar_equals(lhs.y, rhs.y, epsi);
}

Vec2D geometry_vec2d_add(const Vec2D lhs, const Vec2D rhs) {
  return (Vec2D){ .x = lhs.x + rhs.x, .y = lhs.y + rhs.y };
}

Vec2D geometry_vec2d_mul(const Vec2D v, const f32 scalar) {
  return (Vec2D){ .x = v.x * scalar, .y = v.y * scalar };
}

Vec2D geometry_vec2d_mirror(const Vec2D v, const u8 dim) {
  Vec2D u = v;
  if (dim == 1)
    u.x *= -1;
  else if (dim == 2)
    u.y *= -1;
  return u;
}

bool geometry_vec3d_equals(const Vec3D lhs, const Vec3D rhs, const f32 epsi) {
  return 
    geometry_scalar_equals(lhs.x, rhs.x, epsi) &&
    geometry_scalar_equals(lhs.y, rhs.y, epsi) &&
    geometry_scalar_equals(lhs.z, rhs.z, epsi);
}

Vec3D geometry_vec3d_add(const Vec3D lhs, const Vec3D rhs) {
  return (Vec3D){ .x = lhs.x + rhs.x, .y = lhs.y + rhs.y, .z = lhs.z + rhs.z };
}

Vec3D geometry_vec3d_mul(const Vec3D v, const f32 scalar) {
  return (Vec3D){ .x = v.x * scalar, .y = v.y * scalar, .z = v.z * scalar };
}

Vec3D geometry_vec3d_mirror(const Vec3D v, const u8 dim) {
  Vec3D u = v;
  if (dim == 1)
    u.x *= -1;
  else if (dim == 2)
    u.y *= -1;
  else if (dim == 3)
    u.z *= -1;
  return u;
}

Vec3D geometry_camera_transform(const Camera* cam, const Vec3D point) {
  Vec3D relative = {
    point.x - cam->position.x,
    point.y - cam->position.y,
    point.z - cam->position.z
  };
  
  const f32
    cos_pitch = cosf(cam->pitch),
    sin_pitch = sinf(cam->pitch);
  
  const f32 
    y_rotated = relative.y * cos_pitch - relative.z * sin_pitch,
    z_rotated = relative.y * sin_pitch + relative.z * cos_pitch;
  
  const f32 
    cos_yaw = cosf(cam->yaw),
    sin_yaw = sinf(cam->yaw);
  
  const f32 
    x_final = relative.x * cos_yaw + z_rotated * sin_yaw,
    z_final = -relative.x * sin_yaw + z_rotated * cos_yaw;

  return (Vec3D){ .x = x_final, .y = y_rotated, .z = z_final };
}

Vec2D geometry_vec3d_to_2d(
  const Vec3D point, 
  const Vec3D unit_vector, 
  const Vec3D origin
) {
  return (Vec2D){
    .x = (point.x * unit_vector.x) / point.z + origin.x,
    .y = (-point.y * unit_vector.y) / point.z + origin.y
  };
}

void geometry_cube_rotate_xy(Cube3D* c, const f32 angle, const f32 dt) {
  const f32 
    theta = angle * dt,
    sin_theta = sin(theta),
    cos_theta = cos(theta);

  const f32 rx = c->right.x, ry = c->right.y;
  c->right.x = rx * cos_theta - ry * sin_theta;
  c->right.y = rx * sin_theta + ry * cos_theta;

  const f32 ux = c->up.x, uy = c->up.y;
  c->up.x = ux * cos_theta - uy * sin_theta;
  c->up.y = ux * sin_theta + uy * cos_theta;

  const f32 fx = c->forward.x, fy = c->forward.y;
  c->forward.x = fx * cos_theta - fy * sin_theta;
  c->forward.y = fx * sin_theta + fy * cos_theta;
}

void geometry_cube_rotate_yz(Cube3D* c, const f32 angle, const f32 dt) {
  const f32 
    theta = angle * dt,
    sin_theta = sin(theta),
    cos_theta = cos(theta);

  const f32 ry = c->right.y, rz = c->right.z;
  c->right.y = ry * cos_theta - rz * sin_theta;
  c->right.z = ry * sin_theta + rz * cos_theta;

  const f32 uy = c->up.y, uz = c->up.z;
  c->up.y = uy * cos_theta - uz * sin_theta;
  c->up.z = uy * sin_theta + uz * cos_theta;

  const f32 fy = c->forward.y, fz = c->forward.z;
  c->forward.y = fy * cos_theta - fz * sin_theta;
  c->forward.z = fy * sin_theta + fz * cos_theta;
}

void geometry_cube_rotate_xz(Cube3D* c, const f32 angle, const f32 dt) {
  const f32 
    theta = angle * dt,
    sin_theta = sin(theta),
    cos_theta = cos(theta);

  const f32 rx = c->right.x, rz = c->right.z;
  c->right.x = rx * cos_theta - rz * sin_theta;
  c->right.z = rx * sin_theta + rz * cos_theta;

  const f32 ux = c->up.x, uz = c->up.z;
  c->up.x = ux * cos_theta - uz * sin_theta;
  c->up.z = ux * sin_theta + uz * cos_theta;

  const f32 fx = c->forward.x, fz = c->forward.z;
  c->forward.x = fx * cos_theta - fz * sin_theta;
  c->forward.z = fx * sin_theta + fz * cos_theta;
}
