#ifndef __SF3_MODEL__
#define __SF3_MODEL__
#include "sf3_core.h"

/// The format_id for a model file.
#define SF3_FORMAT_ID_MODEL 0x05

/// The material types that can be stored, as bit-field flags.
enum sf3_material_type{
  /// The texture stores the albedo colour.
  SF3_MATERIAL_ALBEDO = 0x01,
  /// The texture stores the normal vectors.
  SF3_MATERIAL_NORMAL = 0x02,
  /// The texture stores the metalness, roughness, and occlusion
  /// values in its R, G, and B channels respectively.
  SF3_MATERIAL_METALLIC = 0x04,
  /// The texture stores the metalness value.
  SF3_MATERIAL_METALNESS = 0x08,
  /// The texture stores the roughness value.
  SF3_MATERIAL_ROUGHNESS = 0x10,
  /// The texture stores the occlusion value.
  SF3_MATERIAL_OCCLUSION = 0x20,
  /// The texture stores the specular reflectivity value.
  SF3_MATERIAL_SPECULAR = 0x40,
  /// The texture stores the emission colour.
  SF3_MATERIAL_EMISSION = 0x80,
};

/// The vertex attributes that can be stored, as bit-field flags.
enum sf3_vertex_format{
  /// The vertex position vector as an X, Y, Z triplet.
  SF3_VERTEX_POSITION = 0x01,
  /// The texture coordinates as a U, V duplet.
  SF3_VERTEX_UV = 0x02,
  /// The vertex colour as an R, G, B triplet.
  SF3_VERTEX_COLOR = 0x04,
  /// The vertex normal vector as an X, Y, Z triplet.
  SF3_VERTEX_NORMAL = 0x08,
  /// The vertex tangent vector as an X, Y, Z triplet.
  SF3_VERTEX_TANGENT = 0x10,
};

/// An array of faces
struct PACK sf3_faces{
  /// The number of vertex indices stored.
  uint32_t count;
  /// The vertex indices, three of which make up a face.
  uint32_t faces[];
};

/// An array of vertex data.
struct PACK sf3_vertices{
  /// The number of floats stored.
  uint32_t count;
  /// The vertex data, packed according to the vertex format.
  float vertices[];
};

/// A 3D model file.
///
/// See SF3_FORMAT_ID_MODEL
struct PACK sf3_model{
  struct sf3_identifier identifier;
  /// The vertex attribute format description.
  uint8_t vertex_format;
  /// The material type description.
  uint8_t material_type;
  /// The size of the textures array in octets.
  uint32_t material_size;
  sf3_str16 textures[];
};

/// Returns the next texture after this one.
/// Note that this function does not perform any bounds checking
/// whatsoever. It is up to you to ensure you do not call this with
/// the last texture.
static inline const sf3_str16 *sf3_model_next_texture(const struct sf3_str16 *texture){
  return (const sf3_str16 *)SF3_SKIP_STRP(texture);
}

/// Returns the faces array of the model.
static inline const struct sf3_faces *sf3_model_faces(const struct sf3_model *model){
  return (const struct sf3_faces *)(((char*)model->textures)+model->material_size);
}

/// Returns the vertices array of the model.
static inline const struct sf3_vertices *sf3_model_vertices(const struct sf3_model *model){
  const struct sf3_faces *faces = sf3_model_faces(model);
  return (const struct sf3_vertices *)(faces->faces+faces->count);
}

/// Returns the number of textures stored in the model.
static inline uint8_t sf3_model_texture_count(const struct sf3_model *model){
  uint8_t type = model->material_type;
  return ((type >> 0) & 1) +
         ((type >> 1) & 1) +
         ((type >> 2) & 1) +
         ((type >> 3) & 1) +
         ((type >> 4) & 1) +
         ((type >> 5) & 1) +
         ((type >> 6) & 1) +
         ((type >> 7) & 1);
}

/// Returns the number of floats per vertex.
static inline uint8_t sf3_model_vertex_stride(const struct sf3_model *model){
  uint8_t type = model->vertex_format;
  return
    ((type >> 0) & 1) * 3 +
    ((type >> 1) & 1) * 2 +
    ((type >> 2) & 1) * 3 +
    ((type >> 3) & 1) * 3 +
    ((type >> 4) & 1) * 3;
}

/// Returns the number of floats per the specified attribute.
/// Note that you cannot call this with a combination of vertex format
/// type flags, only with a singular format type as outlined in the
/// `sf3_vertex_format` enumeration.
/// If the attribute is unknown, 0 is returned.
static inline uint8_t sf3_model_vertex_attribute_count(enum sf3_vertex_format format){
  switch(format){
  case SF3_VERTEX_POSITION: return 3;
  case SF3_VERTEX_UV: return 2;
  case SF3_VERTEX_COLOR: return 3;
  case SF3_VERTEX_NORMAL: return 3;
  case SF3_VERTEX_TANGENT: return 3;
  default: return 0;
  }
}

/// Returns the number of vertices stored in the model.
static inline uint32_t sf3_model_vertex_count(const struct sf3_model *model){
  return sf3_model_vertices(model)->count / sf3_model_vertex_stride(model);
}

/// Returns the material usage type for the given texture index in the model.
static enum sf3_material_type sf3_model_texture_material(const struct sf3_model *model, uint8_t index){
  uint8_t type = model->material_type;
  for(uint8_t i=0; i<8; ++i){
    uint8_t bit = (1 << i);
    if(index == 0)
      return bit;
    if(type & bit){
      index--;
    }
  }
  return 0xFF;
}

/// Returns a human-readable string describing the material type.
/// Note that you cannot call this with a combination of material
/// types flags, only with a singular type as outlined in the
/// `sf3_material_type` enumeration.
static const char *sf3_model_material_type(enum sf3_material_type type){
  switch(type){
  case SF3_MATERIAL_ALBEDO: return "albedo";
  case SF3_MATERIAL_NORMAL: return "normal";
  case SF3_MATERIAL_METALLIC: return "metallic";
  case SF3_MATERIAL_METALNESS: return "metalness";
  case SF3_MATERIAL_ROUGHNESS: return "roughness";
  case SF3_MATERIAL_OCCLUSION: return "occlusion";
  case SF3_MATERIAL_SPECULAR: return "specular";
  case SF3_MATERIAL_EMISSION: return "emission";
  default: return "Unknown";
  }
}
#endif
