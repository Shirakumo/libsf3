#include "sf3_core.h"

#define SF3_FORMAT_ID_ARCHIVE 0x01

struct __attribute__((packed)) sf3_archive_meta{
  int64_t modtime;
  sf3_crc32_checksum checksum;
  sf3_str8 mime_type;
};

struct __attribute__((packed)) sf3_file{
  uint64_t length;
  char data[];
};

struct __attribute__((packed)) sf3_archive{
  struct sf3_identifier identifier;
  uint64_t count;
  uint64_t metadata_size;
  uint64_t entry_offset[];
};

const struct sf3_archive_meta *sf3_archive_meta_entry(const struct sf3_archive *archive, uint64_t index){
  if(archive->count <= index) return 0;
  const char *base = (char *)&archive->entry_offset[archive->count];
  base += archive->entry_offset[index];
  return (const struct sf3_archive_meta *)base;
}

const char *sf3_archive_meta_mime_type(const struct sf3_archive_meta *meta){
  return meta->mime_type.str;
}

const char *sf3_archive_meta_path(const struct sf3_archive_meta *meta){
  return ((sf3_str16 *)(meta->mime_type.str + meta->mime_type.length))->str;
}

const struct sf3_file *sf3_archive_file(const struct sf3_archive *archive, uint64_t index){
  if(archive->count <= index) return 0;
  const char *base = (char *)&archive->entry_offset[0];
  base += archive->metadata_size;
  const uint64_t *offsets = (const uint64_t *)base;
  base = (char*)(offsets+archive->count);
  return (const struct sf3_file *)(base + offsets[index]);
}
