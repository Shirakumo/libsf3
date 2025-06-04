#include "sf3_core.h"

/// The format_id for a physics model file.
#define SF3_FORMAT_ID_PHYSICS_MODEL 0x06

/// The shape types that can be stored in a physics model.
enum sf3_physics_shape_type{
  /// The shape is an `sf3_shape_ellipsoid`.
  SF3_PHYSICS_SHAPE_ELLIPSOID = 0x01,
  /// The shape is an `sf3_shape_box`.
  SF3_PHYSICS_SHAPE_BOX = 0x02,
  /// The shape is an `sf3_shape_cylinder`.
  SF3_PHYSICS_SHAPE_CYLINDER = 0x03,
  /// The shape is an `sf3_shape_pill`.
  SF3_PHYSICS_SHAPE_PILL = 0x04,
  /// The shape is an `sf3_shape_mesh`.
  SF3_PHYSICS_SHAPE_MESH = 0x05,
};

/// An ellipsoid shape described by the radii along the principal
/// axes.
struct PACK sf3_shape_ellipsoid{
  float transform[16];
  uint8_t type;
  /// The radius along X.
  float w;
  /// The radius along Y.
  float h;
  /// The radius along Z.
  float d;
};

/// A box shape described by the half-widths along the principal axes.
struct PACK sf3_shape_box{
  float transform[16];
  uint8_t type;
  /// The half-size in X.
  float w;
  /// The half-size in Y.
  float h;
  /// The half-size in Z.
  float d;
};

/// A cylindrical shape described by two radii and a half-height.
struct PACK sf3_shape_cylinder{
  float transform[16];
  uint8_t type;
  /// The radius of the cap in Y-.
  float bottom_radius;
  /// The radius of the cap in Y+.
  float top_radius;
  /// The half-size in Y.
  float height;
};

/// A pill-box shape described by two radii and a half-height.
struct PACK sf3_shape_pill{
  float transform[16];
  uint8_t type;
  /// The radius of the sphere in Y-.
  float bottom_radius;
  /// The radius of the sphere in Y+.
  float top_radius;
  /// The half-size in Y between the two spheres.
  float height;
};

/// A convex hull described by its edge vertices.
struct PACK sf3_shape_mesh{
  float transform[16];
  uint8_t type;
  /// The number of vertices stored.
  uint16_t count;
  /// The vertices described as triplets of X, Y, Z coordinates.
  float vertices[];
};

/// A generic physics shape.
/// You should look at the `type` field to determine the kind of
/// shape, based on which a cast to a more precise shape can be done.
/// Each shape is defined as being centred on 0,0,0 with X growing to
/// the right, Y growing upwards, and Z growing "inwards"
/// (right-handed rule). The shape is then transformed by the encoded
/// transform matrix to place it into the coordinate system of the
/// entire physics-model.
struct PACK sf3_physics_shape{
  /// The affine transformation matrix of the shape stored as a 4x4
  /// matrix in row-major order.
  float transform[16];
  /// The type identifier of the physics shape.
  /// See the `sf3_physics_shape_type` enumeration.
  uint8_t type;
};

/// A physics model file.
///
/// See SF3_FORMAT_ID_PHYSICS_MODEL
struct PACK sf3_physics_model{
  struct sf3_identifier identifier;
  /// The mass of the entire model in Kg.
  float mass;
  /// The inertia tensor of the entire model stored as a 3x3 matrix in
  /// row-major order.
  float tensor[9];
  /// The number of shapes that comprise the model.
  uint16_t shape_count;
  struct sf3_physics_shape shapes[];
};

/// Access the next shape after this one.
/// Note that this function does not perform any bounds checking
/// whatsoever. It is up to you to ensure you do not call this with
/// the last shape of a physics model.
const struct sf3_physics_shape *sf3_physics_model_next_shape(const struct sf3_physics_shape *shape){
  const char *base = (char*)shape;
  switch(shape->type){
  case SF3_PHYSICS_SHAPE_ELLIPSOID:
    return (const struct sf3_physics_shape *)(base+sizeof(struct sf3_shape_ellipsoid));
  case SF3_PHYSICS_SHAPE_BOX:
    return (const struct sf3_physics_shape *)(base+sizeof(struct sf3_shape_box));
  case SF3_PHYSICS_SHAPE_CYLINDER:
    return (const struct sf3_physics_shape *)(base+sizeof(struct sf3_shape_cylinder));
  case SF3_PHYSICS_SHAPE_PILL:
    return (const struct sf3_physics_shape *)(base+sizeof(struct sf3_shape_pill));
  case SF3_PHYSICS_SHAPE_MESH:{
    const struct sf3_shape_mesh *mesh = (const struct sf3_shape_mesh *)shape;
    return (const struct sf3_physics_shape *)(mesh->vertices+mesh->count);
  }
  default: return 0;
  }
}

/// Return a human-readable string description of the shape type.
const char *sf3_physics_shape_type(enum sf3_physics_shape_type type){
  switch(type){
  case SF3_PHYSICS_SHAPE_ELLIPSOID: return "ellipsoid";
  case SF3_PHYSICS_SHAPE_BOX: return "box";
  case SF3_PHYSICS_SHAPE_CYLINDER: return "cylinder";
  case SF3_PHYSICS_SHAPE_PILL: return "pill";
  case SF3_PHYSICS_SHAPE_MESH: return "mesh";
  default: return "Unknown";
  }
}
