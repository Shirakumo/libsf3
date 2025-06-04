#include "sf3_archive.h"
#include "sf3_audio.h"
#include "sf3_image.h"
#include "sf3_log.h"
#include "sf3_model.h"
#include "sf3_physics_model.h"
#include "sf3_table.h"
#include "sf3_text.h"
#include "sf3_vector_graphic.h"

/// Returns a human-readable string description of the SF3 format_id.
char *sf3_kind(int type){
  switch(type){
  case SF3_FORMAT_ID_ARCHIVE:
    return "Archive";
  case SF3_FORMAT_ID_AUDIO:
    return "Audio";
  case SF3_FORMAT_ID_IMAGE:
    return "Image";
  case SF3_FORMAT_ID_LOG:
    return "Log";
  case SF3_FORMAT_ID_MODEL:
    return "Model";
  case SF3_FORMAT_ID_PHYSICS_MODEL:
    return "Physics Model";
  case SF3_FORMAT_ID_TABLE:
    return "Table";
  case SF3_FORMAT_ID_TEXT:
    return "Text";
  case SF3_FORMAT_ID_VECTOR_GRAPHIC:
    return "Vector Graphic";
  default:
    return "Unknown";
  }
}

/// Returns the standard file type suffix for the given SF3 format_id.
char *sf3_file_type(int type){
  switch(type){
  case SF3_FORMAT_ID_ARCHIVE:
    return "ar.sf3";
  case SF3_FORMAT_ID_AUDIO:
    return "au.sf3";
  case SF3_FORMAT_ID_IMAGE:
    return "img.sf3";
  case SF3_FORMAT_ID_LOG:
    return "log.sf3";
  case SF3_FORMAT_ID_MODEL:
    return "mod.sf3";
  case SF3_FORMAT_ID_PHYSICS_MODEL:
    return "phys.sf3";
  case SF3_FORMAT_ID_TABLE:
    return "tab.sf3";
  case SF3_FORMAT_ID_TEXT:
    return "txt.sf3";
  case SF3_FORMAT_ID_VECTOR_GRAPHIC:
    return "vec.sf3";
  default:
    return "sf3";
  }
}

/// Returns the standard media mime-type for the given SF3 format_id.
char *sf3_mime_type(int type){
  switch(type){
  case SF3_FORMAT_ID_ARCHIVE:
    return "application/x.sf3-archive";
  case SF3_FORMAT_ID_AUDIO:
    return "audio/x.sf3";
  case SF3_FORMAT_ID_IMAGE:
    return "image/x.sf3";
  case SF3_FORMAT_ID_LOG:
    return "application/x.sf3-log";
  case SF3_FORMAT_ID_MODEL:
    return "model/x.sf3";
  case SF3_FORMAT_ID_PHYSICS_MODEL:
    return "model/x.sf3-physics";
  case SF3_FORMAT_ID_TABLE:
    return "application/x.sf3-table";
  case SF3_FORMAT_ID_TEXT:
    return "application/x.sf3-text";
  case SF3_FORMAT_ID_VECTOR_GRAPHIC:
    return "image/x.sf3-vector";
  default:
    return "application/x.sf3";
  }
}
