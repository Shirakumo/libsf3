## About libsf3
libsf3 is a header-only C99 implementation of the [SF3 (Simple File Format Family)](https://shirakumo.org/docs/sf3) specification of binary file formats. It lets you navigate the file formats directly in-memory without any active parsing at all.

This project also comes with a sample application, `sf3_viewer`, which should illustrate the usage patterns of the library, and can be used to print the contents of an SF3 file for easier viewing.

## How To
First you will want to include the source of the relevant header for your use. You can either include all of SF3 via `sf3.h`, or only include the header of the formats you are interested in vie `sf3_image.h` etc.

In general the expectation of this library is that you will have the entire SF3 file in memory somehow, whether that be via `mmap` or by manually reading it in.

First in order to check whether the file is even a valid SF3 file at all, call `sf3_check`. It will return `0` if the file doesn't seem to be an SF3 file, and the ID of the format type contained within otherwise.

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
if(!addr){
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
