#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class FileDescriptor
{
private:
   int fd;
public:
   FileDescriptor(const std::string& path, int flags, mode_t mode);
   ~FileDescriptor();
   FileDescriptor(const FileDescriptor&) = delete;
   FileDescriptor& operator=(const FileDescriptor&) = delete;

   FileDescriptor(FileDescriptor&&) = default;
   FileDescriptor& operator=(FileDescriptor&&) = default;

public:
   bool Write(const std::string& str);
};

 

