#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class FileDescriptor
{
private:
   int fd;
public:
   FileDescriptor();
   ~FileDescriptor();
   FileDescriptor(const FileDescriptor&) = delete;
   FileDescriptor& operator=(const FileDescriptor&) = delete;

public:
   bool Write();
};
 
FileDescriptor::FileDescriptor(const std::string& path, int flags)
{     
   fd = open(path, flags);
   if (fd < 0) {
      std::cout << "file open failed." << std::endl;
      return;
   }
}

FileDescriptor::~FileDescriptor()
{
   close(fd);
   std::cout << "file open failed." << std::endl;
}
