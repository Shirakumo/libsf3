#include "sf3_lib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

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
         sf3_model_faces(model)->count / 3,
         sf3_model_vertex_count(model));
  printf("Vertex attributes:");
  if(model->vertex_format & SF3_VERTEX_POSITION) printf(" position");
  if(model->vertex_format & SF3_VERTEX_UV) printf(" uv");
  if(model->vertex_format & SF3_VERTEX_COLOR) printf(" color");
  if(model->vertex_format & SF3_VERTEX_NORMAL) printf(" normal");
  if(model->vertex_format & SF3_VERTEX_TANGENT) printf(" tangent");
  printf("\nTextures:\n");
  const sf3_str16 *texture = &model->textures[0];
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
          printf("%f ", vertices[0]);
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
  printf("Mass: %f\n",
         physics_model->mass);
  printf("Tensor:");
  for(uint8_t i=0; i<9; ++i){
    if(i%3 == 0) printf("\n");
    printf(" %f", physics_model->tensor[i]);
  }
  printf("\n%d shapes:\n", physics_model->shape_count);
  const struct sf3_physics_shape *shape = &physics_model->shapes[0];
  for(uint16_t i=0; i<physics_model->shape_count; ++i){
    printf(" %4d %s\n", i, sf3_physics_shape_type(shape->type));
    switch(shape->type){
    case SF3_PHYSICS_SHAPE_ELLIPSOID:
      printf("      w: %.3f h: %.3f d: %.3f\n",
             ((struct sf3_shape_ellipsoid *)shape)->w,
             ((struct sf3_shape_ellipsoid *)shape)->h,
             ((struct sf3_shape_ellipsoid *)shape)->d);
      break;
    case SF3_PHYSICS_SHAPE_BOX:
      printf("      w: %.3f h: %.3f d: %f\n",
             ((struct sf3_shape_box *)shape)->w,
             ((struct sf3_shape_box *)shape)->h,
             ((struct sf3_shape_box *)shape)->d);
      break;
    case SF3_PHYSICS_SHAPE_CYLINDER:
      printf("      br: %.3f tr: %.3f h: %f\n",
             ((struct sf3_shape_cylinder *)shape)->bottom_radius,
             ((struct sf3_shape_cylinder *)shape)->top_radius,
             ((struct sf3_shape_cylinder *)shape)->height);
      break;
    case SF3_PHYSICS_SHAPE_PILL:
      printf("      br: %.3f tr: %.3f h: %f\n",
             ((struct sf3_shape_pill *)shape)->bottom_radius,
             ((struct sf3_shape_pill *)shape)->top_radius,
             ((struct sf3_shape_pill *)shape)->height);
      break;
    case SF3_PHYSICS_SHAPE_MESH: {
      struct sf3_shape_mesh *mesh = (struct sf3_shape_mesh *)shape;
      for(uint16_t v=0; v<mesh->count; ++v){
        printf("      %.3f %.3f %.3f\n",
               mesh->vertices[v*3+0],
               mesh->vertices[v*3+1],
               mesh->vertices[v*3+2]);
      }
      break;
    }
    }
    shape = sf3_physics_model_next_shape(shape);
  }
  return 1;
}

int view_table(struct sf3_table *table){
  printf("%d columns, %lu rows\n",
         table->column_count,
         table->row_count);
  printf("Columns:\n");
  const struct sf3_column_spec *spec = &table->columns[0];
  for(uint16_t i=0; i<table->column_count; ++i){
    printf(" %4d %s\n     Type: %s\n     Octets: %d\n     Elements: %d\n",
           i,
           spec->name.str,
           sf3_table_column_type(spec->type),
           spec->length,
           sf3_table_element_count(spec));
    spec = sf3_table_next_column(spec);
  }
  
  uint16_t length = 1 + table->column_count * 19;
  printf("Data:\n┌");
  for(uint16_t i=0; i<table->column_count; ++i){
    if(0<i) printf("┬");
    for(uint16_t j=0; j<18; ++j) printf("─");
  }
  printf("┐\n");
  spec = &table->columns[0];
  for(uint16_t i=0; i<table->column_count; ++i){
    printf("│ %16.16s ", spec->name.str);
    spec = sf3_table_next_column(spec);
  }
  printf("│\n├");
  for(uint16_t i=0; i<table->column_count; ++i){
    if(0<i) printf("┼");
    for(uint16_t j=0; j<18; ++j) printf("─");
  }
  printf("┤\n");
  for(uint64_t r=0; r<table->row_count; ++r){
    printf("│ ");
    for(uint64_t c=0; c<table->column_count; ++c){
      if(0<c) printf(" │ ");
      const char *data = sf3_table_cell(table, r, c, &spec);
      for(uint16_t e=0; e<sf3_table_element_count(spec); ++e){
        switch(spec->type){
        case SF3_COLUMN_UINT8:
          printf("%16u", *((uint8_t*)data));
          break;
        case SF3_COLUMN_UINT16:
          printf("%16u", *((uint16_t*)data));
          break;
        case SF3_COLUMN_UINT32:
          printf("%16u", *((uint32_t*)data));
          break;
        case SF3_COLUMN_UINT64:
          printf("%16lu", *((uint64_t*)data));
          break;
        case SF3_COLUMN_INT8:
          printf("%16hhd", *((int8_t*)data));
          break;
        case SF3_COLUMN_INT16:
          printf("%16hd", *((int16_t*)data));
          break;
        case SF3_COLUMN_INT32:
          printf("%16d", *((int32_t*)data));
          break;
        case SF3_COLUMN_INT64:
          printf("%16ld", *((int64_t*)data));
          break;
        case SF3_COLUMN_FLOAT16:
          printf("%16x", *((uint16_t*)data));
          break;
        case SF3_COLUMN_FLOAT32:
          printf("%16.4f", *((float*)data));
          break;
        case SF3_COLUMN_FLOAT64:
          printf("%16.4f", *((double*)data));
          break;
        case SF3_COLUMN_STRING:
          printf("%16.16s", ((char*)data));
          break;
        case SF3_COLUMN_TIMESTAMP:
          printf("%16.16s", ctime((time_t*)data));
          break;
        case SF3_COLUMN_HIGH_RESOLUTION_TIMESTAMP:{
          uint64_t nanos = *((uint64_t*)data);
          uint64_t seconds = nanos / 1000000000;
          printf("%16.16s", ctime(&seconds));
          break;
        }
        case SF3_COLUMN_BOOLEAN:
          printf("%16.16s", ((char*)data) ? "True" : "False");
          break;
        }
        data += sf3_table_element_size(spec);
      }
    }
    printf(" │\n");
  }
  printf("└");
  for(uint16_t i=0; i<table->column_count; ++i){
    if(0<i) printf("┴");
    for(uint16_t j=0; j<18; ++j) printf("─");
  }
  printf("┘\n");
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
  printf("%d x %d, %d instructions\n",
         vector_graphic->width,
         vector_graphic->height,
         vector_graphic->count);
  printf("Instructions:\n");
  const struct sf3_vector_instruction *instruction = &vector_graphic->instructions[0];
  for(uint32_t i=0; i<vector_graphic->count; ++i){
    printf(" %4d %s\n",
           i,
           sf3_vector_instruction_type(instruction->type));
    switch(instruction->type){
    case SF3_INSTRUCTION_RECTANGLE: {
      const struct sf3_rectangle_instruction *inst = (struct sf3_rectangle_instruction*)instruction;
      printf("      Point: %6.2f %6.2f\n      Size:  %6.2f %6.2f\n",
             inst->bounds.x,
             inst->bounds.y,
             inst->bounds.w,
             inst->bounds.h);
      printf("      Fill:  %.3f %.3f %.3f %.3f\n      Line:  %.3f %.3f %.3f %.3f\n      Thickness: %.2f\n",
             inst->fill.fill_color.r,
             inst->fill.fill_color.g,
             inst->fill.fill_color.b,
             inst->fill.fill_color.a,
             inst->fill.outline_color.r,
             inst->fill.outline_color.g,
             inst->fill.outline_color.b,
             inst->fill.outline_color.a,
             inst->fill.outline_thickness);
      break;}
    case SF3_INSTRUCTION_CIRCLE: {
      const struct sf3_circle_instruction *inst = (struct sf3_circle_instruction*)instruction;
      printf("      Point: %6.2f %6.2f\n      Size:  %6.2f %6.2f\n",
             inst->bounds.x,
             inst->bounds.y,
             inst->bounds.w,
             inst->bounds.h);
      printf("      Fill:  %.3f %.3f %.3f %.3f\n      Line:  %.3f %.3f %.3f %.3f\n      Thickness: %.2f\n",
             inst->fill.fill_color.r,
             inst->fill.fill_color.g,
             inst->fill.fill_color.b,
             inst->fill.fill_color.a,
             inst->fill.outline_color.r,
             inst->fill.outline_color.g,
             inst->fill.outline_color.b,
             inst->fill.outline_color.a,
             inst->fill.outline_thickness);
      break;}
    case SF3_INSTRUCTION_LINE: {
      const struct sf3_line_instruction *inst = (struct sf3_line_instruction*)instruction;
      printf("      Color: %.3f %.3f %.3f %.3f\n      Thickness: %.2f\n",
             inst->color.r,
             inst->color.g,
             inst->color.b,
             inst->color.a,
             inst->thickness);
      printf("      Edges:\n");
      for(uint16_t i=0; i<inst->outline.count; ++i){
        const struct sf3_point *p = &inst->outline.edges[i];
        printf("        %6.2f %6.2f\n", p->x, p->y);
      }
      break;}
    case SF3_INSTRUCTION_POLYGON: {
      const struct sf3_polygon_instruction *inst = (struct sf3_polygon_instruction*)instruction;
      printf("      Fill:  %.3f %.3f %.3f %.3f\n      Line:  %.3f %.3f %.3f %.3f\n      Thickness: %.2f\n",
             inst->fill.fill_color.r,
             inst->fill.fill_color.g,
             inst->fill.fill_color.b,
             inst->fill.fill_color.a,
             inst->fill.outline_color.r,
             inst->fill.outline_color.g,
             inst->fill.outline_color.b,
             inst->fill.outline_color.a,
             inst->fill.outline_thickness);
      printf("      Edges:\n");
      for(uint16_t i=0; i<inst->outline.count; ++i){
        const struct sf3_point *p = &inst->outline.edges[i];
        printf("        %6.2f %6.2f\n", p->x, p->y);
      }
      break;}
    case SF3_INSTRUCTION_CURVE: {
      const struct sf3_curve_instruction *inst = (struct sf3_curve_instruction*)instruction;
      printf("      Fill:  %.3f %.3f %.3f %.3f\n      Line:  %.3f %.3f %.3f %.3f\n      Thickness: %.2f\n",
             inst->fill.fill_color.r,
             inst->fill.fill_color.g,
             inst->fill.fill_color.b,
             inst->fill.fill_color.a,
             inst->fill.outline_color.r,
             inst->fill.outline_color.g,
             inst->fill.outline_color.b,
             inst->fill.outline_color.a,
             inst->fill.outline_thickness);
      printf("      Edges:\n");
      for(uint16_t i=0; i<inst->outline.count-1; i+=3){
        const struct sf3_point *p = &inst->outline.edges[i];
        printf("        %6.2f %6.2f  %6.2f %6.2f  %6.2f %6.2f  %6.2f %6.2f\n",
               p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y, p[3].x, p[3].y);
      }
      break;}
    case SF3_INSTRUCTION_TEXT: {
      const struct sf3_text_instruction *inst = (struct sf3_text_instruction*)instruction;
      printf("      Point: %6.2f %6.2f\n",
             inst->point.x,
             inst->point.y);
      printf("      Color: %.3f %.3f %.3f %.3f\n",
             inst->color.r,
             inst->color.g,
             inst->color.b,
             inst->color.a);
      printf("      Font:  %s\n      Size:  %.2f\n      Text:  %s\n",
             inst->font.str,
             inst->font_size,
             sf3_text_instruction_string(inst));
      break;}
    case SF3_INSTRUCTION_IDENTITY: {
      break;}
    case SF3_INSTRUCTION_MATRIX: {
      for(uint8_t j=0; j<6; ++j){
        if(j%3 == 0) printf("\n        ");
        printf("%f ", ((struct sf3_matrix_instruction*)instruction)->elements[j]);
      }
      break;}
    }
    instruction = sf3_vector_graphic_next_instruction(instruction);
  }
  return 1;
}

int view_file(void *addr, uint8_t type){
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
    fprintf(stderr, "Usage: %s [OPTION...] [FILE...]\n", argv[0]);
    fprintf(stderr, "Print information about SF3 files.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  -b, --brief                do not prepend filenames to output lines\n");
    fprintf(stderr, "  -i, --mime                 output MIME type strings\n");
    fprintf(stderr, "      --extension            output a slash-separated list of extensions\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Report bugs to https://shirakumo.org/projects/libsf3/\n");
    return 0;
  }
  char mime=0, ext=0, brief=0;
  ++argv; --argc;
  while(1){
    if(argv[0][0] != '-')break;
    if(strcmp(argv[0], "-b") == 0 || strcmp(argv[0], "--brief") == 0){
      brief=1;
      ++argv; --argc;
    }else if(strcmp(argv[0], "-i") == 0 || strcmp(argv[0], "--mime") == 0){
      mime=1;
      ++argv; --argc;
    }else if(strcmp(argv[0], "--extension") == 0){
      ext=1;
      ++argv; --argc;
    }else{
      fprintf(stderr, "Unknown option: %s\n", argv[0]);
      return 1;
    }
  }
  for(int i=0; i<argc; ++i){
    sf3_handle handle;
    void *addr;
    size_t size;
    if(!brief) printf("%s: ", argv[i]);
    int type = sf3_open(argv[i], SF3_OPEN_READ_ONLY, &handle);
    if(0 == type){
      printf("%s\n", sf3_strerror(-1));
      continue;
    }
    addr = sf3_data(handle, &size);
    if(!mime && !ext){
      printf("%s file (%s, %lu bytes)\n", sf3_kind(type), sf3_mime_type(type), size);
      if(!sf3_verify(addr, size)){
        printf("Warning: CRC32 checksum does not match!\n");
      }
      view_file(addr, type);
    }
    if(mime){
      printf("%s", sf3_mime_type(type));
    }
    if(ext){
      if(mime)printf(" ");
      printf("%s/%s", sf3_file_type(type), sf3_file_type(0));
    }
    sf3_close(handle);
    printf("\n");
  }
  return 0;
}
