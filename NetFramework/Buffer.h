#include <sys/types.h>
#include <vector>
#include <string>

class Buffer 
{
public:
	static const size_t kCheapPrepend = 8;
	static const size_t kInitialSize = 1024;
	
	explicit Buffer(size_t initialSize = kInitialSize):
		buffer_(kCheapPrepend + initialSize),
		readerIndex_(kCheapPrepend),
		writerIndex_(kCheapPrepend)
		{
			
		}
	
	size_t readableBytes() const
	{
		return writerIndex_ - readerIndex_;
	}


    ssize_t readFd(int fd, int *savedErrno);
	
	size_t writableBytes() const
	{
		return buffer_.size() - writerIndex_;
	}
	
	size_t prependableBytes() const
	{
		return readerIndex_;
	}
	
	//这个作用是指针偏移到rederIndex_的位置上；
	const char *peek() const
	{
		return &*buffer_.begin() + readerIndex_;
	}
	
	void retrieve(size_t len)
	{
		if (len <= readableBytes())
		{
			readerIndex_ += len;
		}
		else
		{
			retrieveAll();
		}
	}
	
	void retrieveAll()
	{
		readerIndex_ = kCheapPrepend;
		writerIndex_  = kCheapPrepend;
	}
	
	void append(const char *data, size_t len)
	{
		ensureWritableBytes(len);
		std::copy(data, data+len, &*buffer_.begin()+writerIndex_);
		writerIndex_ += len;
	}

	std::string retrieveAllAsString()
	{
		std::string result(peek(), readableBytes());
		return result;
	}
	
	void ensureWritableBytes(size_t len)
	{
		if (writableBytes() < len)
		{
			makeSpace(len);
		}
	}
	
	void makeSpace(size_t len)
	{
		//剩余空间包括：可写的+头部剩余的-固定头部长度
		if (writableBytes() + prependableBytes() - kCheapPrepend < len)
		{
			buffer_.resize(writerIndex_+len);
		}
		else
		{
			//挪空间
			size_t readable = readableBytes();
			std::copy(&*buffer_.begin()+readerIndex_, &*buffer_.begin()+writerIndex_, &*buffer_.begin()+kCheapPrepend);
			readerIndex_ = kCheapPrepend;
			writerIndex_ = readerIndex_ + readable;
		}
	}
	
	void append(const void *data, size_t len)
	{
		append(static_cast<const char *>(data), len);
	}

private:
	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;
};
