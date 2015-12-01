#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define NCCS 19
#ifndef _IOC_SIZEBITS
# define _IOC_SIZEBITS	14
#endif

#ifndef _IOC_DIRBITS
# define _IOC_DIRBITS	2
#endif

#define _IOC_NRMASK	((1 << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK	((1 << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK	((1 << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK	((1 << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT+_IOC_SIZEBITS)




#ifndef _IOC_WRITE
# define _IOC_WRITE     1U
#endif

#ifndef _IOC_READ
# define _IOC_READ	2U
#endif




#define _IOC(dir,type,nr,size) 		\
	(((dir)  << _IOC_DIRSHIFT)  | 	\
	 ((type) << _IOC_TYPESHIFT) | 	\
	 ((nr)   << _IOC_NRSHIFT)   | 	\
	 ((size) << _IOC_SIZESHIFT))

#define _IOC_TYPECHECK(t) (sizeof(t))
#define _IOW(type,nr,size)      _IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOR(type,nr,size)      _IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))

typedef unsigned char	cc_t;
typedef unsigned int	speed_t;
typedef unsigned int	tcflag_t;

#define NNCCS 19


#define    BOTHER 0010000
#define    B57600 0010001
#define   B115200 0010002
#define   B230400 0010003
#define   B460800 0010004
#define   B500000 0010005
#define   B576000 0010006
#define   B921600 0010007
#define  B1000000 0010010
#define  B1152000 0010011
#define  B1500000 0010012
#define  B2000000 0010013
#define  B2500000 0010014
#define  B3000000 0010015
#define  B3500000 0010016
#define  B4000000 0010017


struct mtermios2 {
	tcflag_t c_iflag;		/* input mode flags */
	tcflag_t c_oflag;		/* output mode flags */
	tcflag_t c_cflag;		/* control mode flags */
	tcflag_t c_lflag;		/* local mode flags */
	cc_t c_line;			/* line discipline */
	cc_t c_cc[NNCCS];		/* control characters */
	speed_t c_ispeed;		/* input speed */
	speed_t c_ospeed;		/* output speed */
};

#define MTCSETS2		_IOW('T', 0x2B, struct mtermios2)
#define MTCGETS2		_IOR('T',0x2A, struct mtermios2)

/* baudrate: eg. 912600 */

int setbaudrate(int fd, int baudrate)
{
	int ret;
	struct mtermios2 tio;

	ret = ioctl(fd, MTCGETS2, &tio);
	if (ret != 0)
		return -1;
	tio.c_cflag &= ~CBAUD;
	tio.c_cflag |= BOTHER;
	tio.c_ispeed = baudrate;
	tio.c_ospeed = baudrate;
	ret = ioctl(fd, MTCSETS2, &tio);
	if (ret != 0)
		return -1;
	return 0;
}


#if 0
int main(int argc, char *argv[])
{
	int fd, ret;

	struct mtermios2 tio;

	fd = open(argv[1], O_RDWR);

	if (fd < 0) {
		fprintf(stderr, "open error!\n");
		return -1;
	}

	ret = ioctl(fd, MTCGETS2, &tio);
	printf("ret = %d\n", ret);
	tio.c_cflag &= ~CBAUD;
	tio.c_cflag |= BOTHER;
	tio.c_ispeed = 921600;
	tio.c_ospeed = 921600;
	ret = ioctl(fd, MTCSETS2, &tio);
	printf("ret = %d\n", ret);

	return 0;
}

#endif

