/**
 * @file xdma_proxy.h
 * @author Okhalov Steve (ohalov.sk@niistt.ru)
 * @brief
 * @version 0.1
 * @date 2022-08-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <cstdint>

#ifndef UNIX
#error UNIX OS required!

#define PROT_READ (0x1 << 0)
#define PROT_WRITE (0x1 << 1)
#define MAP_SHARED (0x1 << 0)

void * mmap (void *addr, size_t length, int prot, int flags, int fd, size_t offset)
{
    return new char[length];
}

int munmap (void *addr, size_t length)
{
    delete addr;
    return 0;
}
#endif

class BaseMemoryWrapper
{
private:
    char *memory{ nullptr };
    size_t mappedSize;
    size_t baseOffset;

public:
    BaseMemoryWrapper (size_t offset, size_t length) : baseOffset(offset), mappedSize(length) {}
    ~BaseMemoryWrapper () {
        this->unmapMemory();
    }

    /**
   * @brief Проверка состояния памяти
   * @return true В случае, когда память инициализирована
   * @return false В случае, когда память не инициализирована
   */
    bool isMapped () const {
        return this->memory != nullptr;
    }

    /**
   * @brief Инициализция памяти из файлового дескриптора
   *
   * @param fd Файловый дескриптор
   * @param offset Смещение базового адреса в дескрипторе
   * @param length Размер инициализируемой памяти
   * @return Код ошибки
   */
    int mapFromFd (int fd) {
        this->memory = (char *)mmap (NULL, this->mappedSize, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd, this->baseOffset);

        if (this->memory == (void *)-1)
        {
            return -1;
        }

        return 0;
    }

    /**
   * @brief Освобождение памяти
   * @return Код ошибки
   */
    int unmapMemory (void) {
        if (this->memory != nullptr)
        {
            munmap (this->memory, this->mappedSize);
            mappedSize = 0;
            baseOffset = 0;
            return 0;
        }

        return -1;
    }
};

class XdmaCoreBase
{
private:
    BaseMemoryWrapper base;

public:
    XdmaCoreBase (size_t offset, size_t length) {
        this->base.
    }
    virtual ~XdmaCoreBase () {}

    uint32_t * getUInt32Ptr (uint32_t offset) {
    }
};

class XdmaProxy
{
};
