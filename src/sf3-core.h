#ifndef __SF3_CORE__
#define __SF3_CORE__
#include <stdint.h>
#include <stddef.h>

typedef uint32_t sf3_crc32_checksum;
typedef uint8_t sf3_format_id;

typedef struct __attribute__((packed)) sf3_str8{
  uint8_t length;
  char str[];
} sf3_str8;

typedef struct __attribute__((packed)) sf3_str16{
  uint16_t length;
  char str[];
} sf3_str16;

typedef struct __attribute__((packed)) sf3_str32{
  uint32_t length;
  char str[];
} sf3_str32;

typedef struct __attribute__((packed)) sf3_str64{
  uint64_t length;
  char str[];
} sf3_str64;

#define SF3_SKIP_STR(STR) ((STR).str+(STR).length)
#define SF3_SKIP_STRP(STR) ((STR)->str+(STR)->length)
#define SF3_MAGIC {0x81, 0x53, 0x46, 0x33, 0x00, 0xE0, 0xD0, 0x0D, 0x0A, 0x0A}

struct __attribute__((packed)) sf3_identifier{
  char magic[10];
  sf3_format_id format_id;
  sf3_crc32_checksum checksum;
  char null_terminator;
};

int sf3_check(const void *addr, size_t size){
  if(size < sizeof(struct sf3_identifier)) return 0;
  const struct sf3_identifier *identifier = (const struct sf3_identifier *)addr;
  const char magic[10] = SF3_MAGIC;
  for(int i=0; i<10; ++i){
    if(magic[i] != identifier->magic[i]) return 0;
  }
  if(identifier->null_terminator != 0) return 0;
  return identifier->format_id;
};
#endif
