#include "sf3-core.h"

#define SF3_FORMAT_ID_PHYSICS_MODEL 0x06

enum sf3_physics_shape_type{
  SF3_PHYSICS_SHAPE_ELLIPSOID = 0x01,
  SF3_PHYSICS_SHAPE_BOX = 0x02,
  SF3_PHYSICS_SHAPE_CYLINDER = 0x03,
  SF3_PHYSICS_SHAPE_PILL = 0x04,
  SF3_PHYSICS_SHAPE_MESH = 0x05,
};

struct sf3_shape_ellipsoid{
  float transform[16];
  uint8_t type;
  float w;
  float h;
  float d;
};

struct sf3_shape_box{
  float transform[16];
  uint8_t type;
  float w;
  float h;
  float d;
};

struct sf3_shape_cylinder{
  float transform[16];
  uint8_t type;
  float bottom_radius;
  float top_radius;
  float height;
};

struct sf3_shape_pill{
  float transform[16];
  uint8_t type;
  float bottom_radius;
  float top_radius;
  float height;
};

struct sf3_shape_mesh{
  float transform[16];
  uint8_t type;
  uint16_t count;
  float vertices[];
};

struct sf3_physics_shape{
  float transform[16];
  uint8_t type;
};

struct sf3_physics_model{
  struct sf3_identifier identifier;
  float mass;
  float tensor[9];
  uint16_t shape_count;
  struct sf3_physics_shape shapes[];
};

const struct sf3_physics_shape *sf3_physics_model_next_shape(const struct sf3_physics_shape *shape){
  switch(shape->type){
  case SF3_PHYSICS_SHAPE_ELLIPSOID:
    return (const struct sf3_physics_shape *)((const struct sf3_shape_ellipsoid *)shape)+1;
  case SF3_PHYSICS_SHAPE_BOX:
    return (const struct sf3_physics_shape *)((const struct sf3_shape_box *)shape)+1;
  case SF3_PHYSICS_SHAPE_CYLINDER:
    return (const struct sf3_physics_shape *)((const struct sf3_shape_cylinder *)shape)+1;
  case SF3_PHYSICS_SHAPE_PILL:
    return (const struct sf3_physics_shape *)((const struct sf3_shape_pill *)shape)+1;
  case SF3_PHYSICS_SHAPE_MESH:{
    const struct sf3_shape_mesh *mesh = (const struct sf3_shape_mesh *)shape;
    return (const struct sf3_physics_shape *)(mesh->vertices+mesh->count);
  }
  default: return 0;
  }
}
