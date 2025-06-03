#include "sf3_core.h"

#define SF3_FORMAT_ID_VECTOR_GRAPHIC 0x09

enum sf3_vector_instruction_type{
  SF3_INSTRUCTION_LINE = 0x01,
  SF3_INSTRUCTION_RECTANGLE = 0x02,
  SF3_INSTRUCTION_CIRCLE = 0x03,
  SF3_INSTRUCTION_POLYGON = 0x04,
  SF3_INSTRUCTION_CURVE = 0x05,
  SF3_INSTRUCTION_TEXT = 0x06,
  SF3_INSTRUCTION_IDENTITY = 0x11,
  SF3_INSTRUCTION_MATRIX = 0x12,
};

struct __attribute__((packed)) sf3_point{
  float x;
  float y;
};

struct __attribute__((packed)) sf3_size{
  float w;
  float h;
};

struct __attribute__((packed)) sf3_color{
  float r;
  float g;
  float b;
  float a;
};

struct __attribute__((packed)) sf3_shape_outline{
  uint16_t count;
  struct sf3_point edges[];
};

#define SF3_SKIP_OUTLINE(OUTLINE) ((OUTLINE).edges+(OUTLINE).count)

struct __attribute__((packed)) sf3_shape_bounds{
  float x;
  float y;
  float w;
  float h;
};

struct __attribute__((packed)) sf3_shape_fill{
  struct sf3_color fill_color;
  struct sf3_color outline_color;
  float outline_thickness;
};

struct __attribute__((packed)) sf3_text_instruction{
  uint8_t type;
  struct sf3_point point;
  struct sf3_color color;
  float font_size;
  sf3_str16 font;
};

struct __attribute__((packed)) sf3_curve_instruction{
  uint8_t type;
  struct sf3_shape_fill fill;
  struct sf3_shape_outline outline;
};

struct __attribute__((packed)) sf3_polygon_instruction{
  uint8_t type;
  struct sf3_shape_fill fill;
  struct sf3_shape_outline outline;
};

struct __attribute__((packed)) sf3_circle_instruction{
  uint8_t type;
  struct sf3_shape_fill fill;
  struct sf3_shape_bounds bounds;
};

struct __attribute__((packed)) sf3_rectangle_instruction{
  uint8_t type;
  struct sf3_shape_fill fill;
  struct sf3_shape_bounds bounds;
};

struct __attribute__((packed)) sf3_line_instruction{
  uint8_t type;
  struct sf3_color color;
  float thickness;
  struct sf3_shape_outline outline;
};

struct __attribute__((packed)) sf3_matrix_instruction{
  uint8_t type;
  float elements[6];  
};

struct __attribute__((packed)) sf3_identity_instruction{
  uint8_t type;
};

struct __attribute__((packed)) sf3_vector_instruction{
  uint8_t type;
};

struct __attribute__((packed)) sf3_vector_graphic{
  struct sf3_identifier identifier;
  uint32_t width;
  uint32_t height;
  uint32_t count;
  struct sf3_vector_instruction instructions[];
};

const struct sf3_vector_instruction *sf3_vector_graphic_next_instruction(const struct sf3_vector_instruction *instruction){
  const char *base = (char*)instruction;
  switch(instruction->type){
  case SF3_INSTRUCTION_RECTANGLE:
    return (struct sf3_vector_instruction *)(base+sizeof(struct sf3_rectangle_instruction));
  case SF3_INSTRUCTION_CIRCLE:
    return (struct sf3_vector_instruction *)(base+sizeof(struct sf3_circle_instruction));
  case SF3_INSTRUCTION_LINE:
    return (struct sf3_vector_instruction *)SF3_SKIP_OUTLINE(((struct sf3_line_instruction *)instruction)->outline);
  case SF3_INSTRUCTION_POLYGON:
    return (struct sf3_vector_instruction *)SF3_SKIP_OUTLINE(((struct sf3_polygon_instruction *)instruction)->outline);
  case SF3_INSTRUCTION_CURVE:
    return (struct sf3_vector_instruction *)SF3_SKIP_OUTLINE(((struct sf3_curve_instruction *)instruction)->outline);
  case SF3_INSTRUCTION_TEXT:{
    const struct sf3_text_instruction *i = (struct sf3_text_instruction *)instruction;
    return (struct sf3_vector_instruction *)(SF3_SKIP_STRP((sf3_str16 *)SF3_SKIP_STR(i->font)));
  }
  case SF3_INSTRUCTION_IDENTITY:
    return (struct sf3_vector_instruction *)(base+sizeof(struct sf3_identity_instruction));
  case SF3_INSTRUCTION_MATRIX:
    return (struct sf3_vector_instruction *)(base+sizeof(struct sf3_matrix_instruction));
  default:
    return 0;
  }
}

const char *sf3_text_instruction_string(const struct sf3_text_instruction *instruction){
  const sf3_str16 *string = (sf3_str16*)SF3_SKIP_STR(instruction->font);
  return string->str;
}

const char *sf3_vector_instruction_type(enum sf3_vector_instruction_type type){
  switch(type){
  case SF3_INSTRUCTION_LINE: return "line";
  case SF3_INSTRUCTION_RECTANGLE: return "rectangle";
  case SF3_INSTRUCTION_CIRCLE: return "circle";
  case SF3_INSTRUCTION_POLYGON: return "polygon";
  case SF3_INSTRUCTION_CURVE: return "curve";
  case SF3_INSTRUCTION_TEXT: return "text";
  case SF3_INSTRUCTION_IDENTITY: return "identity";
  case SF3_INSTRUCTION_MATRIX: return "matrix";
  default: return "Unknown";
  }
}
