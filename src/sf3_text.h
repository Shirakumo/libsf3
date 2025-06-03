#include "sf3_core.h"

#define SF3_FORMAT_ID_TEXT 0x08

enum sf3_markup_option_type{
  SF3_MARKUP_BOLD = 0x01,
  SF3_MARKUP_ITALIC = 0x02,
  SF3_MARKUP_UNDERLINE = 0x03,
  SF3_MARKUP_STRIKE = 0x04,
  SF3_MARKUP_MONO = 0x05,
  SF3_MARKUP_COLOR = 0x06,
  SF3_MARKUP_SIZE = 0x07,
  SF3_MARKUP_HEADING = 0x08,
  SF3_MARKUP_LINK = 0x09,
  SF3_MARKUP_TARGET = 0x0A,
  SF3_MARKUP_FONT = 0x0B,
};

struct __attribute__((packed)) sf3_markup{
  uint64_t start;
  uint64_t end;
  uint8_t option_type;
};

struct __attribute__((packed)) sf3_markup_color{
  struct sf3_markup markup;
  float r;
  float g;
  float b;
};

struct __attribute__((packed)) sf3_markup_size{
  struct sf3_markup markup;
  float size;
};

struct __attribute__((packed)) sf3_markup_heading{
  struct sf3_markup markup;
  uint8_t level;
};

struct __attribute__((packed)) sf3_markup_link{
  struct sf3_markup markup;
  sf3_str16 address;
};

struct __attribute__((packed)) sf3_markup_target{
  struct sf3_markup markup;
  sf3_str16 address;
};

struct __attribute__((packed)) sf3_markup_font{
  struct sf3_markup markup;
  sf3_str16 font;
};

struct __attribute__((packed)) sf3_text{
  struct sf3_identifier identifier;
  uint64_t markup_size;
  uint32_t markup_count;
  struct sf3_markup markup[];
};

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

const char *sf3_text_string(const struct sf3_text *text){
  const char *base = (char *)text->markup;
  base += text->markup_size;
  return ((sf3_str64*)base)->str;
}

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
