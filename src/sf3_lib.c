#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "sf3_lib.h"

struct sf3_handle{
  enum SF3_OPEN_MODE mode;
  int fd;
  size_t size;
  void *addr;
};


SF3_EXPORT int sf3_tell(char *path){
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

SF3_EXPORT int sf3_open(char *path, enum SF3_OPEN_MODE mode, sf3_handle *handle){
  struct sf3_handle *h = (struct sf3_handle *)sf3_calloc(1, sizeof(struct sf3_handle));
  if(!h) return -1;

  struct stat stat;
  h->fd = open(path, (mode)? O_RDWR : O_RDONLY);
  
  if(h->fd == -1){
    goto cleanup;
  }
  
  if(fstat(h->fd, &stat) == -1){
    goto cleanup;
  }

  h->mode = mode;
  h->size = stat.st_size;
  h->addr = mmap(NULL, h->size,
                 (mode)? (PROT_READ | PROT_WRITE) : PROT_READ,
                 (mode)? MAP_SHARED : MAP_PRIVATE,
                 h->fd, 0);
  if(!h->addr){
    goto cleanup;
  }

  *handle = h;
  return 1;
  
 cleanup:
  sf3_close(h);
  return 0;
}

SF3_EXPORT void sf3_close(sf3_handle handle){
  struct sf3_handle *h = (struct sf3_handle *)handle;
  if(h){
    if(h->addr){
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
  struct sf3_handle *h = (struct sf3_handle *)handle;
  if(h){
    *size = h->size;
    return h->addr;
  }
  return 0;
}

SF3_EXPORT int sf3_write(char *path, sf3_handle handle){
  struct sf3_handle *h = (struct sf3_handle *)handle;
  if(!path){
    if(h->mode == SF3_OPEN_READ_WRITE){
      return msync(h->addr, h->size, MS_SYNC) == 0;
    }else{
      return 0;
    }
  }else{
    
  }
}

#ifndef MIXED_NO_CUSTOM_ALLOCATOR
void *(*mixed_calloc)(size_t num, size_t size) = calloc;
void (*mixed_free)(void *ptr) = free;
#endif
