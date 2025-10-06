#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define BLOCK_SIZE 128
#define TOTAL_SIZE (BLOCK_SIZE * 2)

struct header {
  uint64_t size;
  struct header *next;
};

void print_out(char *format, void *data, size_t data_size) {
  char buf[128];
  ssize_t len = snprintf(buf, sizeof(buf), format,
                         data_size == sizeof(uint64_t) ? *(uint64_t *)data
                                                       : *(void **)data);

  if (len > 0)
    write(STDOUT_FILENO, buf, len);
}

int main() {
  void *start = sbrk(TOTAL_SIZE);
  if (start == (void *)-1) {
    perror("sbrk failed");
    return 1;
  }

  struct header *first = (struct header *)start;
  struct header *second = (struct header *)((char *)start + BLOCK_SIZE);

  first->size = BLOCK_SIZE;
  first->next = NULL;

  second->size = BLOCK_SIZE;
  second->next = first;

  void *first_data = (char *)first + sizeof(struct header);
  void *second_data = (char *)second + sizeof(struct header);

  size_t data_size = BLOCK_SIZE - sizeof(struct header);

  memset(first_data, 0, data_size);
  memset(second_data, 1, data_size);

  printf("first block:       %p\n", (void *)first);
  printf("second block:      %p\n", (void *)second);
  printf("first block size:  %lu\n", first->size);
  printf("first block next:  %p\n", (void *)first->next);
  printf("second block size: %lu\n", second->size);
  printf("second bock next:  %p\n", (void *)second->next);

  unsigned char *fdata = (unsigned char *)first_data;
  unsigned char *sdata = (unsigned char *)second_data;

  for (int i = 0; i < 8; i++)
    printf("%d\n", fdata[i]);
  printf("...\n");
  for (int i = 0; i < 8; i++)
    printf("%d\n", sdata[i]);
  printf("...\n");

  return 0;
}
