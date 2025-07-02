## About libsf3
libsf3 is a C99 implementation of the [SF3 (Simple File Format Family)](https://shirakumo.org/docs/sf3) specification of binary file formats. It lets you navigate the file formats directly in-memory without any active parsing at all.

You can both use this as a header-only library, or as a shared library to link against.

This project also comes with a sample application, `sf3_viewer`, which should illustrate the usage patterns of the library, and can be used to print the contents of an SF3 file for easier viewing.

## How To
If you are intending on using the library as header-only, first you will want to include the source of the relevant header for your use. You can either include all of SF3 via `sf3.h`, or only include the header of the formats you are interested in vie `sf3_image.h` etc. Alternatively, if you would like to dynamically link instead, you should include *only* `sf3_lib.h` and link against `libsf3`.

In general the expectation of this library is that you will have the entire SF3 file in memory somehow, whether that be via `mmap` or by manually reading it in. In order to check whether a block of memory is even a valid SF3 file at all, call `sf3_check`. It will return `0` if the file doesn't seem to be an SF3 file, and the ID of the format type contained within otherwise.

From there you can simply cast the pointer to the respective format's struct type, and use the various struct fields and accessor functions to read out the contents.

```C
// POSIX mungling to mmap the file
struct stat stat;
int fd = open(file, O_RDONLY);
if(fd == -1) return;

if(fstat(fd, &stat) == -1){
  close(fd);
  return;
}

size_t size = stat.st_size;
void *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
if(addr == MAP_FAILED){
  close(fd);
  return;
}

// Check the file for its header and actual type
int type = sf3_check(addr, size);
if(!type || type != SF3_FORMAT_ID_IMAGE){
  munmap(addr, size);
  close(fd);
  return;
}

// We have an image, so cast the data
struct sf3_image *image = (struct sf3_image *)addr;

// Now we can look at image->height, etc.

// Cleanup
munmap(addr, size);
close(fd);
```

Alternatively if you use the shared library, this simplifies to:

```C
// Open the file.
sf3_handle handle;
int type = sf3_open(file, SF3_OPEN_READ_ONLY, &handle);
if(!type) return;

// Check that the type is what we want
if(type != SF3_FORMAT_ID_IMAGE){
  sf3_close(handle);
  return;
}

// We have an image, so cast the data
size_t size;
void *addr = sf3_data(handle, &size);
struct sf3_image *image = (struct sf3_image *)addr;

// Now we can look at image->height, etc.

// Cleanup
sf3_close(handle);
```

The shared library further includes functionality to write an SF3 file out to disk. To do this, call `sf3_create` with the file in the required format in memory. Constructing the memory as needed is up to you, however. Once the handle is created, you can call `sf3_write` to compute the CRC32 and flush the file out to disk.

If you've opened the file via `sf3_open` with `SF3_OPEN_READ_WRITE`, then you can also call `sf3_write` with a null path to recompute the CRC32 and flush the memory representation back out to file. The usefulness of this is limited though, since the file cannot be resized.
