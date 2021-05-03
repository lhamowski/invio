#pragma once

#include "fwd.hpp"

#include "support/assert.hpp"

#include <cstdint>
#include <memory>
#include <stack>
#include <utility>

namespace invio::utils {

template <typename T>
class growable_memory_arena :
    public std::enable_shared_from_this<growable_memory_arena<T>>
{
private:
    class deleter
    {
    public:
        deleter(const std::shared_ptr<growable_memory_arena>& arena) :
            arena_{arena}
        {
        }

        void operator()(T* mem) const
        {
            if (auto arena = arena_.lock())
                arena->push(std::unique_ptr<T>(mem));
            else
                std::default_delete<T>{}(mem);
        }

    private:
        std::weak_ptr<growable_memory_arena> arena_;
    };

public:
    using item = std::unique_ptr<T, deleter>;

public:
    explicit growable_memory_arena(std::size_t init_size,
                                   std::size_t reserve_size) :
        reserve_size_{reserve_size}
    {
        for (std::size_t i{}; i < init_size; ++i)
            push(std::make_unique<T>());
    }

    growable_memory_arena(const growable_memory_arena&) = delete;
    growable_memory_arena& operator=(const growable_memory_arena&) = delete;

    growable_memory_arena(growable_memory_arena&&) = delete;
    growable_memory_arena& operator=(growable_memory_arena&&) = delete;

public:
    auto get(std::size_t size)
    {
        if (arena_.empty())
            push(std::make_unique<T>());
        item mem{arena_.top().release(), deleter{this->shared_from_this()}};
        arena_.pop();
        mem->resize(size);
        return mem;
    }

private:
    void push(std::unique_ptr<T> val)
    {
        val->reserve(reserve_size_);
        arena_.push(std::move(val));
    }

private:
    std::stack<std::unique_ptr<T>> arena_;
    std::size_t reserve_size_{};
};

}  // namespace invio::utils
