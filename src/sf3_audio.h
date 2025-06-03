#include "sf3_core.h"

#define SF3_FORMAT_ID_AUDIO 0x02

enum sf3_audio_format{
  SF3_SAMPLE_ALAW = 0x01,
  SF3_SAMPLE_INT16 = 0x02,
  SF3_SAMPLE_INT32 = 0x04,
  SF3_SAMPLE_INT64 = 0x08,
  SF3_SAMPLE_ULAW = 0x11,
  SF3_SAMPLE_UINT16 = 0x12,
  SF3_SAMPLE_UINT32 = 0x14,
  SF3_SAMPLE_UINT64 = 0x18,
  SF3_SAMPLE_FLOAT2 = 0x22,
  SF3_SAMPLE_FLOAT4 = 0x24,
  SF3_SAMPLE_FLOAT8 = 0x28
};

struct PACK sf3_audio{
  struct sf3_identifier identifier;
  uint32_t samplerate;
  uint8_t channels;
  uint8_t format;
  uint64_t frame_count;
  char samples[];
};

int sf3_audio_sample_size(const struct sf3_audio *audio){
  return audio->format & 0x0F;
}

int sf3_audio_frame_size(const struct sf3_audio *audio){
  return (audio->format & 0x0F) * audio->channels;
}

float sf3_audio_duration(const struct sf3_audio *audio){
  return ((double)audio->frame_count) / audio->samplerate;
}

char *sf3_audio_format(enum sf3_audio_format format){
  switch(format){
  case SF3_SAMPLE_ALAW: return "ALAW";
  case SF3_SAMPLE_INT16: return "INT16";
  case SF3_SAMPLE_INT32: return "INT32";
  case SF3_SAMPLE_INT64: return "INT64";
  case SF3_SAMPLE_ULAW: return "ULAW";
  case SF3_SAMPLE_UINT16: return "UINT16";
  case SF3_SAMPLE_UINT32: return "UINT32";
  case SF3_SAMPLE_UINT64: return "UINT64";
  case SF3_SAMPLE_FLOAT2: return "FLOAT2";
  case SF3_SAMPLE_FLOAT4: return "FLOAT4";
  case SF3_SAMPLE_FLOAT8: return "FLOAT8";
  default: return "Unknown";
  }
}
