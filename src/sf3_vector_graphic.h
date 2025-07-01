#ifndef __SF3_VECTOR_GRAPHIC__
#define __SF3_VECTOR_GRAPHIC__
#include "sf3_core.h"

/// The format_id for a vector graphic file.
#define SF3_FORMAT_ID_VECTOR_GRAPHIC 0x09

/// The vector instructions that can be stored in a vector graphic
/// file.
enum sf3_vector_instruction_type{
  /// The instruction is a `sf3_line_instruction`.
  SF3_INSTRUCTION_LINE = 0x01,
  /// The instruction is a `sf3_rectangle_instruction`.
  SF3_INSTRUCTION_RECTANGLE = 0x02,
  /// The instruction is a `sf3_circle_instruction`.
  SF3_INSTRUCTION_CIRCLE = 0x03,
  /// The instruction is a `sf3_polygon_instruction`.
  SF3_INSTRUCTION_POLYGON = 0x04,
  /// The instruction is a `sf3_curve_instruction`.
  SF3_INSTRUCTION_CURVE = 0x05,
  /// The instruction is a `sf3_text_instruction`.
  SF3_INSTRUCTION_TEXT = 0x06,
  /// The instruction is a `sf3_identity_instruction`.
  SF3_INSTRUCTION_IDENTITY = 0x11,
  /// The instruction is a `sf3_matrix_instruction`.
  SF3_INSTRUCTION_MATRIX = 0x12,
};

/// Description of a point on the canvas.
struct SF3_PACK sf3_point{
  /// The X coordinate.
  float x;
  /// The Y coordinate.
  float y;
};

/// Description of an extent on the canvas.
struct SF3_PACK sf3_size{
  /// The width in X.
  float w;
  /// The height in Y.
  float h;
};

/// Description of a colour of a shape.
struct SF3_PACK sf3_color{
  /// The red channel, in the range of [0,1].
  float r;
  /// The green channel, in the range of [0,1].
  float g;
  /// The blue channel, in the range of [0,1].
  float b;
  /// The alpha channel, in the range of [0,1].
  float a;
};

/// Description of a shape's outline.
struct SF3_PACK sf3_shape_outline{
  /// The number of edges stored in the outline.
  uint16_t count;
  /// The edge points of the outline.
  struct sf3_point edges[];
};

/// Macro to obtain a pointer past the outline structure.
#define SF3_SKIP_OUTLINE(OUTLINE) ((OUTLINE).edges+(OUTLINE).count)

/// Description of a shape's bounds or extent on the canvas.
struct SF3_PACK sf3_shape_bounds{
  /// The X coordinate of the lower left corner.
  float x;
  /// The Y coordinate of the lower left corner.
  float y;
  /// The width in X of the shape.
  float w;
  /// The height in Y of the shape.
  float h;
};

/// Description of the fill parameters of a shape.
struct SF3_PACK sf3_shape_fill{
  /// The colour to fill the insides of the shape's outline with.
  struct sf3_color fill_color;
  /// The colour to fill the outline stroke of the shape with.
  struct sf3_color outline_color;
  /// The thickness of the outline stroke of the shape.
  float outline_thickness;
};

/// An instruction to draw text.
/// See `sf3_text_instruction_string`
struct SF3_PACK sf3_text_instruction{
  uint8_t type;
  /// The center point of the baseline of the first character.
  struct sf3_point point;
  /// The colour to fill the text glyphs with.
  struct sf3_color color;
  /// The size of the font in em units.
  float font_size;
  /// The name of the font-family to use for the text.
  sf3_str16 font;
};

/// An instruction to draw a Bezier curve.
struct SF3_PACK sf3_curve_instruction{
  uint8_t type;
  /// The shapes' drawing parameters.
  struct sf3_shape_fill fill;
  /// The actual outline of the bezier curve, as shared quadruplets of
  /// points, where the first and every third point after is an edge
  /// point, and the points between are control points.
  struct sf3_shape_outline outline;
};

/// An instruction to draw a closed polygon.
struct SF3_PACK sf3_polygon_instruction{
  uint8_t type;
  /// The shapes' drawing parameters.
  struct sf3_shape_fill fill;
  /// The edge points of the outline of the polygon as in
  /// counter-clockwise order.
  struct sf3_shape_outline outline;
};

/// An instruction to draw a circle.
struct SF3_PACK sf3_circle_instruction{
  uint8_t type;
  /// The shapes' drawing parameters.
  struct sf3_shape_fill fill;
  /// The bounding box of the shape.
  struct sf3_shape_bounds bounds;
};

/// An instruction to draw a rectangle.
struct SF3_PACK sf3_rectangle_instruction{
  uint8_t type;
  /// The shapes' drawing parameters.
  struct sf3_shape_fill fill;
  /// The bounding box of the shape.
  struct sf3_shape_bounds bounds;
};

/// An instruction to draw a segmented line.
struct SF3_PACK sf3_line_instruction{
  uint8_t type;
  /// The colour to fill the stroke of the line with.
  struct sf3_color color;
  /// The thickness of the stroke of the line.
  float thickness;
  /// The edge points of the contiguous line segments.
  struct sf3_shape_outline outline;
};

/// An instruction to apply a transformation matrix to the
/// instructions following after this one.
struct SF3_PACK sf3_matrix_instruction{
  uint8_t type;
  /// The elements of the 3x2 transform matrix stored in row-major
  /// order.
  float elements[6];
};

/// An instruction to apply an identity transformation matrix to the
/// instructions folllowing after this one.
struct SF3_PACK sf3_identity_instruction{
  uint8_t type;
};

/// Generic representation of a vector drawing instruction.
/// See the type for the precise type stored within.
struct SF3_PACK sf3_vector_instruction{
  /// See the `sf3_vector_instruction_type` enumeration for possible
  /// values.
  uint8_t type;
};

/// A vector graphic file.
///
/// See SF3_FORMAT_ID_VECTOR_GRAPHIC
struct SF3_PACK sf3_vector_graphic{
  struct sf3_identifier identifier;
  /// The width of the canvas in pixels.
  uint32_t width;
  /// The height of the canvas in pixels.
  uint32_t height;
  /// The number of vector instructions stored.
  uint32_t count;
  /// The instructions to execute in order to draw the graphic.
  struct sf3_vector_instruction instructions[];
};

/// Returns the next instruction after this one.
/// Note that this function does not perform any bounds checking
/// whatsoever. It is up to you to ensure you do not call this with
/// the last instruction of a vector graphic file.
SF3_INLINE const struct sf3_vector_instruction *sf3_vector_graphic_next_instruction(const struct sf3_vector_instruction *instruction){
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

/// Returns the string to draw of a text instruction.
SF3_INLINE const char *sf3_text_instruction_string(const struct sf3_text_instruction *instruction){
  const sf3_str16 *string = (sf3_str16*)SF3_SKIP_STR(instruction->font);
  return string->str;
}

/// Returns a human-readable string description of the vector
/// instruction type.
SF3_EXPORT const char *sf3_vector_instruction_type(enum sf3_vector_instruction_type type){
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
#endif
