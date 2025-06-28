#ifndef __SF3_ARCHIVE__
#define __SF3_ARCHIVE__
#include "sf3_core.h"

/// The format_id for an archive file
#define SF3_FORMAT_ID_ARCHIVE 0x01

/// A single archive file metadata block.
///
/// See sf3_archive_meta_mime_type
/// See sf3_archive_meta_path
struct SF3_PACK sf3_archive_meta{
  /// The last time the associated file was modified, as a UNIX timestamp
  int64_t modtime;
  /// The CRC32 checksum of the file payload
  sf3_crc32_checksum checksum;
  /// The media mime-type of the file contents
  sf3_str8 mime_type;
};

/// A single file from an archive.
struct SF3_PACK sf3_file{
  /// The length of the data payload in bytes.
  uint64_t length;
  /// The data payload of the file.
  /// You **must not** read more than `length` bytes from this array.
  char data[];
};

/// An archive file.
///
/// See SF3_FORMAT_ID_ARCHIVE
struct SF3_PACK sf3_archive{
  struct sf3_identifier identifier;
  /// The number of files stored in the archive.
  /// This menas that there are this number of metadata intries and
  /// this number of file payloads.
  uint64_t count;
  /// The size of the metadata block in bytes.
  uint64_t metadata_size;
  /// The array of offsets for each metadata entry.
  uint64_t entry_offset[];
};

/// Retrieves the metadata entry of the given index.
///
/// If the index is larger than the number of files in the archive,
/// null is returned.
static inline const struct sf3_archive_meta *sf3_archive_meta_entry(const struct sf3_archive *archive, uint64_t index){
  if(archive->count <= index) return 0;
  const char *base = (char *)&archive->entry_offset[archive->count];
  base += archive->entry_offset[index];
  return (const struct sf3_archive_meta *)base;
}

/// Returns the C string for the mime-type
static inline const char *sf3_archive_meta_mime_type(const struct sf3_archive_meta *meta){
  return meta->mime_type.str;
}

/// Returns the C string for the relative file path
static inline const char *sf3_archive_meta_path(const struct sf3_archive_meta *meta){
  return ((sf3_str16 *)(meta->mime_type.str + meta->mime_type.length))->str;
}

/// Retrieves the file entry of the given index.
///
/// If the index is larger than the number of files in the archive,
/// null is returned.
static inline const struct sf3_file *sf3_archive_file(const struct sf3_archive *archive, uint64_t index){
  if(archive->count <= index) return 0;
  const char *base = (char *)&archive->entry_offset[0];
  base += archive->metadata_size;
  const uint64_t *offsets = (const uint64_t *)base;
  base = (char*)(offsets+archive->count);
  return (const struct sf3_file *)(base + offsets[index]);
}
#endif
