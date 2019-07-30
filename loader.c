/*
 *  Team No 20 
 *  Loader for static elf32 binaries. Tested with diet lib.
 *  diet gcc -static -g -o loader loader.c -Wl,--section-start -Wl,.text=0x100800 -Wl,--section-start -Wl,.note.gnu.build-id=0x200000
 *  Credits: This was taken from the class assignment notes and discussion. I thank Professor Peter for help and encouragement during this exercise.
 *       
 */


#include <stdio.h>
#include "elf32.h"
#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char **argv, char **envp){
 int fd;
 // Open file that is compiled file in this case hello compiled using diet for smaller text segment
 if ((fd = open(argv[1],O_RDONLY)) < 0) {
 perror("open");
 exit(1);
 }
 
 //set AUX vector
   set_auxillary_vector(envp);
  
  
// Define header format
 struct elf32_ehdr elf_header;
 read(fd, &elf_header, sizeof(elf_header));
 assert(elf_header.e_phentsize == sizeof(struct elf32_phdr));
 int size_segments = elf_header.e_phnum;
 // struct for segments
 struct elf32_phdr p_headers[size_segments];
 lseek(fd, elf_header.e_phoff, SEEK_SET);
 read(fd, p_headers, sizeof(p_headers));
 
 //printf("Program header table entry count %d \n", size_segments );
 

 // check for the Loadable segments 
 
 for(int i=0;i<size_segments;i++){
	 
	 if(p_headers[i].p_type == PT_LOAD){
	 
	     int offset = ((uint32_t)p_headers[i].p_vaddr)& 0xFFF;
             void *base = (void *)((uint32_t) p_headers[i].p_vaddr -offset);
             int len = (p_headers[i].p_memsz + offset + 0xFFF) & 0xFFFFF000;
             char *buf= mmap(base,len, PROT_READ| PROT_WRITE | PROT_EXEC ,  MAP_PRIVATE | MAP_ANONYMOUS| MAP_FIXED, -1, 0);
             lseek(fd,p_headers[i].p_offset, SEEK_SET);
             read(fd,buf + offset, p_headers[i].p_filesz);

             //printf("Segment no %d is loadable with Virtual Address at %x \n", i, phdrs[i].p_vaddr);
	 }

       

 }
  

  int *p =  (int *)argv;
  p[0] = argc -1 ;
  p[-1] = (uint32_t) elf_header.e_entry;

 // Does cleanup 
  cleanup(p);
  
  
 return 0;
}


void cleanup(int *p){
//Thank you piazza.	
__asm__("movl $0, %edx\n");
__asm__("movl %0, %%esp\n" : : "r" (p - 1) : );
__asm__("ret");
	
}

void set_auxillary_vector(char **envp){
//Avoiding segfault 	
	 while (*envp) {envp++;}
  int *aux_vector = (int *)(envp+1) ;
  while (*aux_vector){
  if (*aux_vector == 3){
      *aux_vector = 100;
      }
  aux_vector += 2;
}
	
}
 
