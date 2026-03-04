#pragma once

#include "Common/Common.h"
#include "Actor/Actor.h"

#include <vector>
#include <memory>

namespace JD
{
    class JD_API IObjectPool
    {
    public:
        virtual ~IObjectPool() = default;
        virtual void Release(std::unique_ptr<Actor>&& obj) = 0;
    };

    template <typename T>
    class ObjectPool : public IObjectPool
    {
    public:
        virtual ~ObjectPool() = default;

    public:
        ObjectPool(const int size = 0)
        {
            pool.reserve(size);

            std::unique_ptr<T> obj;
            for (int i = 0; i < size; ++i)
            {
                obj = std::make_unique<T>();
                static_cast<Actor*>(obj.get())->SetOwnerPool(this);
                pool.emplace_back(std::move(obj));
            }
        }

    public:
        std::unique_ptr<T> Acquire()
        {
            std::unique_ptr<T> obj;
            if (!pool.empty())
            {
                obj = std::move(pool.back());
                pool.pop_back();
            }
            else
            {
                obj = std::make_unique<T>();
                static_cast<Actor*>(obj.get())->SetOwnerPool(this);
            }

            return obj;
        }

        virtual void Release(std::unique_ptr<Actor>&& obj) override
        {
            ReleaseTyped(std::unique_ptr<T>(static_cast<T*>(obj.release())));
        }

        void ReleaseTyped(std::unique_ptr<T>&& obj)
        {
            pool.emplace_back(std::move(obj));
        }

    private:
        std::vector<std::unique_ptr<T>> pool;
    };
}
