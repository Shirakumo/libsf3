#include "sf3_core.h"

/// The format_id for a text file.
#define SF3_FORMAT_ID_TEXT 0x08

/// The possible markup option types.
enum sf3_markup_option_type{
  /// The option marks bolding.
  SF3_MARKUP_BOLD = 0x01,
  /// The option marks italicisation.
  SF3_MARKUP_ITALIC = 0x02,
  /// The option marks underlining.
  SF3_MARKUP_UNDERLINE = 0x03,
  /// The option marks strikethrough.
  SF3_MARKUP_STRIKE = 0x04,
  /// The option marks monospacing.
  SF3_MARKUP_MONO = 0x05,
  /// The option marks color and is a `sf3_markup_color`.
  SF3_MARKUP_COLOR = 0x06,
  /// The option marks font size and is a `sf3_markup_size`.
  SF3_MARKUP_SIZE = 0x07,
  /// The option marks a heading and is a `sf3_markup_heading`.
  SF3_MARKUP_HEADING = 0x08,
  /// The option marks a hyperlink and is a `sf3_markup_link`.
  SF3_MARKUP_LINK = 0x09,
  /// The option marks a link target and is a `sf3_markup_target`.
  SF3_MARKUP_TARGET = 0x0A,
  /// The option marks a font switch and is a `sf3_markup_font`.
  SF3_MARKUP_FONT = 0x0B,
};

/// Generic markup option.
/// See the `option_type` for the specific kind of option this block
/// might be describing.
struct PACK sf3_markup{
  /// The starting index of the markup after and including which the
  /// markup should apply.
  uint64_t start;
  /// The ending index of the markup before which the markup should
  /// apply.
  uint64_t end;
  /// The identifier of the specific type of markup that this is.
  /// See the `sf3_markup_option_type` enumeration.
  uint8_t option_type;
};

/// Description of a color change.
struct PACK sf3_markup_color{
  struct sf3_markup markup;
  /// The red channel, in the range of [0,1].
  float r;
  /// The green channel, in the range of [0,1].
  float g;
  /// The blue channel, in the range of [0,1].
  float b;
};

/// Description of a font size change.
struct PACK sf3_markup_size{
  struct sf3_markup markup;
  /// The size of the font in em units.
  float size;
};

/// Description of a heading.
struct PACK sf3_markup_heading{
  struct sf3_markup markup;
  /// The level of the heading as a positive integer.
  uint8_t level;
};

/// Description of a hyperlink.
struct PACK sf3_markup_link{
  struct sf3_markup markup;
  /// The address of the link, either the address of a corresponding
  /// target markup, or an URL.
  sf3_str16 address;
};

/// Description of a link target.
struct PACK sf3_markup_target{
  struct sf3_markup markup;
  /// The address of this target.
  sf3_str16 address;
};

/// Description of a font family change.
struct PACK sf3_markup_font{
  struct sf3_markup markup;
  /// The name of the font-family to select.
  sf3_str16 font;
};

/// A text file.
///
/// See SF3_FORMAT_ID_TEXT
struct PACK sf3_text{
  struct sf3_identifier identifier;
  /// The size of the markup options in bytes.
  uint64_t markup_size;
  /// The number of markup options stored.
  uint32_t markup_count;
  /// The markup options.
  struct sf3_markup markup[];
};

/// Returns the next markup option after this one.
/// Note that this function does not perform any bounds checking
/// whatsoever. It is up to you to ensure you do not call this with
/// the last markup option.
const struct sf3_markup *sf3_text_next_markup(const struct sf3_markup *markup){
  const char *base = (char*)markup;
  switch(markup->option_type){
  case SF3_MARKUP_BOLD:
  case SF3_MARKUP_ITALIC:
  case SF3_MARKUP_UNDERLINE:
  case SF3_MARKUP_STRIKE:
  case SF3_MARKUP_MONO:
    return (struct sf3_markup *) (base+sizeof(struct sf3_markup));
  case SF3_MARKUP_COLOR:
    return (struct sf3_markup *) (base+sizeof(struct sf3_markup_color));
  case SF3_MARKUP_SIZE:
    return (struct sf3_markup *) (base+sizeof(struct sf3_markup_size));
  case SF3_MARKUP_HEADING:
    return (struct sf3_markup *) (base+sizeof(struct sf3_markup_heading));
  case SF3_MARKUP_LINK:
    return (struct sf3_markup *) SF3_SKIP_STR(((struct sf3_markup_link *)markup)->address);
  case SF3_MARKUP_TARGET:
    return (struct sf3_markup *) SF3_SKIP_STR(((struct sf3_markup_target *)markup)->address);
  case SF3_MARKUP_FONT:
    return (struct sf3_markup *) SF3_SKIP_STR(((struct sf3_markup_font *)markup)->font);
  default:
    return 0;
  }
}

/// Returns the raw textual contents.
const char *sf3_text_string(const struct sf3_text *text){
  const char *base = (char *)text->markup;
  base += text->markup_size;
  return ((sf3_str64*)base)->str;
}

/// Returns a human-readable string description of the markup option.
const char *sf3_text_markup_option_type(enum sf3_markup_option_type type){
  switch(type){
  case SF3_MARKUP_BOLD: return "bold";
  case SF3_MARKUP_ITALIC: return "italic";
  case SF3_MARKUP_UNDERLINE: return "underline";
  case SF3_MARKUP_STRIKE: return "strike";
  case SF3_MARKUP_MONO: return "mono";
  case SF3_MARKUP_COLOR: return "color";
  case SF3_MARKUP_SIZE: return "size";
  case SF3_MARKUP_HEADING: return "heading";
  case SF3_MARKUP_LINK: return "link";
  case SF3_MARKUP_TARGET: return "target";
  case SF3_MARKUP_FONT: return "font";
  default: return "Unknown";
  }
}
