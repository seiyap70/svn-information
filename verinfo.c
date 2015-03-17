#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>
#include <errno.h>
#include <assert.h>

#include "svn_info.h"

#define BUILD64

#ifdef BUILD32
#define BITS(name) name##32
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Shdr Elf32_Shdr
#define Elf_Addr Elf32_Addr
#elif defined(BUILD64)
#define BITS(name) name##64
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Shdr Elf64_Shdr
#define Elf_Addr Elf64_Addr
#endif

void print_error(char *file, unsigned int line, int errnum, char *s) {
    fflush(stdout);
    if (file == NULL || line == 0)
        fprintf(stderr, "[-] %s: %s\n", s, strerror(errnum));
    else
        fprintf(stderr, "[-] %s:%d: %s: %s\n", file, line, s, strerror(errnum));
}

#define __FATAL(name) do { \
    print_error(__FILE__, __LINE__-1, errno, name); \
    goto _fatal; \
} while(0)

int check_elf(Elf_Ehdr *ehdr) {
    if (ehdr->e_type != ET_EXEC) {
        fprintf(stderr, "[-] ELF not ET_EXEC\n");
        return 0;
    }

    if (ehdr->e_ident[EI_VERSION] != EV_CURRENT) {
        fprintf(stderr, "[-] ELF not current version\n");
        return 0;
    }

#ifdef BUILD32
    if (ehdr->e_machine != EM_386) {
        fprintf(stderr, "[-] ELF not EM_386\n");
        return 0;
    }
#elif defined(BUILD64)
    if (ehdr->e_machine != EM_X86_64) {
        fprintf(stderr, "[-] ELF not EM_X86_64\n");
        return 0;
    }
#endif
    return 1;
}

char *read_section(int fd, Elf_Shdr *shdr)
{
	char *buffer = NULL;
	int res;
	if ((buffer = (char*)malloc(shdr->sh_size)) != NULL)
	{
		if (lseek(fd, shdr->sh_offset, SEEK_SET) != -1)
		{
			 if ((res = read(fd, buffer, shdr->sh_size)) == shdr->sh_size)
			 {
				 return buffer;
			 }
		}
		free(buffer);
		buffer = NULL;
	}
	return buffer;
}

int print_verion_info(int fd, int fdout)
{
	
	Elf_Ehdr ehdr;
    Elf_Phdr *phdr = NULL, *next_phdr = NULL;
	Elf_Shdr *shdr = NULL, *next_shdr = NULL,*str_shdr=NULL;
	const char*str_section=NULL;
	char *str_table = NULL,*char_info=NULL;
	struct svn_informations *psvn_info = NULL;
	int res;
	int i;
	size_t psz;
	
	 if (lseek(fd, 0, SEEK_SET) == -1)
        	__FATAL("lseek");
	
	if ((res = read(fd, &ehdr, sizeof(ehdr))) == -1)
        	__FATAL("read");
    else if (res != sizeof(ehdr)) {
        	fprintf(stderr, "[-] File is too small\n");
        	goto _fatal;
    }

    if (!check_elf(&ehdr))
 		goto _fatal;
	
#ifdef BUILD32
	printf("[+] x86-32bit ELF\n");
#elif defined(BUILD64)
	printf("[+] x86-64bit ELF\n");
#endif
	 // read phdrs
    if ((phdr = malloc(ehdr.e_phnum * sizeof(Elf_Phdr))) == NULL)
        __FATAL("malloc");
    
    if (lseek(fd, ehdr.e_phoff, SEEK_SET) == -1)
        __FATAL("lseek");

    if ((res = read(fd, phdr, ehdr.e_phnum * sizeof(Elf_Phdr))) == -1)
        __FATAL("read");
    else if (res != ehdr.e_phnum * sizeof(Elf_Phdr)) {
        fprintf(stderr, "[-] File is too small\n");
        goto _fatal;
    }

    // read shdrs
    if (ehdr.e_shnum > 0) {
        if ((shdr = malloc(ehdr.e_shnum * sizeof(Elf_Shdr))) == NULL)
            __FATAL("malloc");

        if (lseek(fd, ehdr.e_shoff, SEEK_SET) == -1)
            __FATAL("lseek");

        if ((res = read(fd, shdr, ehdr.e_shnum * sizeof(Elf_Shdr))) == -1)
            __FATAL("read");
        else if (res != ehdr.e_shnum * sizeof(Elf_Shdr)) {
            fprintf(stderr, "[-] File is too small\n");
            goto _fatal;
        }
    }
	printf("section header number:%d,str index %d\n", ehdr.e_shnum,ehdr.e_shstrndx);
	
	str_shdr=&shdr[ehdr.e_shstrndx];
	str_table = read_section(fd, str_shdr);
	if (!str_table) {
        fprintf(stderr, "[-] read string share header error\n");
        goto _fatal;
    }
	
	for(i = 0, next_shdr=shdr; i < ehdr.e_shnum; i++,next_shdr++)
	{
		str_section = str_table + next_shdr->sh_name;
		if(!strcmp(str_section,VERINFO_SECTION_NAME))
		{
			//found you!
			assert(next_shdr->sh_size == sizeof(struct svn_informations));
			psvn_info=(struct svn_informations*)read_section(fd, next_shdr);
			printf("SVN informations:\n");
			printf("last svn modify author:%s\n", psvn_info->author);
			printf("last svn url:%s\n", psvn_info->svnurl_info);
			printf("last svn work directory:%s\n", psvn_info->work_dir);
			printf("last svn work revision:%s\n", psvn_info->work_rev);
			printf("last svn revision:%s\n", psvn_info->last_src_rev);
			printf("last date:%s\n", psvn_info->last_date_info);
						
			
			break;
		}
	}
	
	
_fatal:
		return res;
}

int main(int argc, char **argv)
{
	int fd;

	if(argc < 2)
	{
		printf("arguments error");
		return -1;
	}
	
	fd = open(argv[1],O_RDONLY);
	if(fd < 0)
	{
		perror("open");	
		return -1;
	}
	return print_verion_info(fd,fileno(stdout));
	
}
