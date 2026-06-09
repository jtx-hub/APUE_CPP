#include "FileDescriptor.h"
 #include <unistd.h>

bool FileDescriptor::Write(const std::string& str) 
{
    if(fd < 0) {
        std::cout << "fd error, write failed."
        return false;
    }
    
    const char* buf = str.data(); 
    int len = str.size();

    size_t pos = 0;

    while(len > 0) { // 异常中断，导致write写入不足
        int res = write(fd, buf+pos, len);
        if(res < 0) {
            std::cout << "write failed, str: " << str << ".\n" << std::endl;
            break;
        }
        len -= res;
        pos += res;
    }
}