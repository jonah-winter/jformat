#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

// #define SRC_DIR ${CMAKE_SOURCE_DIR}
#define BUFFER_SIZE 1024

int open_file(const char* fpath, char** map, size_t* size_out, int* fdescriptor)
{
  *fdescriptor = open(fpath, O_RDWR);
  if (*fdescriptor == -1) {
    perror("could not open file\n");
    return 1;
  }
  struct stat stat_buffer;
  if (fstat(*fdescriptor, &stat_buffer) == -1) { 
    perror("couldnt get file size\n"); 
    close(*fdescriptor);
    return 1;
  }

  if (stat_buffer.st_size == 0) {
    close(*fdescriptor);
    return 0;
  }
  *map = (char*)mmap(NULL, stat_buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, *fdescriptor, 0);

  if (map == MAP_FAILED) {
    perror("mmap failed");
    close(*fdescriptor);
    return 0;
  }

  *size_out = stat_buffer.st_size;

  return 0;
}

int close_file(char** map, size_t* size, int* file_descriptor) {
  munmap(*map, *size);
  close(*file_descriptor);
  return 0;
}
int replace_char(char* map, size_t size, char target, char replacement) {
  if (map == NULL || size == 0) return 1;

  for (size_t i = 0; i < size; i++) {
    if (map[i] == target) {
      map[i] = replacement;
    }
  }
  return 0;
}

int replace_str_inclusive(char* map, size_t size, const char* target, const char* replacement) {
  if (map == NULL || size == 0) return 0;
  size_t idx = 0;

  for (size_t i = 0; i < size; i++) {
    if (map[i] == target[idx]) {
      if (strlen(target) == idx + 1) {
        size_t repl_idx = 0;
        for (size_t z = i - idx; z <= i; z++) {
          map[z] = replacement[repl_idx];
          ++repl_idx;
        }
        idx = 0;
      }
      ++idx;
    } else {
      idx = 0;
    }
  }
  
  return 0;
}

int is_al(char ch) {
  int asc = (int)ch;
  // 65-90
  // 97-122
  // 1 is true 0 is false
  // obviously
  if (asc >= 65 && asc <= 90) return 1;
  if (asc >= 97 && asc <= 122) return 1;
  return 0;
}

int replace_str_exclusive(char* map, size_t size, const char* target, const char* replacement) {
  if (map == NULL || size == 0) return 0;
  size_t idx = 0;
  
  for (size_t i = 0; i < size; i++) {
    if (map[i] == target[idx]) {
      if (strlen(target) == idx + 1 
          && (i + 2 == size || !is_al(map[i + 1]))) {
        size_t repl_idx = 0;
        for (size_t z = i - idx; z <= i; z++) {
          map[z] = replacement[repl_idx];
          ++repl_idx;
        }
        idx = 0;
      }
      ++idx;
    } else {
      size_t y = 0;
      while (is_al(map[i + y]) && i + y < size) {
        ++y;
      }
      if (i + y < size) {
        i += y;
      }
      idx = 0;
    }
  }

  return 0;
}

int add_str(char* map, size_t size, const char* target, const char* add) {
  if (map == NULL || size == 0) return 0;
  size_t idx = 0;

  for (size_t i = 0; i < size; i++) {
    if (map[i] == target[idx]) {
      if (strlen(target) == idx + 1) {
        idx = 0;
      }
      ++idx;
    } else {
      idx = 0;
    }
  }
  return 0;
}

int main(void) {
  char filepath[BUFFER_SIZE];
  snprintf(filepath, sizeof(filepath), "%s/test.c", SRC_DIR);
  
  char* map;
  size_t map_size;
  int fd;

  if (open_file(filepath, &map, &map_size, &fd) == 0 && map != NULL) {
    replace_char(map, map_size, 's', 'S');
    replace_char(map, map_size, 'i', '0');
    replace_str_exclusive(map, map_size, "world", "hello");
    close_file(&map, &map_size, &fd);
  }
}
