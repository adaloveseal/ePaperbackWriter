#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <arpa/inet.h>

struct psf2_header {
	unsigned int magic;
	unsigned int version;
	unsigned int headersize;    /* offset of bitmaps in file */
	unsigned int flags;
	unsigned int length;	/* number of glyphs */
	unsigned int charsize;      /* number of bytes for each character */
	unsigned int height, width; /* max dimensions of glyphs */
};

int main(int argc, char **argv) {
	if (argc < 3) return 1;
	int asc = atoi(argv[2]);
	if ((asc < 0) || (asc > 255)) return 6;
	int fd = open(argv[1], O_RDONLY);
	if (fd == 0) return 2;
	struct stat filestat;
	fstat(fd, &filestat);
	if (filestat.st_size < sizeof(struct psf2_header)) return 3;
	void *data = mmap(NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	struct psf2_header *psf = data;
	if (psf->magic != 0x864ab572) {
		fprintf(stderr, "Wrong magic number: got %x, should be 0x864ab572\n", psf->magic);
		return 4;
	}
	if (filestat.st_size < psf->headersize + psf->length * psf->charsize) return 5;
	char *bitmap = data + psf->headersize + asc * psf->charsize;
	int i, j, width = (psf->width + 7) / 8;
	printf("+");
	for (i = 0; i < psf->width; i++)
		printf("-");
	printf("+\n");
	for (i = 0; i < psf->charsize; i += width) {
		printf("|");
		short *row = (short*)(bitmap + i);
		for (j = width * 8 - 1; j >= width * 8 - 0 - psf->width; j--) {
			printf("%c", (ntohs(*row) & 1 << j) >> j ? '*' : ' ');
		}
		printf("|\n");
	}
	printf("+");
	for (i = 0; i < psf->width; i++)
		printf("-");
	printf("+\n");
	munmap(data, filestat.st_size);
	close(fd);
	return 0;
}
