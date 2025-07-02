#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "sf3_lib.h"

#ifndef thread_local
# if defined NN_NINTENDO_SDK
#  define thread_local
# elif __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#  define thread_local _Thread_local
# elif defined _WIN32 && ( defined _MSC_VER || defined __ICL || defined __DMC__ || defined __BORLANDC__ )
#  define thread_local __declspec(thread)
/* note that ICC (linux) and Clang are covered by __GNUC__ */
# elif defined __GNUC__ || defined __SUNPRO_C || defined __xlC__
#  define thread_local __thread
# else
#  define thread_local
# endif
#endif

struct handle{
  enum sf3_open_mode mode;
  int fd;
  size_t size;
  void *addr;
};

thread_local enum sf3_error err = SF3_OK;

SF3_EXPORT enum sf3_error sf3_error(){
  return err;
}

SF3_EXPORT const char *sf3_strerror(enum sf3_error error){
  switch(error){
  case SF3_OK:
    return "No error occurred.";
  case SF3_OPEN_FAILED:
    return "Failed to open file.";
  case SF3_OUT_OF_MEMORY:
    return "Out of memory.";
  case SF3_MMAP_FAILED:
    return "Failed to map file into memory.";
  case SF3_INVALID_HANDLE:
    return "Invalid sf3 file handle.";
  case SF3_WRITE_FAILED:
    return "Failed to write file.";
  default:
    return "Unknown error";
  }
}

SF3_EXPORT int sf3_tell(const char *path){
  void *handle;
  if(!sf3_open(path, SF3_OPEN_READ_ONLY, &handle)){
    return 0;
  }
  size_t size;
  void *addr = sf3_data(handle, &size);
  int res = sf3_check(addr, size);
  sf3_close(handle);
  return res;
}

SF3_EXPORT int sf3_open(const char *path, enum sf3_open_mode mode, sf3_handle *handle){
  struct handle *h = (struct handle *)sf3_calloc(1, sizeof(struct handle));
  if(!h){
    err = SF3_OUT_OF_MEMORY;
    return 0;
  }

  struct stat stat;
  h->fd = open(path, (mode)? O_RDWR : O_RDONLY);
  
  if(h->fd == -1){
    err = SF3_OPEN_FAILED;
    goto cleanup;
  }
  
  if(fstat(h->fd, &stat) == -1){
    err = SF3_OPEN_FAILED;
    goto cleanup;
  }

  h->mode = mode;
  h->size = stat.st_size;
  h->addr = mmap(NULL, h->size,
                 (mode)? (PROT_READ | PROT_WRITE) : PROT_READ,
                 (mode)? MAP_SHARED : MAP_PRIVATE,
                 h->fd, 0);
  if(h->addr == MAP_FAILED){
    err = SF3_MMAP_FAILED;
    goto cleanup;
  }

  *handle = h;
  return 1;
  
 cleanup:
  sf3_close(h);
  return 0;
}

SF3_EXPORT void sf3_close(sf3_handle handle){
  struct handle *h = (struct handle *)handle;
  if(h){
    if(h->addr != MAP_FAILED){
      munmap(h->addr, h->size);
      h->addr = 0;
    }
    if(0 <= h->fd){
      close(h->fd);
      h->fd = -1;
    }
    h->mode = 0;
    h->size = 0;
    sf3_free(h);
  }
}

SF3_EXPORT void *sf3_data(sf3_handle handle, size_t *size){
  struct handle *h = (struct handle *)handle;
  if(h){
    if(size) *size = h->size;
    return h->addr;
  }
  err = SF3_INVALID_HANDLE;
  return 0;
}

SF3_EXPORT int sf3_create(void *addr, size_t size, sf3_handle *handle){
  struct handle *h = (struct handle *)sf3_calloc(1, sizeof(struct handle));
  if(!h){
    err = SF3_OUT_OF_MEMORY;
    return -1;
  }
  h->mode = SF3_OPEN_READ_WRITE;
  h->fd = -1;
  h->size = size;
  h->addr = addr;
  *handle = h;
  return 1;
}

SF3_EXPORT int sf3_write(const char *path, sf3_handle handle){
  struct handle *h = (struct handle *)handle;
  struct sf3_identifier *id = (struct sf3_identifier *)h->addr;
  if(h->size < sizeof(struct sf3_identifier)){
    err = SF3_INVALID_HANDLE;
    return 0;
  }else if(!path){
    if(h->mode == SF3_OPEN_READ_WRITE && h->fd != -1){
      size_t size = sf3_size(id);
      sf3_write_header(id->format_id, h->addr, size);
      return msync(h->addr, h->size, MS_SYNC) == 0;
    }else{
      return 0;
    }
  }else{
    struct sf3_identifier header;
    const void *payload = ((const uint8_t*)h->addr+sizeof(struct sf3_identifier));
    size_t size = sf3_size(id);
    
    int fd = open(path, O_WRONLY | O_CREAT);
    if(fd == -1){
      err = SF3_OPEN_FAILED;
      return 0;
    }
    sf3_write_header(id->format_id, &header, size);
    write(fd, &header, sizeof(struct sf3_identifier));
    size -= sizeof(struct sf3_identifier);
    if(write(fd, payload, size) < size){
      err = SF3_WRITE_FAILED;
      close(fd);
      return 0;
    }
    ftruncate(fd, size);
    close(fd);
    return 1;
  }
}

#ifndef SF3_NO_CUSTOM_ALLOCATOR
void *(*mixed_calloc)(size_t num, size_t size) = calloc;
void (*mixed_free)(void *ptr) = free;
#endif
