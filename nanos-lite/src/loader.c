#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
extern size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
  uint32_t e_offset = (uint32_t)&ramdisk_start; // assume this ELF file is at the 0 offest of the ramdisk
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));   // ramdisk_read is already add the ramdisk_start
  Elf_Phdr phdr;
  for(uint32_t i = 0; i < ehdr.e_phnum; i++) {;
    ramdisk_read(&phdr, ehdr.e_phoff + i * ehdr.e_phentsize, ehdr.e_phentsize);
    if(phdr.p_type == PT_LOAD) {
      Log("%x %x %d", phdr.p_vaddr, e_offset + (void *)phdr.p_offset, phdr.p_filesz);
      memcpy((void *)phdr.p_vaddr, e_offset + (void *)phdr.p_offset, phdr.p_filesz); // assume this ELF file is at the 0 offest of the ramdisk
      memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
    }
    
  }

  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
