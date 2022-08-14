#ifndef XMDA_SAMPLE_CONTROLLER_H
#define XMDA_SAMPLE_CONTROLLER_H

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

#include "xdma_sample_controller_global.h"

// Utility definitions =========================================================
#define __R(type, base, offset) *((type *)(base + offset))
#define __U32R(base, offset) __R(uint32_t, base, offset)
#define __S32R(base, offset) __R(int32_t, base, offset)
#define __ROFT(base, reg, value) \
    static constexpr uint32_t base ## _ ## reg ## _OFFSET = value

// =============================================================================

// Definition section ==========================================================
#define SAMPLE_CORE_1_REG_0_IS_THREADSAFE 0
#define SAMPLE_CORE_1_REG_1_IS_THREADSAFE 1
#define SAMPLE_CORE_2_REG_0_IS_THREADSAFE 0
#define SAMPLE_CORE_2_REG_1_IS_THREADSAFE 1
#define SAMPLE_CORE_3_REG_0_IS_THREADSAFE 1
#define SAMPLE_CORE_3_REG_1_IS_THREADSAFE 0
#define SAMPLE_CORE_3_REG_2_IS_THREADSAFE 1
#define SAMPLE_CORE_3_REG_3_IS_THREADSAFE 1

// =============================================================================

class XDMA_SAMPLE_CONTROLLER_EXPORT Xdma_sample_controller
{
public:
    __ROFT(SAMPLE_CORE_1, REG_0, 0x0);
    __ROFT(SAMPLE_CORE_1, REG_1, 0x4);
    __ROFT(SAMPLE_CORE_2, REG_0, 0x0);
    __ROFT(SAMPLE_CORE_2, REG_1, 0x4);
    __ROFT(SAMPLE_CORE_3, REG_0, 0x0);
    __ROFT(SAMPLE_CORE_3, REG_1, 0x4);
    __ROFT(SAMPLE_CORE_3, REG_2, 0x8);
    __ROFT(SAMPLE_CORE_3, REG_3, 0xc);

private:
    std::string openedDevice;

    // Base offsets region =====================================================
    char * sampleCoreBase_1;
    char * sampleCoreBase_2;
    char * sampleCoreBase_3;
    // =========================================================================

    // Threadsafe section ======================================================
#if SAMPLE_CORE_1_REG_0_IS_THREADSAFE == 1
    std::mutex sampleCore1Reg0Mx;
#endif
#if SAMPLE_CORE_1_REG_1_IS_THREADSAFE == 1
    std::mutex sampleCore1Reg1Mx;
#endif
#if SAMPLE_CORE_2_REG_0_IS_THREADSAFE == 1
    std::mutex sampleCore2Reg0Mx;
#endif
#if SAMPLE_CORE_2_REG_1_IS_THREADSAFE == 1
    std::mutex sampleCore2Reg1Mx;
#endif
#if SAMPLE_CORE_3_REG_0_IS_THREADSAFE == 1
    std::mutex sampleCore3Reg0Mx;
#endif
#if SAMPLE_CORE_3_REG_1_IS_THREADSAFE == 1
    std::mutex sampleCore3Reg1Mx;
#endif
#if SAMPLE_CORE_3_REG_2_IS_THREADSAFE == 1
    std::mutex sampleCore3Reg2Mx;
#endif
#if SAMPLE_CORE_3_REG_3_IS_THREADSAFE == 1
    std::mutex sampleCore3Reg3Mx;
#endif
    // =========================================================================

    // Register operations =====================================================
    void setSampleCore1Reg0Value(uint32_t newValue) {
#if SAMPLE_CORE_1_REG_0_IS_THREADSAFE == 1
        std::lock_guard<std::mutex> lock(sampleCore1Reg0Mx);
#endif
        __U32R(sampleCoreBase_1, SAMPLE_CORE_1_REG_0_OFFSET) = newValue;
    }

    uint32_t getSampleCore1Reg0Value(void) {
        uint32_t tmp = 0;
#if SAMPLE_CORE_1_REG_0_IS_THREADSAFE == 1
        {
            std::lock_guard<std::mutex> lock(sampleCore1Reg0Mx);
            tmp = __U32R(sampleCoreBase_1, SAMPLE_CORE_1_REG_0_OFFSET);
        }
#else
        tmp = __U32R(sampleCoreBase_1, SAMPLE_CORE_1_REG_0_OFFSET);
#endif
        return tmp;
    }

    // =========================================================================

public:
    Xdma_sample_controller();
    ~Xdma_sample_controller() {}

    int initController(std::string devPath) {

        return 0;
    }
};

#endif // XMDA_SAMPLE_CONTROLLER_H
