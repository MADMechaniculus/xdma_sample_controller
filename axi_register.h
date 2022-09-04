#ifndef XDMAREGISTER_H
#define XDMAREGISTER_H

#include <cstdint>

#include "xdma_memorybase.h"

class AXIRegisterImpl {
protected:
    MemoryBase * origin{nullptr};
    size_t regOffset;

    // uint32_t methods ========================================================
    void pull(uint32_t & var) {
        var = *((uint32_t*)(this->origin + this->regOffset));
    }

    void push(uint32_t value) {
        *((uint32_t*)(this->origin + this->regOffset)) = value;
    }
    // =========================================================================

    // int32_t methods =========================================================
    void pull(int32_t & var) {
        var = *((int32_t*)(this->origin + this->regOffset));
    }

    void push(int32_t value) {
        *((int32_t*)(this->origin + this->regOffset)) = value;
    }
    // =========================================================================

public:
    AXIRegisterImpl(size_t offset) : regOffset(offset) {}
    virtual ~AXIRegisterImpl() {}

    void setOrigin(MemoryBase * orig) {
        this->origin = orig;
    }
};

template <typename T>
class AXIRegister : public AXIRegisterImpl
{
public:
    AXIRegister(size_t offset) : AXIRegisterImpl(offset) {}
    ~AXIRegister() {}

    void push(T value) {
        AXIRegisterImpl::push(value);
    }

    void pull(T & var) {
        AXIRegisterImpl::pull(var);
    }
};

// Template instantiation ======================================================
template<>
class AXIRegister<int32_t>;
template<>
class AXIRegister<uint32_t>;
// =============================================================================

// Type definitions ============================================================
typedef AXIRegisterImpl        AXIReg_t;
typedef AXIRegister<int32_t>   AXIInt32Reg;
typedef AXIRegister<uint32_t>  AXIUInt32Reg;
// =============================================================================

#endif // XDMAREGISTER_H
