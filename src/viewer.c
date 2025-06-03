#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "sf3.h"

int map_file(char *file, void **addr, size_t *size){
  struct stat stat;
  int fd = -1;
  
  printf("%s:\n", file);
  fd = open(file, O_RDONLY);
  
  if(fd == -1){
    printf("ERROR: Failed to open file.\n");
    goto cleanup;
  }
  
  if(fstat(fd, &stat) == -1){
    printf("ERROR: Failed to retrieve file size.\n");
    goto cleanup;
  }

  *size = stat.st_size;
  *addr = mmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
  if(!*addr){
    printf("ERROR: Failed to mmap file.\n");
    goto cleanup;
  }

  return 1;
  
 cleanup:
  if(*addr) munmap(*addr, *size);
  if(0 <= fd) close(fd);
  return 0;
}

int view_archive(struct sf3_archive *archive){
  printf("%lu files:\n",
         archive->count);
  for(uint64_t i=0; i<archive->count; ++i){
    const struct sf3_archive_meta *meta = sf3_archive_meta_entry(archive, i);
    printf("%4d %s\n     Modtime: %s     Mime: %s\n     CRC32: %08x\n     Size: %d\n",
           i,
           sf3_archive_meta_path(meta),
           ctime(&meta->modtime),
           sf3_archive_meta_mime_type(meta),
           meta->checksum,
           sf3_archive_file(archive, i)->length);
  }
  return 1;
}

int view_audio(struct sf3_audio *audio){
  printf("%d frames, %d Hz, %d channels, %s encoding\n",
         audio->frame_count,
         audio->samplerate,
         audio->channels,
         sf3_audio_format(audio->format));
  printf("%fs of audio data at %d bytes/frame\n",
         sf3_audio_duration(audio),
         sf3_audio_frame_size(audio));
  return 1;
}

int view_image(struct sf3_image *image){
  printf("%u x %u x %u, %d channels (%s), %s encoding\n",
         image->width,
         image->height,
         image->depth,
         sf3_image_channel_count(image),
         sf3_image_channel_layout(image->channels),
         sf3_image_pixel_format(image->format));
  printf("%lu pixels, %d bytes/pixel\n",
         image->width*image->height*image->depth,
         sf3_image_pixel_stride(image));
  return 1;
}

int view_log(struct sf3_log *log){
  return 1;
}

int view_model(struct sf3_model *model){
  return 1;
}

int view_physics_model(struct sf3_physics_model *physics_model){
  return 1;
}

int view_table(struct sf3_table *table){
  return 1;
}

int view_text(struct sf3_text *text){
  return 1;
}

int view_vector_graphic(struct sf3_vector_graphic *vector_graphic){
  return 1;
}

int view_file(void *addr, size_t size){
  int type = sf3_check(addr, size);
  if(!type){
    printf("ERROR: Not a valid SF3 file.\n");
    return 0;
  }
  printf("%s file (%s, %lu bytes)\n", sf3_kind(type), sf3_mime_type(type), size);
  
  switch(type){
  case SF3_FORMAT_ID_ARCHIVE:
    return view_archive((struct sf3_archive *)addr);
  case SF3_FORMAT_ID_AUDIO:
    return view_audio((struct sf3_audio *)addr);
  case SF3_FORMAT_ID_IMAGE:
    return view_image((struct sf3_image *)addr);
  case SF3_FORMAT_ID_LOG:
    return view_log((struct sf3_log *)addr);
  case SF3_FORMAT_ID_MODEL:
    return view_model((struct sf3_model *)addr);
  case SF3_FORMAT_ID_PHYSICS_MODEL:
    return view_physics_model((struct sf3_physics_model *)addr);
  case SF3_FORMAT_ID_TABLE:
    return view_table((struct sf3_table *)addr);
  case SF3_FORMAT_ID_TEXT:
    return view_text((struct sf3_text *)addr);
  case SF3_FORMAT_ID_VECTOR_GRAPHIC:
    return view_vector_graphic((struct sf3_vector_graphic *)addr);
  default:
    printf("ERROR: Invalid type ID: %d\n", type);
    return 0;
  }
}

int main(int argc, char *argv[]){
  if(argc<2){
    fprintf(stderr, "Usage: ./sf3-viewer FILE...\n");
    return 1;
  }
  for(int i=1; i<argc; ++i){
    void *addr;
    size_t size;
    if(map_file(argv[i], &addr, &size)){
      view_file(addr, size);
      munmap(addr, size);
    }
    printf("\n");
  }
  return 0;
}
