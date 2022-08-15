#ifndef XDMAREGISTER_H
#define XDMAREGISTER_H

#include <cstdint>

#include "xdma_memorybase.h"

class XDMARegisterImpl {
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
    XDMARegisterImpl(size_t offset) : regOffset(offset) {}
    virtual ~XDMARegisterImpl() {}

    void setOrigin(MemoryBase * orig) {
        this->origin = orig;
    }
};

template <typename T>
class XDMARegister : public XDMARegisterImpl
{
public:
    XDMARegister(size_t offset) : XDMARegisterImpl(offset) {}
    ~XDMARegister() {}

    void push(T value) {
        XDMARegisterImpl::push(value);
    }

    void pull(T & var) {
        XDMARegisterImpl::pull(var);
    }
};

// Template instantiation ======================================================
template<>
class XDMARegister<int32_t>;
template<>
class XDMARegister<uint32_t>;
// =============================================================================

// Type definitions ============================================================
typedef XDMARegisterImpl        xdmaReg_t;
typedef XDMARegister<int32_t>   xdmaInt32Reg;
typedef XDMARegister<uint32_t>  xdmaUInt32Reg;
// =============================================================================

#endif // XDMAREGISTER_H
