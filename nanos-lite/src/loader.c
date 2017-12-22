#include "common.h"
#include "memory.h"

//#define DEFAULT_ENTRY ((void *)0x4000000)
#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern size_t fs_filesz(int fd);
extern int fs_open(const char *pathname, int flags, int mode);
extern int fs_close(int fd);
extern void _map(_Protect *p, void *va, void *pa);

uintptr_t loader(_Protect *as, const char *filename) {
  //ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());

	//int fd = fs_open(filename, 0, 0);
	//printf("fd = %d\n", fd);
	//fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd)); 
	//fs_close(fd); 
	
	int fd = fs_open(filename, 0, 0);
	int filesize = fs_filesz(fd);
	void *vaddr, *page;
	vaddr = DEFAULT_ENTRY;

	//printf("filesize = %x\n", filesize);
	//printf("filesize - PGSIZE = %x\n", filesize - PGSIZE);
	//printf("filesize - 3*PASIZE = %x\n", filesize - 3*PGSIZE);
	//printf("filesize - 5*PASIZE = %x\n", filesize - 5*PGSIZE);

	while (filesize > 0) {
		page = new_page();
		_map(as, vaddr, page);
		fs_read(fd, page, PGSIZE);
		vaddr += PGSIZE;
		filesize = filesize - PGSIZE;
		//printf("filesize = %x\n", filesize);
	}
	fs_close(fd);

	return (uintptr_t)DEFAULT_ENTRY;
}
