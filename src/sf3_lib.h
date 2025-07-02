#ifndef __SF3_LIB_H__
#define __SF3_LIB_H__
#ifdef __cplusplus
extern "C" {
#endif

#if defined(SF3_STATIC)
#  define SF3_EXPORT static
#  define SF3_INLINE static inline
#elif defined(_MSC_VER)
#  if defined(SF3_BUILD)
#    define SF3_EXPORT __declspec(dllexport)
#    define SF3_INLINE __declspec(dllexport)
#  else
#    define SF3_EXPORT __declspec(dllimport)
#    define SF3_INLINE __declspec(dllimport)
#  endif
#elif defined(__GNUC__)
#  if defined(SF3_BUILD)
#    define SF3_EXPORT __attribute__((visibility("default")))
#    define SF3_INLINE __attribute__((visibility("default")))
#  else
#    define SF3_EXPORT
#    define SF3_INLINE static inline
#  endif
#else
#  define SF3_EXPORT
#endif
#include "sf3.h"

  enum sf3_open_mode{
    /// Open the file for reading only.
    SF3_OPEN_READ_ONLY = 0,
    /// Open the file for reading and writing.
    /// Note however that if you write into the memory of the file, it
    /// is not guaranteed that other applications will be able to see
    /// the changes until after sf3_write has returned successfully.
    SF3_OPEN_READ_WRITE = 1,
  };

  enum sf3_error{
    /// No error has occurred
    SF3_OK = 0,
    /// The call to open failed. The file may not exist or you may
    /// lack the necessary permissions.
    SF3_OPEN_FAILED,
    /// A memory allocation failed.
    SF3_OUT_OF_MEMORY,
    /// The file failed to map into memory.
    SF3_MMAP_FAILED,
    /// An invalid sf3_handle was passed.
    SF3_INVALID_HANDLE,
    /// The file write operation failed.
    SF3_WRITE_FAILED,
    /// The file is not a valid SF3 file.
    SF3_INVALID_FILE,
  };

  /// Opaque representation of a file handle.
  typedef void *sf3_handle;

  /// Return the error code.
  /// This is thread local.
  SF3_EXPORT enum sf3_error sf3_error();

  /// Return a string representation of the error code.
  SF3_EXPORT const char *sf3_strerror(enum sf3_error error);

  /// Returns the sf3_format_id of the file or 0 on failure.
  ///
  /// The failure can occur because the path is inaccessible, there is
  /// no more memory available, or the file is not an actual SF3 file.
  /// Note that this will not perform a CRC32 checksum verification of
  /// the file.
  SF3_EXPORT int sf3_tell(const char *path);

  /// Opens the SF3 file at the given path.
  /// 
  /// If successful returns the format ID and stores the handle in
  /// the handle argument. You can then use sf3_data to retrieve the
  /// actual sf3 file address and size. This will also already call
  /// sf3_check for you, though not sf3_verify.
  ///
  /// If the file open fails, the application is out of memory, or the
  /// file is not a valid SF3 file, zero is returned instead.
  SF3_EXPORT int sf3_open(const char *path, enum sf3_open_mode mode, sf3_handle *handle);

  /// Closes the file handle.
  ///
  /// After closing the handle is discarded and calling any function
  /// with it leads to undefined behaviour.
  SF3_EXPORT void sf3_close(sf3_handle handle);

  /// Returns the address of the SF3 file payload and its size.
  ///
  /// The size parameter may be a null pointer, in which case the size
  /// is not returned.
  ///
  /// This may fail if the handle is invalid, in which case a null
  /// pointer is returned.
  SF3_EXPORT void *sf3_data(sf3_handle handle, size_t *size);

  /// Create a handle from a given SF3 file payload in memory.
  ///
  /// This may be useful if you want to call sf3_write to serialize
  /// the SF3 file back out to a file. Note that the SIZE here should
  /// be the size of the memory block at ADDR, and does not
  /// necessarily have to correspond to the size of the file contents
  /// (meaning you can over-allocate). The size of the file contents
  /// are computed automatically when sf3_write is called.
  SF3_EXPORT int sf3_create(void *addr, size_t size, sf3_handle *handle);

  /// Write the SF3 file back out to a file.
  ///
  /// PATH may be a null pointer if the handle was obtained through
  /// sf3_open with mode set to SF3_OPEN_READ_WRITE. In that case this
  /// will write the file header back into memory with a freshly
  /// computed CRC32 checksum, and then call msync to flush the file.
  ///
  /// If the PATH is not a null pointer, the SF3 file is written out
  /// to the file at that path. As part of this the SF3 header is
  /// written fresh, including computing a fresh CRC32 checksum. The
  /// memory of the file is **not** modified, however.
  /// If the file at PATH already exists, it is truncated to the size
  /// of the SF3 file after writing.
  SF3_EXPORT int sf3_write(const char *path, sf3_handle handle);

#ifdef SF3_NO_CUSTOM_ALLOCATOR
#define sf3_calloc calloc
#define sf3_free free
#else
  /// Allocates a new block of memory.
  /// 
  /// sf3_calloc must return either 0 or a pointer to a memory region
  /// that is num*size octets large. It may only return 0 if the allocation
  /// failed for some reason, such as an out of memory condition.
  /// The returned memory region must be cleared, meaning all addressable
  /// octets within the region must be 0. The memory region also must not
  /// overlap with any other allocated memory regions.
  SF3_EXPORT extern void *(*sf3_calloc)(size_t num, size_t size);

  /// Releases a previously allocated region of memory.
  ///
  /// The behaviour is undefined if a pointer is passed to this function
  /// that was not returned by a prior call to sf3_calloc, or if the pointer
  /// was already passed to sf3_free before being returned by another call
  /// to sf3_calloc.
  SF3_EXPORT extern void (*sf3_free)(void *ptr);
#endif

#ifdef __cplusplus
}
#endif
#endif
