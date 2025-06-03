#include "sf3-core.h"

#define SF3_FORMAT_ID_TABLE 0x07

enum sf3_column_type{
  SF3_COLUMN_UINT8 = 0x01,
  SF3_COLUMN_UINT16 = 0x02,
  SF3_COLUMN_UINT32 = 0x04,
  SF3_COLUMN_UINT64 = 0x08,
  SF3_COLUMN_INT8 = 0x11,
  SF3_COLUMN_INT16 = 0x12,
  SF3_COLUMN_INT32 = 0x14,
  SF3_COLUMN_INT64 = 0x18,
  SF3_COLUMN_FLOAT16 = 0x22,
  SF3_COLUMN_FLOAT32 = 0x24,
  SF3_COLUMN_FLOAT64 = 0x28,
  SF3_COLUMN_STRING = 0x31,
  SF3_COLUMN_TIMESTAMP = 0x48,
  SF3_COLUMN_HIGH_RESOLUTION_TIMESTAMP = 0x58,
  SF3_COLUMN_BOOLEAN = 0x61,
};

struct __attribute__((packed)) sf3_column_spec{
  uint32_t length;
  uint8_t type;
  sf3_str16 name;
};

struct __attribute__((packed)) sf3_table{
  struct sf3_identifier identifier;
  uint16_t column_count;
  uint64_t row_length;
  uint64_t row_count;
  uint32_t spec_length;
  struct sf3_column_spec columns[];
};

const char *sf3_table_data(const struct sf3_table *table){
  return ((char*)table->columns)+table->spec_length;
}

const struct sf3_column_spec *sf3_table_next_column(const struct sf3_column_spec *column){
  return (struct sf3_column_spec *)SF3_SKIP_STR(column->name);
}

const struct sf3_column_spec *sf3_table_column(const struct sf3_table *table, uint16_t column){
  if(table->column_count <= column) return 0;
  const struct sf3_column_spec *col = table->columns;
  for(uint16_t i=0; i<column; ++i){
    col = sf3_table_next_column(col);
  }
  return col;
}

const char *sf3_table_row(const struct sf3_table *table, uint64_t row){
  if(table->row_count <= row) return 0;
  return sf3_table_data(table)+(table->row_count*row);
}

const char *sf3_table_cell(const struct sf3_table *table, uint64_t row, uint64_t column, const struct sf3_column_spec **spec){
  if(table->column_count <= column) return 0;
  if(table->row_count <= row) return 0;
  const struct sf3_column_spec *col = table->columns;
  const char *data = sf3_table_row(table, row);
  for(uint16_t i=0; i<column; ++i){
    data += col->length;
    col = sf3_table_next_column(col);
  }
  *spec = col;
  return data;
}

uint8_t sf3_table_element_size(const struct sf3_column_spec *column){
  return column->type & 0x0F;
}

uint32_t sf3_table_element_count(const struct sf3_column_spec *column){
  if(column->type == SF3_COLUMN_STRING) return 1;
  return column->length / (column->type & 0x0F);
}
