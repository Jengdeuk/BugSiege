#pragma once

#include "Common.h"

#include <cstdint>

namespace JD
{
    class JD_API RTTI
    {
    public:
        virtual ~RTTI() = default;
        virtual std::uintptr_t GetType() const = 0;

        virtual bool Is(std::uintptr_t id) const { return false; }
        virtual bool Is(const RTTI* rtti) const { return rtti ? Is(rtti->GetType()) : false; }

        template<typename T>
        T* As() {
            return Is(T::TypeIdClass()) ? static_cast<T*>(this) : nullptr;
        }

        template<typename T>
        const T* As() const {
            return Is(T::TypeIdClass()) ? static_cast<const T*>(this) : nullptr;
        }
    };
}

#define RTTI_DECLARATIONS(Type, ParentType)                                             \
protected:                                                                              \
    static std::uintptr_t TypeIdClass() {                                               \
        static char unique;                                                             \
        return reinterpret_cast<std::uintptr_t>(&unique);                               \
    }                                                                                   \
public:                                                                                 \
    using Super = ParentType;                                                           \
    virtual std::uintptr_t GetType() const override { return Type::TypeIdClass(); }     \
    virtual bool Is(std::uintptr_t id) const override {                                 \
        return (id == TypeIdClass()) ? true : ParentType::Is(id);                       \
    }                                                                                   \
    virtual bool Is(const RTTI* rtti) const override {                                  \
        return rtti ? Is(rtti->GetType()) : false;                                      \
    }
