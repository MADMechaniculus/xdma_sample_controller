#ifndef MEMORYBASE_H
#define MEMORYBASE_H

#include <cstdint>
#include <stdio.h>

#ifndef UNIX
void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, size_t offset) {
    return new char;
}
int munmap(void *addr, size_t length) {
    delete (char*)addr;
    return 0;
}
#endif

class MemoryBase
{
    char * memory{(void*)-1};
    size_t __offset;
    size_t __size;
public:
    MemoryBase();
    MemoryBase(size_t offset, size_t len) : __offset(offset), __size(len) {}

    size_t size() const {
        return this->__size;
    }

    size_t offset() const {
        return this->__offset;
    }

    int mmapFromFd(int fd, size_t offset = 0, size_t len = 0) {
        this->memory = (char*)mmap(NULL, \
                                   len != 0 ? len : this->__size, \
                                   0, 0, \
                                   fd, offset != 0 ? offset : this->__offset);
        if (this->memory != (void*)-1)
            return 0;
        return -1;
    }


};

#endif // MEMORYBASE_H
