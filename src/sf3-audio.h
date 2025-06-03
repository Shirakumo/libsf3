#include "sf3-core.h"

#define SF3_FORMAT_ID_AUDIO 0x02

enum SF3_AUDIO_FORMAT{
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

struct __attribute__((packed)) sf3_audio{
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
