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
  time_t start = log->start;
  printf("Start: %s", ctime(&start));
  if(log->end == INT64_MAX) printf("End:   (none)\n");
  else printf("End:   %s", ctime(&log->end));
  printf("%d chunks:\n", log->chunk_count);
  const struct sf3_log_chunk *chunk = &log->chunks[0];
  for(uint16_t i=0; i<log->chunk_count; ++i){
    printf("Chunk %d (%lu bytes, %d entries)\n",
           i, chunk->size, chunk->entry_count);
    for(uint32_t e=0; e<chunk->entry_count; ++e){
      char tstamp[128] = {0};
      const struct sf3_log_entry *entry = sf3_log_entry(chunk, e);
      time_t time = start + entry->time/1000;
      uint32_t millis = entry->time % 1000;
      strftime(tstamp, 32, "%F %T", gmtime(&time));
      printf(" %s.%03d [%+3d] %s <%s> %s\n",
             tstamp,
             millis,
             entry->severity,
             sf3_log_entry_source(entry),
             sf3_log_entry_category(entry),
             sf3_log_entry_message(entry));
    }
    chunk = sf3_log_next_chunk(chunk);
  }
  return 1;
}

int view_model(struct sf3_model *model){
  printf("%d textures, %u faces, %u vertices\n",
         sf3_model_texture_count(model),
         sf3_model_faces(model)->count,
         sf3_model_vertex_count(model));
  printf("Vertex attributes:");
  if(model->vertex_format & SF3_VERTEX_POSITION) printf(" position");
  if(model->vertex_format & SF3_VERTEX_UV) printf(" uv");
  if(model->vertex_format & SF3_VERTEX_COLOR) printf(" color");
  if(model->vertex_format & SF3_VERTEX_NORMAL) printf(" normal");
  if(model->vertex_format & SF3_VERTEX_TANGENT) printf(" tangent");
  printf("\nTextures:\n");
  sf3_str16 *texture = &model->textures[0];
  for(uint8_t i=0; i<sf3_model_texture_count(model); ++i){
    printf("%16s %s\n",
           sf3_model_material_type(sf3_model_texture_material(model, i)),
           texture->str);
    texture = sf3_model_next_texture(texture);
  }
  printf("Vertices:\n");
  const float *vertices = sf3_model_vertices(model)->vertices;
  for(uint32_t i=0; i<sf3_model_vertex_count(model); ++i){
    for(uint32_t a=1; a<255; a = a << 1){
      if(a & model->vertex_format){
        for(uint8_t f=0; f<sf3_model_vertex_attribute_count(a); ++f){
          printf("%f ", vertices[f]);
          ++vertices;
        }
        printf(" ");
      }
    }
    printf("\n");
  }
  return 1;
}

int view_physics_model(struct sf3_physics_model *physics_model){
  return 1;
}

int view_table(struct sf3_table *table){
  return 1;
}

int view_text(struct sf3_text *text){
  printf("Markup:\n");
  const struct sf3_markup *markup = &text->markup[0];
  for(uint32_t i=0; i<text->markup_count; ++i){
    printf("%6lu %6lu %s",
           markup->start,
           markup->end,
           sf3_text_markup_option_type(markup->option_type));
    switch(markup->option_type){
    case SF3_MARKUP_COLOR:
      printf(" %f %f %f",
             ((struct sf3_markup_color*)markup)->r,
             ((struct sf3_markup_color*)markup)->g,
             ((struct sf3_markup_color*)markup)->b);
      break;
    case SF3_MARKUP_SIZE:
      printf(" %f", ((struct sf3_markup_size*)markup)->size);
      break;
    case SF3_MARKUP_HEADING:
      printf(" %d", ((struct sf3_markup_heading*)markup)->level);
      break;
    case SF3_MARKUP_LINK:
      printf(" %s", ((struct sf3_markup_link*)markup)->address.str);
      break;
    case SF3_MARKUP_TARGET:
      printf(" %s", ((struct sf3_markup_target*)markup)->address.str);
      break;
    case SF3_MARKUP_FONT:
      printf(" %s", ((struct sf3_markup_font*)markup)->font.str);
      break;
    }
    printf("\n");
    markup = sf3_text_next_markup(markup);
  }
  printf("Text:\n%s\n", sf3_text_string(text));
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
