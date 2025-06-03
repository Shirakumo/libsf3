#include "sf3_core.h"

#define SF3_FORMAT_ID_MODEL 0x05

enum sf3_material_type{
  SF3_MATERIAL_ALBEDO = 0x01,
  SF3_MATERIAL_NORMAL = 0x02,
  SF3_MATERIAL_METALLIC = 0x04,
  SF3_MATERIAL_METALNESS = 0x08,
  SF3_MATERIAL_ROUGHNESS = 0x10,
  SF3_MATERIAL_OCCLUSION = 0x20,
  SF3_MATERIAL_SPECULAR = 0x40,
  SF3_MATERIAL_EMISSION = 0x80,
};

enum sf3_vertex_format{
  SF3_VERTEX_POSITION = 0x01,
  SF3_VERTEX_UV = 0x02,
  SF3_VERTEX_COLOR = 0x04,
  SF3_VERTEX_NORMAL = 0x08,
  SF3_VERTEX_TANGENT = 0x10,
};

struct PACK sf3_faces{
  uint32_t count;
  uint32_t faces[];
};

struct PACK sf3_vertices{
  uint32_t count;
  float vertices[];
};

struct PACK sf3_model{
  struct sf3_identifier identifier;
  uint8_t vertex_format;
  uint8_t material_type;
  uint32_t material_size;
  sf3_str16 textures[];
};

const sf3_str16 *sf3_model_next_texture(const struct sf3_str16 *texture){
  return (const sf3_str16 *)SF3_SKIP_STRP(texture);
}

const struct sf3_faces *sf3_model_faces(const struct sf3_model *model){
  return (const struct sf3_faces *)(((char*)model->textures)+model->material_size);
}

const struct sf3_vertices *sf3_model_vertices(const struct sf3_model *model){
  const struct sf3_faces *faces = sf3_model_faces(model);
  return (const struct sf3_vertices *)(faces->faces+faces->count);
}

uint8_t sf3_model_texture_count(const struct sf3_model *model){
  uint8_t type = model->material_type;
  return
    (type >> 0) & 1 +
    (type >> 1) & 1 +
    (type >> 2) & 1 +
    (type >> 3) & 1 +
    (type >> 4) & 1 +
    (type >> 5) & 1 +
    (type >> 6) & 1 +
    (type >> 7) & 1;
}

uint8_t sf3_model_vertex_stride(const struct sf3_model *model){
  uint8_t type = model->vertex_format;
  return
    ((type >> 0) & 1) * 3 +
    ((type >> 1) & 1) * 2 +
    ((type >> 2) & 1) * 3 +
    ((type >> 3) & 1) * 3 +
    ((type >> 4) & 1) * 3;
}

uint8_t sf3_model_vertex_attribute_count(enum sf3_vertex_format format){
  switch(format){
  case SF3_VERTEX_POSITION: return 3;
  case SF3_VERTEX_UV: return 2;
  case SF3_VERTEX_COLOR: return 3;
  case SF3_VERTEX_NORMAL: return 3;
  case SF3_VERTEX_TANGENT: return 3;
  default: return 0;
  }
}

uint32_t sf3_model_vertex_count(const struct sf3_model *model){
  return sf3_model_vertices(model)->count / sf3_model_vertex_stride(model);
}

enum sf3_material_type sf3_model_texture_material(const struct sf3_model *model, uint8_t index){
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

const char *sf3_model_material_type(enum sf3_material_type type){
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
