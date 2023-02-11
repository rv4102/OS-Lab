#include <fcntl.h>
#include <stdio.h>
#include <sys/file.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: lockfile <filepath>\n");
    return 1;
  }
  const char *filepath = argv[1];
  int fd = open(filepath, O_RDWR | O_CREAT, 0666);
  flock(fd, LOCK_SH);
  printf("process id : %d\n", getpid());
  while (1) {
    write(fd, "locked\n", 7);
    sleep(1);
  }
  return 0;
}