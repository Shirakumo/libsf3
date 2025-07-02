#define SF3_NO_CUSTOM_ALLOCATOR
#include "sf3_lib.h"
#include "sf3_lib.c"

int main(int argc, const char *argv[]){
  int all_ok = 1;
  for(int i=1; i<argc; ++i){
    sf3_handle handle;
    const char *path = argv[i];
    if(!sf3_open(path, SF3_OPEN_READ_ONLY, &handle)){
      fprintf(stderr, "%s:\n", path);
      fprintf(stderr, "  Failed to open!\n");
      continue;
    }
    
    int failed = 0;
    size_t size;
    void *addr = sf3_data(handle, &size);
    struct sf3_identifier *id = (struct sf3_identifier *)addr;
    if(size != sf3_size(id)){
      if(!failed)fprintf(stderr, "%s:\n", path);
      failed = 1;
      fprintf(stderr, "  Mismatched size!\n");
      fprintf(stderr, "  File: %lu Computed: %lu\n", size, sf3_size(id));
    }
    if(!sf3_verify(addr, size)){
      if(!failed)fprintf(stderr, "%s:\n", path);
      failed = 1;
      const void *payload = (const void *)(((const uint8_t *)addr)+sizeof(struct sf3_identifier));
      sf3_crc32_checksum check = sf3_compute_checksum(payload, size-sizeof(struct sf3_identifier));
      fprintf(stderr, "  Mismatched checksum!\n");
      fprintf(stderr, "  File: %4x Computed: %4x\n", id->checksum, check);
    }

    if(failed) all_ok = 0;
    sf3_close(handle);
  }
  return (all_ok)? 0 : 1;
}
