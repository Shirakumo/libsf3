#ifndef __SF3_AUDIO__
#define __SF3_AUDIO__
#include "sf3_core.h"

/// The format_id for an audio file.
#define SF3_FORMAT_ID_AUDIO 0x02

/// The possible audio sample formats.
enum sf3_audio_format{
  /// The audio is stored as bytes encoded in the "A-law" scheme.
  SF3_SAMPLE_ALAW = 0x01,
  /// The audio is stored as 16-bit signed PCM.
  SF3_SAMPLE_INT16 = 0x02,
  /// The audio is stored as 32-bit signed PCM.
  SF3_SAMPLE_INT32 = 0x04,
  /// The audio is stored as 64-bit signed PCM.
  SF3_SAMPLE_INT64 = 0x08,
  /// The audio is stored as bytes encoded in the "u-law" scheme.
  SF3_SAMPLE_ULAW = 0x11,
  /// The audio is stored as 16-bit unsigned PCM.
  SF3_SAMPLE_UINT16 = 0x12,
  /// The audio is stored as 32-bit unsigned PCM.
  SF3_SAMPLE_UINT32 = 0x14,
  /// The audio is stored as 64-bit unsigned PCM.
  SF3_SAMPLE_UINT64 = 0x18,
  /// The audio is stored as 16-bit IEEE half-precision floats.
  SF3_SAMPLE_FLOAT16 = 0x22,
  /// The audio is stored as 32-bit IEEE single-precision floats.
  SF3_SAMPLE_FLOAT32 = 0x24,
  /// The audio is stored as 64-bit IEEE double-precision floats.
  SF3_SAMPLE_FLOAT64 = 0x28
};

/// An audio file.
///
/// See SF3_FORMAT_ID_AUDIO
struct PACK sf3_audio{
  struct sf3_identifier identifier;
  /// The samplerate of the audio in Hertz.
  uint32_t samplerate;
  /// The number of audio channels per audio frame.
  uint8_t channels;
  /// The data format of the samples.
  /// See the SF3_AUDIO_FORMAT enumeration.
  uint8_t format;
  /// The total number of audio frames stored.
  uint64_t frame_count;
  /// The opaque sample data.
  /// See sf3_audio_sample_size
  /// See sf3_audio_frame_size
  char samples[];
};

/// Returns the number of bytes per audio sample.
static inline int sf3_audio_sample_size(const struct sf3_audio *audio){
  return audio->format & 0x0F;
}

/// Returns the number of bytes per audio frame.
static inline int sf3_audio_frame_size(const struct sf3_audio *audio){
  return (audio->format & 0x0F) * audio->channels;
}

/// Returns the duration of the audio data in seconds.
static inline float sf3_audio_duration(const struct sf3_audio *audio){
  return ((double)audio->frame_count) / audio->samplerate;
}

/// Returns a string descriptor of the format type.
static char *sf3_audio_format(enum sf3_audio_format format){
  switch(format){
  case SF3_SAMPLE_ALAW: return "A-law";
  case SF3_SAMPLE_INT16: return "int16";
  case SF3_SAMPLE_INT32: return "int32";
  case SF3_SAMPLE_INT64: return "int64";
  case SF3_SAMPLE_ULAW: return "u-law";
  case SF3_SAMPLE_UINT16: return "uint16";
  case SF3_SAMPLE_UINT32: return "uint32";
  case SF3_SAMPLE_UINT64: return "uint64";
  case SF3_SAMPLE_FLOAT16: return "float2";
  case SF3_SAMPLE_FLOAT32: return "float4";
  case SF3_SAMPLE_FLOAT64: return "float8";
  default: return "Unknown";
  }
}
#endif
