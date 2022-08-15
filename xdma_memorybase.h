#ifndef MEMORYBASE_H
#define MEMORYBASE_H

#include <cstdint>
#include <cstdio>

#ifndef unix
#error UNIX OS required! This code not for non UNIX execution!
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

#else
#include <sys/mman.h>
#endif

/**
 * @brief Обёртка базового смещения в памяти
 */
class MemoryBase
{
    char * memory{nullptr};
    size_t __offset;
    size_t __size;
public:
    MemoryBase();
    MemoryBase(size_t offset, size_t len);

    size_t size() const;

    size_t offset() const;

    int mmapFromFd(int fd, size_t offset = 0, size_t len = 0);

    bool isMapped(void);

    char * getMem(void);
};

#endif // MEMORYBASE_H
