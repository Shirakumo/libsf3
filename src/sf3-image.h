#include "sf3-core.h"

#define SF3_FORMAT_ID_IMAGE 0x03

enum sf3_pixel_format{
  SF3_PIXEL_INT8 = 0x01,
  SF3_PIXEL_INT16 = 0x02,
  SF3_PIXEL_INT32 = 0x04,
  SF3_PIXEL_INT64 = 0x08,
  SF3_PIXEL_UINT8 = 0x11,
  SF3_PIXEL_UINT16 = 0x12,
  SF3_PIXEL_UINT32 = 0x14,
  SF3_PIXEL_UINT64 = 0x18,
  SF3_PIXEL_FLOAT2 = 0x22,
  SF3_PIXEL_FLOAT4 = 0x24,
  SF3_PIXEL_FLOAT8 = 0x28
};

enum sf3_channel_layout{
  SF3_PIXEL_V = 0x01,
  SF3_PIXEL_VA = 0x02,
  SF3_PIXEL_RGB = 0x03,
  SF3_PIXEL_RGBA = 0x04,
  SF3_PIXEL_AV = 0x12,
  SF3_PIXEL_BGR = 0x13,
  SF3_PIXEL_ABGR = 0x14,
  SF3_PIXEL_ARGB = 0x24,
  SF3_PIXEL_BGRA = 0x34,
  SF3_PIXEL_CMYK = 0x44,
  SF3_PIXEL_KYMC = 0x54,
};

struct __attribute__((packed)) sf3_image{
  struct sf3_identifier identifier;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint8_t channels;
  uint8_t format;
  char pixels[];
};

int sf3_image_channel_size(const struct sf3_image *image){
  return image->format & 0x0F;
}

int sf3_image_channel_count(const struct sf3_image *image){
  return image->channels & 0x0F;
}

int sf3_image_pixel_stride(const struct sf3_image *image){
  return (image->channels & 0x0F) * (image->format & 0x0F);
}

char *sf3_image_pixel_format(enum sf3_pixel_format format){
  switch(format){
  case SF3_PIXEL_INT8: return "INT8";
  case SF3_PIXEL_INT16: return "INT16";
  case SF3_PIXEL_INT32: return "INT32";
  case SF3_PIXEL_INT64: return "INT64";
  case SF3_PIXEL_UINT8: return "UINT8";
  case SF3_PIXEL_UINT16: return "UINT16";
  case SF3_PIXEL_UINT32: return "UINT32";
  case SF3_PIXEL_UINT64: return "UINT64";
  case SF3_PIXEL_FLOAT2: return "FLOAT2";
  case SF3_PIXEL_FLOAT4: return "FLOAT4";
  case SF3_PIXEL_FLOAT8: return "FLOAT8";
  default: "Unknown";
  }
}

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
