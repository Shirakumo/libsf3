#ifndef __SF3_LOG__
#define __SF3_LOG__
#include "sf3_core.h"

/// The format_id for a log file.
#define SF3_FORMAT_ID_LOG 0x04

/// A message entry in a log file.
/// See sf3_log_entry_source
/// See sf3_log_entry_category
/// See sf3_log_entry_message
struct PACK sf3_log_entry{
  /// The size of the log entry structure in bytes.
  uint32_t size;
  /// The time at which the log entry was created, in milliseconds
  /// since the log's start.
  uint64_t time;
  /// The severity of the message, with positive being more severe,
  /// negative more detailed.
  uint8_t severity;
  sf3_str8 source;
};

/// A chunk of log entries in a log file.
struct PACK sf3_log_chunk{
  /// The size of the chunk structure in bytes.
  uint64_t size;
  /// The number of registered log entries.
  uint32_t entry_count;
  /// The offsets to the log entries.
  uint64_t entry_offset[];
};

/// A log file.
///
/// See SF3_FORMAT_ID_LOG
struct PACK sf3_log{
  struct sf3_identifier identifier;
  int64_t start;
  int64_t end;
  uint16_t chunk_count;
  struct sf3_log_chunk chunks[];
};

/// Returns the string of the "source" for the entry.
static inline const char *sf3_log_entry_source(const struct sf3_log_entry *entry){
  return entry->source.str;
}

/// Returns the string of the "category" for the entry.
static inline const char *sf3_log_entry_category(const struct sf3_log_entry *entry){
  return ((sf3_str8 *)SF3_SKIP_STR(entry->source))->str;
}

/// Returns the string of the message for the entry.
static inline const char *sf3_log_entry_message(const struct sf3_log_entry *entry){
  return ((sf3_str16 *)SF3_SKIP_STRP((sf3_str8 *)SF3_SKIP_STR(entry->source)))->str;
}

/// Returns the next entry after this one.
/// Note that this function does not perform any bounds checking
/// whatsoever. It is up to you to ensure you do not call this with
/// the last entry of a chunk.
static inline const struct sf3_log_entry *sf3_log_next_entry(const struct sf3_log_entry *entry){
  return (const struct sf3_log_entry*)(((char*)entry)+entry->size);
}

/// Returns the first entry of the chunk.
static inline const struct sf3_log_entry *sf3_log_first_entry(const struct sf3_log_chunk *chunk){
  return (const struct sf3_log_entry*)chunk->entry_offset+chunk->entry_count;
}

/// Returns the log entry at the requested index.
/// If the entry index is out of bounds for the chunk, null is
/// returned instead.
static inline const struct sf3_log_entry *sf3_log_entry(const struct sf3_log_chunk *chunk, uint32_t entry){
  if(chunk->entry_count <= entry) return 0;
  return (const struct sf3_log_entry *)(((char*)chunk)+chunk->entry_offset[entry]);
}

/// Returns the next chunk after this one.
/// Note that this function does not perform any bounds checking
/// whatsoever. It is up to you to ensure you do not call this with
/// the last chunk of a log.
static inline const struct sf3_log_chunk *sf3_log_next_chunk(const struct sf3_log_chunk *chunk){
  return (const struct sf3_log_chunk *)(((char*)chunk)+chunk->size);
}

/// Returns the number of log entries that can be stored in the chunk.
static inline uint32_t sf3_log_chunk_capacity(const struct sf3_log_chunk *chunk){
  uint64_t end_of_offsets = chunk->entry_offset[0];
  uint64_t offsets_size = (end_of_offsets - 4 - 8);
  return offsets_size / sizeof(uint64_t);
}

/// Returns the number of log entries that can still be added to the
/// chunk before it runs out of capacity.
static inline uint32_t sf3_log_chunk_remaining(const struct sf3_log_chunk *chunk){
  return sf3_log_chunk_capacity(chunk) - chunk->entry_count;
}
#endif
