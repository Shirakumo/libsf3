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
#include<stdio.h>

enum SF3_OPEN_MODE{
  SF3_OPEN_READ_ONLY = 0,
  SF3_OPEN_READ_WRITE = 1,
};

typedef void *sf3_handle;

/// Returns the sf3_format_id of the file or 0 on failure.
///
/// The failure can occur because the path is inaccessible, there is
/// no more memory available, or the file is not an actual SF3 file.
/// Note that this will not perform a CRC32 checksum verification of
/// the file.
SF3_EXPORT int sf3_tell(char *path);

/// 
SF3_EXPORT int sf3_open(char *path, enum SF3_OPEN_MODE mode, sf3_handle *handle);

/// 
SF3_EXPORT void sf3_close(sf3_handle handle);

/// 
SF3_EXPORT void *sf3_data(sf3_handle handle, size_t *size);

/// 
SF3_EXPORT int sf3_write(char *path, sf3_handle handle);

#ifdef SF3_NO_CUSTOM_ALLOCATOR
#define sf3_calloc calloc
#define sf3_free free
#define sf3_realloc realloc
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
