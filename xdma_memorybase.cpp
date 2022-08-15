#include "xdma_memorybase.h"

MemoryBase::MemoryBase(size_t offset, size_t len) : __offset(offset), __size(len) {}

size_t MemoryBase::size() const {
    return this->__size;
}

size_t MemoryBase::offset() const {
    return this->__offset;
}

int MemoryBase::mmapFromFd(int fd, size_t offset, size_t len) {
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

bool MemoryBase::isMapped() {
    return this->memory != nullptr;
}

char *MemoryBase::getMem() {
    return this->memory;
}
