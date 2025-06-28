#ifndef __SF3_CORE__
#define __SF3_CORE__
#include <stdint.h>
#include <stddef.h>

#if defined(__DOXYGEN__)
#  define SF3_PACK
#else
#  define SF3_PACK __attribute__((packed))
#endif

/// Type for a CRC32 checksum.
typedef uint32_t sf3_crc32_checksum;

/// Type for an SF3 file format ID.
/// Note that not all values are valid.
typedef uint8_t sf3_format_id;

/// SF3 string with a maximum length of 2^8-1.
/// The string is null-terminated, and the entire length of the char
/// array including the null terminator is in the length field.
typedef struct SF3_PACK sf3_str8{
  uint8_t length;
  char str[];
} sf3_str8;

/// SF3 string with a maximum length of 2^16-1.
/// The string is null-terminated, and the entire length of the char
/// array including the null terminator is in the length field.
typedef struct SF3_PACK sf3_str16{
  uint16_t length;
  char str[];
} sf3_str16;

/// SF3 string with a maximum length of 2^32-1.
/// The string is null-terminated, and the entire length of the char
/// array including the null terminator is in the length field.
typedef struct SF3_PACK sf3_str32{
  uint32_t length;
  char str[];
} sf3_str32;

/// SF3 string with a maximum length of 2^64-1.
/// The string is null-terminated, and the entire length of the char
/// array including the null terminator is in the length field.
typedef struct SF3_PACK sf3_str64{
  uint64_t length;
  char str[];
} sf3_str64;

/// Macro to retrieve a pointer past the variable size of an SF3 string.
#define SF3_SKIP_STR(STR) ((STR).str+(STR).length)

/// Macro to retrieve a pointer past the variable size of an SF3 string.
/// This is a variant for the case where the value is a pointer to the
/// string.
#define SF3_SKIP_STRP(STR) ((STR)->str+(STR)->length)

/// The SF3 file header magic identifier bytes.
#define SF3_MAGIC {0x81, 0x53, 0x46, 0x33, 0x00, 0xE0, 0xD0, 0x0D, 0x0A, 0x0A}

/// The basic header structure of every SF3 file.
struct SF3_PACK sf3_identifier{
  /// These magic bits must be the same as SF3_MAGIC.
  char magic[10];
  /// The ID of the actual file type contained within.
  sf3_format_id format_id;
  /// A CRC32 checksum of the file after this identifier struct.
  /// May be used to check the file for corruption.
  sf3_crc32_checksum checksum;
  /// A final null terminator marking the end of the identifier.
  char null_terminator;
};

/// Checks whether a chunk of memory is a valid SF3 file.
/// If valid, returns the format id of the file.
/// If invalid, returns zero.
///
/// Note that this will not check whether the entire file contents are
/// valid, they may still be filled with completely bogus values. As
/// such, this function makes no security guarantees whatsoever about
/// the block of memory it is handed.
static int sf3_check(const void *addr, size_t size){
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
