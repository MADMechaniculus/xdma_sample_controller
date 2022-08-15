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

#include <sys/stat.h>
#include <fcntl.h>
#include <syscall.h>
#include <unistd.h>

#include <iostream>
#include <cstdint>
#include <cstdio>
#include <regex>
#include <fstream>
#include <unordered_map>

#include "xdma_memorybase.h"
#include "xdma_register.h"

class XdmaIdent {
public:
    // IdentBody section =======================================================
    // Функциональность данного класса не реализована, уточнить формат данных
    // у разработчика конфигурации ПЛИС.
    // =========================================================================
    class IdentBody {
    public:
        typedef struct {
            uint32_t serialNumber;
            uint32_t firmwareVersion;
            uint32_t fpgaModel;
        } identData_t;
    protected:
        identData_t __data;
    public:
        IdentBody() {}
        IdentBody(identData_t d) : __data(d) {}
        ~IdentBody() {}

        identData_t * data(void) { return &this->__data; }
        std::string stringify(void) {
            std::string ret = "[THIS FUNCTIONAL IS NOT IMPLEMENTED]";
            return ret;
        }
        bool parse(char * data, size_t len) {
            if (len != sizeof (identData_t))
                return false;

            // Functional is not implemented.
            // Check ident data format from device.

            return true;
        }
    };
    // =========================================================================

protected:
    /**
     * @brief IDENT_OFFSET
     * @warning Необходима конкретизация смещения базового адреса!
     */
    static constexpr size_t IDENT_OFFSET = 0x0;

    /**
     * @brief IDENT_LENGTH
     * @warning Необходима конкретизация размерности идентификационного ядра!
     */
    static constexpr size_t IDENT_LENGTH = 0x0;

    char * identBase{nullptr};

    IdentBody idn;

public:
    XdmaIdent(int fd) {
        if (fd < 0) {
            throw std::runtime_error("Target file descriptor isinvalid!");
        }

        identBase = (char*)mmap(NULL, IDENT_LENGTH, PROT_READ, \
                                MAP_SHARED, fd, IDENT_OFFSET);
        if (identBase == nullptr) {
            throw std::runtime_error("Memory mapping operation has faulted!");
        }

        if (!idn.parse(identBase, sizeof (IdentBody::identData_t))) {
            throw std::runtime_error("Mapped memory not contains target information!");
        }
    }
};

class XdmaProxy
{
protected:

    std::unordered_map<std::string, int> fileDescriptorsMap;


public:
    explicit XdmaProxy() noexcept {}

    ~XdmaProxy() {
        deinit();
    }

    /**
     * @brief Проверка наличия модуля драйвера в ядре
     * @return true - Загружен
     * @return false - Не загружен
     */
    bool isDrvLoaded(void) {
        // Ничего лучше не придумал
        int retCode = std::system("lsmod | grep xdma >> /dev/null");
        return !retCode;
    }

    /**
     * @brief Проверка существования устройства с указанным ID в дереве PCI устройств
     * @param deviceAddress Адрес устройства [^[hex][hex]\:[hex][hex]\.[hex]$]
     * @return true - Существует
     * @return false - Ошибка или отсутствует
     */
    bool isDeviceExists(std::string deviceAddress) {
        if (deviceAddress.empty())
            return false;

        bool retCode = false;

        std::regex deviceId("(^[a-fA-F0-9][a-fA-F0-9]\:[a-fA-F0-9][a-fA-F0-9]\.[a-fA-F0-9]$)", \
                            std::regex_constants::ECMAScript | std::regex_constants::icase);

        if (std::regex_search(deviceAddress, deviceId)) {
            retCode = (std::system(("lspci | grep " + deviceAddress + " >> /dev/null").c_str()) == 0);
        }

        return retCode;
    }

    /**
     * @brief Загрузка модуля драйвера в ядро
     * @param modPath Путь к модулю
     * @return true - Успешная загрузка модуля
     * @return false - ошибка загрузки модуля
     */
    bool injectModule(std::string modPath) {
        std::vector<char> binaryBuffer;
        int ret = -1;

        struct stat moduleFileInfo;
        if (stat(modPath.c_str(), &moduleFileInfo) == 0) {
            binaryBuffer.resize(moduleFileInfo.st_size);
            std::ifstream moduleStream(modPath, std::ios::binary);
            if (moduleStream.is_open()) {
                moduleStream.read((char*)binaryBuffer.data(), moduleFileInfo.st_size);
                moduleStream.close();
                ret = syscall(__NR_init_module, binaryBuffer.data(), binaryBuffer.size(), "");
            } else {
                return false;
            }
        }

        return (ret == 0 ? true : false);
    }

    /**
     * @brief Открытие файлового дескриптора устройства
     * @param targetDevice Путь к целевому устройству
     * @return 0 - успешное открытие файлового дескриптора
     * @return -1 - файловый дескриптор не может быть открыт
     * @return -2 - файловый десприктор уже в коллекции открытых дескрипторов
     */
    int initWithTarget(std::string targetDevice = "/dev/xdma0_user") {

        if (this->fileDescriptorsMap.count(targetDevice) != 0) {
            return -2;  // Descriptor of the device already opened
        }

        struct stat statBuffer;
        if (stat(targetDevice.c_str(), &statBuffer) != 0) {
            return -1;  // Descriptor of the device can`t be opened
        }

        int tmpFd = -1;
        tmpFd = open(targetDevice.c_str(), O_RDWR | O_NONBLOCK);
        if (tmpFd == -1) {
            return -1;  // Descriptor of the device can`t be opened
        }

        this->fileDescriptorsMap.emplace(targetDevice, tmpFd);

        return 0;
    }

    void deinit(void) {
        if (!this->fileDescriptorsMap.empty()) {
            for (auto & kv : this->fileDescriptorsMap) {
                close(kv.second);
            }

            this->fileDescriptorsMap.clear();
        }
    }

    /**
     * @brief Получение файлового дескриптора открытого устройства
     * @param targetDevice Путь к файлу целевого устройства
     * @return fd - Если целевое устройство содержится в коллекции открытых устройств
     * @return -1 - Если целевое устройство отсутствует
     */
    int getFdByTarget(std::string targetDevice = "/dev/xdma0_user") {
        if (this->fileDescriptorsMap.count(targetDevice) != 0)
            return this->fileDescriptorsMap.at(targetDevice);
        return -1;
    }
};
