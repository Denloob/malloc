#ifndef DEBUG
#define DEBUG
#endif
#include "LinkedListAllocator.hpp"
#include <iostream>
#include <vector>

#if 0
#define USE_LLA
#endif

int main(int argc, char *argv[])
{
    using Alloc = LinkedListAllocator<>;

    // Allocate 3 numbers, assign, and print their value
    auto *b = (int *)Alloc::allocate(sizeof(int));
    auto *b2 = (int *)Alloc::allocate(sizeof(int));
    auto *b3 = (int *)Alloc::allocate(sizeof(int));
    *b = 3;
    *b2 = 4;
    *b3 = 5;
    std::cout << "*b = " << *b << " ; *b2 = " << *b2 << " ; *b3 = " << *b3 << '\n';
    // After we dealloc, the memory will be fragmented, but the allocator should
    //  deal with it. Let's see where the first int is located
    std::cout << "b addr = " << (void *)b << '\n';
    Alloc::deallocate(b3);
    Alloc::deallocate(b);
    Alloc::deallocate(b2);

    // Now let's allocate a big buffer. If the fragmentation is not fixed,
    //  it's address will be after the 3 ints. If not, it will be similar to
    //  b's addr.
    auto *b_new = (int *)Alloc::allocate(sizeof(int) * 10);
    std::cout << "b_new addr = " << (void *)b_new << '\n';
    Alloc::deallocate(b_new);

    {
        // I could not get the allocator traits to work on VisualStudio,
        //  but they did work with pedantic c++20 g++
        //  If you have a linux VM, partition, or subsystem, give it a try :D

        #ifdef USE_LLA
        // Now let's use the LLA, LinkedListAllocator wrapper that implements
        //  std::allocator_traits.
        std::vector<int, LLA<int>> v{1, 2, 3, 4, 5};
        // And also a normal vector which will use your default allocator.
        std::vector<int> v2(1);

        std::cout << "v (Custom Allocator) buffer addr = " << (void *)v.data()
                  << '\n';
        std::cout << "v2 (Default C++ allocator) buffer addr = "
                  << (void *)v2.data() << '\n';

        std::cout << "v.at(2) = " << v.at(2) << '\n';
        #endif

        {
            #ifdef USE_LLA
            // Let's allocate a large vec, and then free it.
            std::vector<int, LLA<int>> large_vec(200);
            #else
            auto large_mem = Alloc::allocate(sizeof(int) * 200);
            #endif

            // Let's check the address of a new int we alloc
            auto i = (int *)Alloc::allocate(sizeof(int));
            std::cout << "i addr (large string allocated) = " << (void *)i
                      << '\n';
            Alloc::deallocate(i);

            #ifndef USE_LLA
            Alloc::deallocate(large_mem);
            #endif
        }

        // Now when the large string was freed, what will be the address of an int?
        auto i = (int *)Alloc::allocate(sizeof(int));
        std::cout << "i addr (no large string) = " << (void *)i << '\n';
        Alloc::deallocate(i);
    }
#ifdef DEBUG
    // And right before exiting, let's  check if all the memory was deallocated
    std::cout << "Memory leaks (before exit) ?: " << std::boolalpha
              << Alloc::check_memory_leaks() << '\n';
    // What if we don't dealloc everything?
    (void)Alloc::allocate(sizeof(int) * 10);
    std::cout << "Memory leaks (before exit with non deallocated memory) ?: "
              << std::boolalpha << Alloc::check_memory_leaks() << '\n';
#endif
    return 0;
}
