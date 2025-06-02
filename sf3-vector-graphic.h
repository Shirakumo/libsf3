#include "sf3-core.h"

#define SF3_FORMAT_ID_VECTOR_GRAPHIC 0x09

enum sf3_vector_instruction{
  SF3_INSTRUCTION_LINE = 0x01,
  SF3_INSTRUCTION_RECTANGLE = 0x02,
  SF3_INSTRUCTION_CIRCLE = 0x03,
  SF3_INSTRUCTION_POLYGON = 0x04,
  SF3_INSTRUCTION_CURVE = 0x05,
  SF3_INSTRUCTION_TEXT = 0x06,
  SF3_INSTRUCTION_IDENTITY = 0x11,
  SF3_INSTRUCTION_MATRIX = 0x12,
};

struct sf3_point{
  float x;
  float y;
};

struct sf3_size{
  float w;
  float h;
};

struct sf3_color{
  float r;
  float g;
  float b;
  float a;
};

struct sf3_shape_outline{
  uint16_t count;
  struct sf3_point edges[];
};

struct sf3_shape_bounds{
  float x;
  float y;
  float w;
  float h;
};

struct sf3_shape_fill{
  struct sf3_color fill_color;
  struct sf3_color outline_color;
  float outline_thickness;
};

struct sf3_text_instruction{
  uint8_t type;
  struct sf3_point point;
  struct sf3_color color;
  float font_size;
  sf3_str16 font;
};

struct sf3_curve_instruction{
  uint8_t type;
  struct sf3_shape_outline outline;
  struct sf3_shape_fill fill;
};

struct sf3_polygon_instruction{
  uint8_t type;
  struct sf3_shape_outline outline;
  struct sf3_shape_fill fill;
};

struct sf3_circle_instruction{
  uint8_t type;
  struct sf3_shape_bounds bounds;
  struct sf3_shape_fill fill;
};

struct sf3_rectangle_instruction{
  uint8_t type;
  struct sf3_shape_bounds bounds;
  struct sf3_shape_fill fill;
};

struct sf3_line_instruction{
  uint8_t type;
  struct sf3_shape_outline outline;
  struct sf3_color color;
  float thickness;
};

struct sf3_matrix_instruction{
  uint8_t type;
  float elements[6];  
};

struct sf3_identity_instruction{
  uint8_t type;
};

struct sf3_vector_instruction{
  uint8_t type;
};

struct sf3_vector_graphic{
  struct sf3_identifier identifier;
  uint32_t width;
  uint32_t height;
  uint32_t count;
  struct sf3_vector_instruction[];
};

const struct sf3_vector_instruction *sf3_vector_graphic_next_instruction(const struct sf3_vector_instruction *instruction){
  switch(instruction->type){
  case SF3_INSTRUCTION_LINE:
    return (struct sf3_vector_instruction *)((struct sf3_line_instruction *)instruction)+1;
  case SF3_INSTRUCTION_RECTANGLE:
    return (struct sf3_vector_instruction *)((struct sf3_rectangle_instruction *)instruction)+1;
  case SF3_INSTRUCTION_CIRCLE:
    return (struct sf3_vector_instruction *)((struct sf3_circle_instruction *)instruction)+1;
  case SF3_INSTRUCTION_POLYGON:
    return (struct sf3_vector_instruction *)((struct sf3_polygon_instruction *)instruction)+1;
  case SF3_INSTRUCTION_CURVE:
    return (struct sf3_vector_instruction *)((struct sf3_curve_instruction *)instruction)+1;
  case SF3_INSTRUCTION_TEXT:{
    // More complex logic is necessary due to the variable-length of the instruction.
    const struct sf3_text_instruction *i = (struct sf3_text_instruction *)instruction;
    sf3_str16 *string = ((char*)&instruction->font.str)+instruction->font.length;
    return (struct sf3_vector_instruction *)(string->str+string->length);
  }
  case SF3_INSTRUCTION_IDENTITY:
    return (struct sf3_vector_instruction *)((struct sf3_identity_instruction *)instruction)+1;
  case SF3_INSTRUCTION_MATRIX:
    return (struct sf3_vector_instruction *)((struct sf3_matrix_instruction *)instruction)+1;
  default:
    return 0;
  }
}

const char *sf3_text_instruction_string(const struct sf3_text_instruction instruction){
  sf3_str16 *string = ((char*)&instruction->font.str)+instruction->font.length;
  return string.str;
}
