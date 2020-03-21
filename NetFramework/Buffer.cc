#include "Buffer.h"

ssize_t Buffer::readFd(int fd, int *savedErrno)
{
	char extrabuf[65526];
	struct iovec vec[2];
	const size_t writable = writableBytes();
	vec[0].iov_base = &*buffer_.begin()+writerIndex_;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof(extrabuf);
	const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
	const ssize_t n = readv(fd, vec, iovcnt);
	id (n < 0)
	{
		*savedErrno = errno;
	}
	else if (static_cast<size_t>(n) <= writable)
	{
		writerIndex += n;
	}
	else
	{
		writerIndex_ = buffer_.size();
		append(extrabuf, n - writable);
	}
	return n;
}