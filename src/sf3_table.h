#include "sf3_core.h"

/// The format_id for a table file.
#define SF3_FORMAT_ID_TABLE 0x07

/// The column value types that can be stored.
enum sf3_column_type{
  /// The column elements are stored in 8-bit signed format.
  SF3_COLUMN_INT8 = 0x01,
  /// The column elements are stored in 16-bit signed format.
  SF3_COLUMN_INT16 = 0x02,
  /// The column elements are stored in 32-bit signed format.
  SF3_COLUMN_INT32 = 0x04,
  /// The column elements are stored in 64-bit signed format.
  SF3_COLUMN_INT64 = 0x08,
  /// The column elements are stored in 8-bit unsigned format.
  SF3_COLUMN_UINT8 = 0x11,
  /// The column elements are stored in 16-bit unsigned format.
  SF3_COLUMN_UINT16 = 0x12,
  /// The column elements are stored in 32-bit unsigned format.
  SF3_COLUMN_UINT32 = 0x14,
  /// The column elements are stored in 64-bit unsigned format.
  SF3_COLUMN_UINT64 = 0x18,
  /// The column elements are stored in 16-bit IEEE half-precision
  /// float format.
  SF3_COLUMN_FLOAT16 = 0x22,
  /// The column elements are stored in 32-bit IEEE single-precision
  /// float format.
  SF3_COLUMN_FLOAT32 = 0x24,
  /// The column elements are stored in 64-bit IEEE double-precision
  /// float format.
  SF3_COLUMN_FLOAT64 = 0x28,
  /// The column element is a singular null-terminated string.
  SF3_COLUMN_STRING = 0x31,
  /// The column elements are stored in 64-bit signed format in the
  /// UNIX epoch.
  SF3_COLUMN_TIMESTAMP = 0x48,
  /// The column elements are stored in 64-bit signed format in the
  /// UNIX epoch, with a nanosecond precision.
  SF3_COLUMN_HIGH_RESOLUTION_TIMESTAMP = 0x58,
  /// The column elements are stored in 8-bit unsigned format, where
  /// any value above 0 is "true" and 0 is "false".
  SF3_COLUMN_BOOLEAN = 0x61,
};

/// Description of a column within the table.
struct PACK sf3_column_spec{
  /// The length of the column's values in bytes.
  uint32_t length;
  /// The type of data stored in the column.
  /// See the `sf3_column_type` enumeration.
  uint8_t type;
  /// The name of the column.
  sf3_str16 name;
};

/// A table file.
///
/// See SF3_FORMAT_ID_TABLE
struct PACK sf3_table{
  struct sf3_identifier identifier;
  /// The number of columns in the table.
  uint16_t column_count;
  /// The length of each row in bytes.
  uint64_t row_length;
  /// The number of rows in the table.
  uint64_t row_count;
  /// The length of all the column specs in bytes.
  uint32_t spec_length;
  /// The column descriptions
  struct sf3_column_spec columns[];
};

/// Returns a pointer to the encoded table data.
const char *sf3_table_data(const struct sf3_table *table){
  return ((char*)table->columns)+table->spec_length;
}

/// Returns the next column spec after this one.
/// Note that this function does not perform any bounds checking
/// whatsoever. It is up to you to ensure you do not call this with
/// the last column spec of a table.
const struct sf3_column_spec *sf3_table_next_column(const struct sf3_column_spec *column){
  return (struct sf3_column_spec *)SF3_SKIP_STR(column->name);
}

/// Returns the column spec at the requested index.
/// If an invalid column index is given, null is returned instead.
const struct sf3_column_spec *sf3_table_column(const struct sf3_table *table, uint16_t column){
  if(table->column_count <= column) return 0;
  const struct sf3_column_spec *col = table->columns;
  for(uint16_t i=0; i<column; ++i){
    col = sf3_table_next_column(col);
  }
  return col;
}

/// Returns a pointer to the start of the given row.
/// If an invalid row index is given, null is returned instead.
const char *sf3_table_row(const struct sf3_table *table, uint64_t row){
  if(table->row_count <= row) return 0;
  return sf3_table_data(table)+(table->row_length*row);
}

/// Returns a pointer to the start of the given cell.
/// Also returns ap ointer to the associated column descriptor in
/// spec.
/// If an invalid row or column index is given, null is returned
/// instead and the spec pointer is considered invalid.
///
/// Note that this function needs to scan over all columns preceding
/// the requested column. If you are reading out multiple rows of the
/// same column, writing your own iteration with the cached offset of
/// the column start would be much more efficient.
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

/// Returns the number of bytes per element of the given column.
uint8_t sf3_table_element_size(const struct sf3_column_spec *column){
  return column->type & 0x0F;
}

/// Returns the number of elements in a cell of the given column.
uint32_t sf3_table_element_count(const struct sf3_column_spec *column){
  if(column->type == SF3_COLUMN_STRING) return 1;
  return column->length / (column->type & 0x0F);
}

/// Returns a human-readable string description of the column type.
const char* sf3_table_column_type(enum sf3_column_type type){
  switch(type){
  case SF3_COLUMN_UINT8: return "uint8";
  case SF3_COLUMN_UINT16: return "uint16";
  case SF3_COLUMN_UINT32: return "uint32";
  case SF3_COLUMN_UINT64: return "uint64";
  case SF3_COLUMN_INT8: return "int8";
  case SF3_COLUMN_INT16: return "int16";
  case SF3_COLUMN_INT32: return "int32";
  case SF3_COLUMN_INT64: return "int64";
  case SF3_COLUMN_FLOAT16: return "float16";
  case SF3_COLUMN_FLOAT32: return "float32";
  case SF3_COLUMN_FLOAT64: return "float64";
  case SF3_COLUMN_STRING: return "string";
  case SF3_COLUMN_TIMESTAMP: return "timestamp";
  case SF3_COLUMN_HIGH_RESOLUTION_TIMESTAMP: return "high resolution timestamp";
  case SF3_COLUMN_BOOLEAN: return "boolean";
  default: return "Unknown";
  }
}
