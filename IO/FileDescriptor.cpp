#include "FileDescriptor.h"
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
    errno 是系统全局变量，系统函数失败 → 系统自动给 errno 赋值，只管读取就行
    strerror (errno) → 翻译erron为能看懂的文字
*/

FileDescriptor::FileDescriptor(const std::string& path, int flags, mode_t mode)
{     
   fd = open(path.c_str(), flags, mode);
   if (fd < 0) {
        // 如果失败，直接把系统底层的错误原因（strerror）抛出来，强行打断构造！
        throw std::runtime_error("file open failed: " + std::string(strerror(errno)));
    }
}

FileDescriptor::~FileDescriptor()
{
    // 只有当 fd 是有效的时候，才去 close
    if (fd >= 0) {
        close(fd);
        std::cout << "[Debug] FileDescriptor deleted, fd " << fd << " closed." << std::endl;
    }
}


FileDescriptor::FileDescriptor(FileDescriptor&& other) noexcept {
    std::cout << "触发移动构造操作." << std::endl;
    this->fd = other.fd;
    other.fd = -1;
}

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) noexcept {
    std::cout << "触发移动赋值操作." << std::endl;
    if (this != &other) {
        if (this->fd >= 0) {
            close(this->fd);
        }
        this->fd = other.fd;
        other.fd = -1;
    }
    return *this;
}

bool FileDescriptor::Write(const std::string& str) 
{
    if(fd < 0) {
        // 如果对一个僵尸对象执行 Write，必须严厉报错
        throw std::logic_error("Write failed: File descriptor is invalid (maybe moved).");
    }
    
    const char* buf = str.data(); 
    int len = str.size();
    size_t pos = 0;

    while(len > 0) { // 异常中断，导致write写入不足
        int res = write(fd, buf+pos, len);
        if(res < 0) {
            if (errno == EINTR) {
                continue; // 遇到系统的中断信号 EINTR，什么都没错，重试即可
            }
            throw std::runtime_error("write failed: " + std::string(strerror(errno)));
        }
        len -= res;
        pos += res;
    }
    return true;
}