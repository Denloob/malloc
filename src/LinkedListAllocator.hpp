#pragma once

#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <stdexcept>

constexpr std::size_t MEGA_BYTE{2 << 9};
template <std::size_t PageSize = MEGA_BYTE> class LinkedListAllocator
{
  public:
    static void *allocate(std::size_t size)
    {
        if (size <= 0)
        {
            throw std::invalid_argument{"Invalid size"};
        }

        Header *header = find_free(size);
        if (!header)
        {
            throw std::bad_alloc{};
        }

        if (header->size() > size)
        {
            Header *prev_next = header->next;
            header->next = ptr_cast<Header>(ptr_cast<char>(header + 1) + size);

            if (static_cast<std::size_t>(prev_next - header->next) <=
                sizeof(Header))
            {
                header->next = prev_next;
            }
            else
            {
                header->next->free = true;
                header->next->next = prev_next;
            }
        }

        header->free = false;
        return header + 1;
    }

    static void deallocate(void *ptr)
    {
        if (!ptr)
        {
            return;
        }

        Header *header = static_cast<Header *>(ptr) - 1;
        assert(!header->free && "Trying to deallocate a free block.");
        header->free = true;
    }

  private:
    template <typename T> static constexpr T *ptr_cast(void *ptr)
    {
        return static_cast<T *>(ptr);
    }
    template <typename T> static constexpr const T *ptr_cast(const void *ptr)
    {
        return static_cast<const T *>(ptr);
    }

    struct Header
    {
        Header *next;
        bool free;
        [[nodiscard]] constexpr std::size_t size() const
        {
            const char *block_end =
                next ? ptr_cast<char>(next) : (page.data() + page.size());
            const char *block_start = ptr_cast<char>(this + 1);

            return block_end - block_start;
        }
    };

    static Header *find_free(std::size_t size)
    {
        auto *curr = ptr_cast<Header>(page.data());
        while (curr && (!curr->free || curr->size() < size))
        {
            curr = curr->next;
        }

        return curr;
    }

    static std::array<char, PageSize> page;
};

template <std::size_t PageSize>
std::array<char, PageSize> LinkedListAllocator<PageSize>::page = std::invoke(
    []() constexpr
    {
        Header header{
            .next = nullptr,
            .free = true,
        };
        auto *header_char_arr = ptr_cast<char>(&header);

        std::array<char, PageSize> page_ininitalizer{};
        std::copy(header_char_arr, header_char_arr + sizeof(Header),
                  page_ininitalizer.data());

        return page_ininitalizer;
    });
