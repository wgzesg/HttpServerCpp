#include "utils.h"
#include <fcntl.h>
namespace MyHttp{
int setNonBlocking(int fd) {
	if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) ==
	    -1) {
		return -1;
	}
	return 0;
}
}