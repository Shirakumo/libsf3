#ifndef __SF3_IMAGE__
#define __SF3_IMAGE__
#include "sf3_core.h"

/// The format_id for an image file.
#define SF3_FORMAT_ID_IMAGE 0x03

/// The possible pixel channel formats.
enum sf3_pixel_format{
  /// The channel values are stored in 8-bit signed format.
  SF3_PIXEL_INT8 = 0x01,
  /// The channel values are stored in 16-bit signed format.
  SF3_PIXEL_INT16 = 0x02,
  /// The channel values are stored in 32-bit signed format.
  SF3_PIXEL_INT32 = 0x04,
  /// The channel values are stored in 64-bit signed format.
  SF3_PIXEL_INT64 = 0x08,
  /// The channel values are stored in 8-bit unsigned format.
  SF3_PIXEL_UINT8 = 0x11,
  /// The channel values are stored in 16-bit unsigned format.
  SF3_PIXEL_UINT16 = 0x12,
  /// The channel values are stored in 32-bit unsigned format.
  SF3_PIXEL_UINT32 = 0x14,
  /// The channel values are stored in 64-bit unsigned format.
  SF3_PIXEL_UINT64 = 0x18,
  /// The channel values are stored in 16-bit IEEE half-precision
  /// float format.
  SF3_PIXEL_FLOAT16 = 0x22,
  /// The channel values are stored in 32-bit IEEE single-precision
  /// float format.
  SF3_PIXEL_FLOAT32 = 0x24,
  /// The channel values are stored in 64-bit IEEE double-precision
  /// float format.
  SF3_PIXEL_FLOAT64 = 0x28
};

/// The possible pixel channel layouts.
enum sf3_channel_layout{
  /// Each pixel consists of a (grayscale) value channel.
  SF3_PIXEL_V = 0x01,
  /// Each pixel consists of a (grayscale) value and an alpha channel.
  SF3_PIXEL_VA = 0x02,
  /// Each pixel consists of a (truecolor) red, green, and blue
  /// channel.
  SF3_PIXEL_RGB = 0x03,
  /// Each pixel consists of a (truecolor) red, green, blue, and alpha
  /// channel.
  SF3_PIXEL_RGBA = 0x04,
  /// Each pixel consists of an alpha and a (grayscale) value channel.
  SF3_PIXEL_AV = 0x12,
  /// Each pixel consists of a (truecolor) blue, green, and red
  /// channel.
  SF3_PIXEL_BGR = 0x13,
  /// Each pixel consists of an alpha and a (truecolor) blue, green,
  /// and red channel.
  SF3_PIXEL_ABGR = 0x14,
  /// Each pixel consists of an alpha and a (truecolor) red, green,
  /// and blue channel.
  SF3_PIXEL_ARGB = 0x24,
  /// Each pixel consists of a (truecolor) blue, green, red, and alpha
  /// channel.
  SF3_PIXEL_BGRA = 0x34,
  /// Each pixel consists of a cyan, magenta, yellow, and black channel.
  SF3_PIXEL_CMYK = 0x44,
  /// Each pixel consists of a black, yellow, magenta, and cyan channel.
  SF3_PIXEL_KYMC = 0x54,
};

/// An image file.
///
/// See SF3_FORMAT_ID_IMAGE
struct PACK sf3_image{
  struct sf3_identifier identifier;
  /// The width of the image.
  uint32_t width;
  /// The height of the image.
  uint32_t height;
  /// The depth (number of layers) of the image.
  uint32_t depth;
  /// The channel layout of the pixels.
  uint8_t channels;
  /// The value format of the channels.
  uint8_t format;
  /// The raw pixel data laid out in row-major order.
  char pixels[];
};

/// Returns the number of bytes per channel.
int sf3_image_channel_size(const struct sf3_image *image){
  return image->format & 0x0F;
}

/// Returns the number of channels per pixel.
int sf3_image_channel_count(const struct sf3_image *image){
  return image->channels & 0x0F;
}

/// Returns the number of bytes per pixel.
int sf3_image_pixel_stride(const struct sf3_image *image){
  return (image->channels & 0x0F) * (image->format & 0x0F);
}

/// Returns a human-readable string representation of the format.
char *sf3_image_pixel_format(enum sf3_pixel_format format){
  switch(format){
  case SF3_PIXEL_INT8: return "int8";
  case SF3_PIXEL_INT16: return "int16";
  case SF3_PIXEL_INT32: return "int32";
  case SF3_PIXEL_INT64: return "int64";
  case SF3_PIXEL_UINT8: return "uint8";
  case SF3_PIXEL_UINT16: return "uint16";
  case SF3_PIXEL_UINT32: return "uint32";
  case SF3_PIXEL_UINT64: return "uint64";
  case SF3_PIXEL_FLOAT16: return "float2";
  case SF3_PIXEL_FLOAT32: return "float4";
  case SF3_PIXEL_FLOAT64: return "float8";
  default: "Unknown";
  }
}

/// Returns a human-readable string representation of the layout.
char *sf3_image_channel_layout(enum sf3_channel_layout format){
  switch(format){
  case SF3_PIXEL_V: return "V";
  case SF3_PIXEL_VA: return "VA";
  case SF3_PIXEL_RGB: return "RGB";
  case SF3_PIXEL_RGBA: return "RGBA";
  case SF3_PIXEL_AV: return "AV";
  case SF3_PIXEL_BGR: return "BGR";
  case SF3_PIXEL_ABGR: return "ABGR";
  case SF3_PIXEL_ARGB: return "ARGB";
  case SF3_PIXEL_BGRA: return "BGRA";
  case SF3_PIXEL_CMYK: return "CMYK";
  case SF3_PIXEL_KYMC: return "KYMC";
  default: return "Unknown";
  }
}
#endif
