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
#include "axi_register.h"

class XdmaProxy
{
protected:

    std::unordered_map<std::string, int> AXIDescriptorsMap;
    std::unordered_map<std::string, int> DMADescriptorsMap;

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

        std::regex deviceId("(^[a-fA-F0-9][a-fA-F0-9]\\:[a-fA-F0-9][a-fA-F0-9]\\.[a-fA-F0-9]$)", \
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
    int initAXIWithTarget(std::string targetDevice = "/dev/xdma0_user") {

        if (targetDevice.find("user") == std::string::npos)
            return -1;  // Target path is incompatible

        if (this->AXIDescriptorsMap.count(targetDevice) != 0) {
            return -2;  // Descriptor of the device already opened
        }

        struct stat statBuffer;
        if (stat(targetDevice.c_str(), &statBuffer) != 0)
            return -1;  // Descriptor of the device can`t be opened

        int tmpFd = -1;
        tmpFd = open(targetDevice.c_str(), O_RDWR | O_NONBLOCK);
        if (tmpFd == -1)
            return -1;  // Descriptor of the device can`t be opened

        this->AXIDescriptorsMap.emplace(targetDevice, tmpFd);

        return 0;
    }

    /**
     * @brief Открытие файлового дескриптора устройства поточного чтения
     * @param targetDevice Путь к целевому устройству
     * @return 0 - успешное открытие дескриптора
     * @return -1 - файловый дескриптор не может быть открыт
     * @return -2 - файловый дескриптор уже в коллекции открытых десприкторов
     */
    int initRDMAWithTarget(std::string targetDevice = "/dev/xdma0_c2h_0") {
        if (targetDevice.find("c2h_") == std::string::npos)
            return -1;  // Target path is incompatible

        if (this->DMADescriptorsMap.count(targetDevice) != 0)
            return -2;  // File already opened

        struct stat statBuffer;
        if (stat(targetDevice.c_str(), &statBuffer) != 0)
            return -1;

        int tmpFd = -1;
        tmpFd = open(targetDevice.c_str(), O_RDWR | O_NONBLOCK);
        if (tmpFd == -1)
            return -1;

        this->DMADescriptorsMap.emplace(targetDevice, tmpFd);
        return 0;
    }

    /**
     * @brief Закрытие файловых дескрипторов
     */
    void deinit(void) {
        if (!this->AXIDescriptorsMap.empty()) {
            for (auto & kv : this->AXIDescriptorsMap) {
                close(kv.second);
            }

            this->AXIDescriptorsMap.clear();
        }

        if (!this->DMADescriptorsMap.empty()) {
            for (auto & dd : this->DMADescriptorsMap) {
                close(dd.second);
            }

            this->DMADescriptorsMap.clear();
        }
    }

    /**
     * @brief Получение файлового дескриптора открытого устройства
     * @param targetDevice Путь к файлу целевого устройства
     * @return fd - Если целевое устройство содержится в коллекции открытых устройств
     * @return -1 - Если целевое устройство отсутствует
     */
    int getAXIFdByTarget(std::string targetDevice = "/dev/xdma0_user") {
        if (this->AXIDescriptorsMap.count(targetDevice) != 0)
            return this->AXIDescriptorsMap.at(targetDevice);
        return -1;
    }
};
