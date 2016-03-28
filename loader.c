#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#include <elf.h>

#include <sys/mman.h>

unsigned char *st_str(unsigned char *elf, long index) {
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf;

  Elf64_Shdr *shdr = (Elf64_Shdr *)(elf + ehdr->e_shoff);
  while (shdr->sh_type != SHT_SYMTAB) shdr++;
  while (shdr->sh_type != SHT_STRTAB) shdr++; // The first strtab after the symtab

  return elf + shdr->sh_offset + index;
}

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

  Elf64_Shdr *shdr = (Elf64_Shdr *)(t + ehdr->e_shoff);
  while (shdr->sh_type != SHT_SYMTAB) shdr++;

  Elf64_Sym *symb = (Elf64_Sym *)(t + shdr->sh_offset);
  while (strcmp(st_str(t, symb->st_name), "_start_c") != 0) symb++;

  e_entry = (void *) testbuf + symb->st_value; // TODO: make work with entry point?

  // ARGC, ARGV
  stack[0] = 1;
  stack[1] = "libc.so";
  stack[2] = 0;

  // ENV
  stack[3] = "LANG=C";
  stack[4] = 0;

  // AUXV
  stack[5] = AT_BASE; stack[6] = testbuf; // TODO: maybe let musl calculate?
  stack[7] = AT_NULL; stack[8] = 0;

  // Dynamic linker info:
  while (phdr->p_type != PT_DYNAMIC) phdr++;
  dynv = (testbuf + phdr->p_vaddr);

  printf("Jumping to %x in test.bin loaded at %x\n", e_entry, testbuf);
  e_entry(stack, dynv);

  return 0;
}
