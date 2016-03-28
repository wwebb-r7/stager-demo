#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>

#include <fcntl.h>
#include <dlfcn.h>

int main() {
  int libcfile;
  struct stat statbuf;
  void *libcbuf, *libc;
  void (*my_printf)(char *, char *);

  printf("Hello, %s!\n", "world");
  /**
  libcfile = open("musl/lib/libc.so", O_RDONLY);
  fstat(libcfile, &statbuf);


  libcbuf = mmap(0, statbuf.st_size, PROT_EXEC | PROT_READ, MAP_PRIVATE, libcfile, 0);
  libc = dlopenbuf("new-libc", libcbuf, statbuf.st_size, RTLD_NOW | RTLD_GLOBAL);
  my_printf = dlsym(libc, "printf");

  my_printf("Hello, %s!", "world");
  */
  return 0;
}
