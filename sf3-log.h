#include "sf3-core.h"

#define SF3_FORMAT_ID_LOG 0x04

struct sf3_log_entry{
  uint32_t size;
  uint64_t time;
  uint8_t severity;
  sf3_str8 source;
};

struct sf3_log_chunk{
  uint64_t size;
  uint32_t entry_count;
  uint64_t entry_offset[];
};

struct sf3_log{
  struct sf3_identifier identifier;
  int64_t start;
  int64_t end;
  uint16_t chunk_count;
  struct sf3_log_chunk chunks[];
};

const char *sf3_log_entry_source(const struct sf3_log_entry *entry){
  return entry->source.str;
}

const char *sf3_log_entry_category(const struct sf3_log_entry *entry){
  return ((sf3_str8 *)SF3_SKIP_STR(entry->source))->str;
}

const char *sf3_log_entry_message(const struct sf3_log_entry *entry){
  return ((sf3_str16 *)SF3_SKIP_STRP((sf3_str8 *)SF3_SKIP_STR(entry->source)))->str;
}

const struct sf3_log_entry *sf3_log_next_entry(const struct sf3_log_entry *entry){
  return (const struct sf3_log_entry*)SF3_SKIP_STRP((sf3_str16 *)SF3_SKIP_STRP((sf3_str8 *)SF3_SKIP_STR(entry->source)));
}

const struct sf3_log_entry *sf3_log_first_entry(const struct sf3_log_chunk *chunk){
  return (const struct sf3_log_entry*)chunk->entry_offset+chunk->entry_count;
}

const struct sf3_log_entry *sf3_log_entry(const struct sf3_log_chunk *chunk, uint32_t entry){
  if(chunk->entry_count <= entry) return 0;
  return (const struct sf3_log_entry *)(((char*)chunk)+chunk->entry_offset[entry]);
}

const struct sf3_log_chunk *sf3_log_next_chunk(const struct sf3_log_chunk *chunk){
  return (const struct sf3_log_chunk *)(((char*)chunk)+chunk->size);
}

uint32_t sf3_log_chunk_capacity(const struct sf3_log_chunk *chunk){
  uint64_t end_of_offsets = chunk->entry_offset[0];
  uint64_t offsets_size = (end_of_offsets - 4 - 8);
  return offsets_size / sizeof(uint64_t);
}

uint32_t sf3_log_chunk_remaining(const struct sf3_log_chunk *chunk){
  return sf3_log_chunk_capacity(chunk) - chunk->entry_count;
}
