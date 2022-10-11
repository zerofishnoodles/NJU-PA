#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;  // 文件被打开之后的读写指针
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, invalid_read, serial_write},
  {"/dev/events", 0, 0, events_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char *pathname, int flags, int mode) {
  // omit flags and mode 

  for(int i=0; i<NR_FILES; i++) {
    if(strcmp(pathname, file_table[i].name) == 0) return i;
  }

  panic("Can't find the file!");
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len) {
  // boundary
  if(file_table[fd].open_offset + len >= file_table[fd].size) len = file_table[fd].size - file_table[fd].open_offset;

  if(len < 0 ) len = 0;

  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;

  if(file_table[fd].read == NULL) {
    ramdisk_read(buf, offset, len);
  }else {
    file_table[fd].read(buf, offset, len);
  }

  file_table[fd].open_offset += len;
  return len;
}
// size_t fs_read(int fd, void *buf, size_t len){
//   if(file_table[fd].open_offset+len>=file_table[fd].size)
//     len=file_table[fd].size-file_table[fd].open_offset;
//   if(len<0)
//     len=0;
//   if(file_table[fd].read==NULL)
//     ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
//   else
//     len=file_table[fd].read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
//   file_table[fd].open_offset+=len;
//   return len;
// }

size_t fs_write(int fd, const void *buf, size_t len) {
  // boundary
  if(fd != FD_STDERR && fd != FD_STDIN && fd != FD_STDOUT){
    if(file_table[fd].open_offset + len >= file_table[fd].size) len = file_table[fd].size - file_table[fd].open_offset;
  }
  assert(len >= 0);

  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;

  if(file_table[fd].write != NULL) {
    file_table[fd].write(buf, offset, len);
  }else {
    ramdisk_write(buf, offset, len);
  }

  file_table[fd].open_offset += len;
  return len;
}


size_t fs_lseek(int fd, size_t offset, int whence) {
  switch(whence){
      case SEEK_SET:
          assert(offset >= 0 && offset <= file_table[fd].size);
          file_table[fd].open_offset=offset;
          break;
      case SEEK_CUR:
          assert((int32_t)offset >= -(int32_t)file_table[fd].open_offset && (int32_t)offset <= (int32_t)file_table[fd].size - (int32_t)file_table[fd].open_offset);
          file_table[fd].open_offset+=offset;
          break;
      case SEEK_END:
          assert((int32_t)offset <= 0 && (int32_t)offset + (int32_t)file_table[fd].size >= 0);
          file_table[fd].open_offset=file_table[fd].size+offset;
          break;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  return 0;
}


void init_fs() {
  // TODO: initialize the size of /dev/fb
}


