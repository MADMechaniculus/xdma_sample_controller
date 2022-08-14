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

#define PROT_READ (0x1 << 0)
#define PROT_WRITE (0x1 << 1)
#define MAP_SHARED (0x1 << 1)

#endif

class MemoryBase
{
    char * memory{nullptr};
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
                                   PROT_READ | PROT_WRITE, MAP_SHARED, \
                                   fd, offset != 0 ? offset : this->__offset);
        if (this->memory != (void*)-1) {
            return 0;
        }
        
        this->memory = nullptr;
        return -1;
    }

    bool isMapped(void) {
        return this->memory != nullptr;
    }

    char * getMem(void) {
        return this->memory;
    }
};

#endif // MEMORYBASE_H
