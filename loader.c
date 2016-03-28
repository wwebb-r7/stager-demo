#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

#include <elf.h>

#include <sys/mman.h>

int main() {
  long testfile, testelf;
  struct stat *statbuf;
  unsigned char *testbuf, *t;
  void (*e_entry)(long *, long *);

  long got, rela, relasz, rel, relsz;

  long stack[9] = {0};
  long *dynv;

  statbuf = mmap(0, sizeof(struct stat), PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  testfile = open("test.bin", O_RDONLY);
  testelf = open("test", O_RDONLY);

  fstat(testfile, statbuf);
  testbuf = mmap(0, statbuf->st_size, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_PRIVATE, testfile, 0);

  fstat(testelf, statbuf);
  t = mmap(0, statbuf->st_size, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_PRIVATE, testelf, 0);

  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)t;
	Elf64_Phdr *phdr = (Elf64_Phdr *)(t + ehdr->e_phoff);
  e_entry = (void *) testbuf + 0xf86; // TODO: lookup _start_c symbol or jump to entry point

  // ARGC, ARGV
  stack[0] = 1;
  stack[1] = "libc.so";
  stack[2] = 0;

  // ENV
  stack[3] = "LANG=C";
  stack[4] = 0;

  // AUXV
  stack[5] = AT_BASE; stack[6] = testbuf; // TODO: maybe let musl calculate
  stack[7] = AT_NULL; stack[8] = 0;

  // Dynamic linker info:
  while (phdr->p_type != PT_DYNAMIC) phdr++;
  dynv = (testbuf + phdr->p_vaddr);

  printf("Jumping to %x in test.bin loaded at %x\n", e_entry, testbuf);
  e_entry(stack, dynv);

  return 0;
}
