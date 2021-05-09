/*
    Functions:
    - MemoryHandler::MemoryUsage() : to show how much memory is currently dynamic allocated
    - MemoryHandler::Allocations() : to show how many allocations and deallocations you've done
    - MemoryHandler::UnfreedAllocations() : to show how many allocations are still alive (unfreed)
    - MemoryHandler::ShowAll() : calls all the above functions (in this order)

    Option:
    If you want to print a message with how much memory you allocate every time
    go to line 5 and change "#define MESSAGES_ON false" to "#define MESSAGES_ON true"
    Format of a printed message in console is: "<FILENAME> <LINE>: <Message>"

    How is done:
    I overloaded the new, new[], delete and delete[] operators.
    You can do maximum 10,000 unfreed allocations before you exceed the limit
    (if you want to increase / decrease this limit go to line 4 and change the number)

    Others:
    If you try to delete an array like a variable (basically, just deleting the 1st element)
    you'll get an error message saying "YOU DELETED AN ARRAY LIKE A VARIABLE, USE []"
    Also if you try to allocate 0 bytes you'll get a warning in console
*/

#ifndef MEMORYHANDLER_H_INCLUDED
#define MEMORYHANDLER_H_INCLUDED

// change to true to show messages in console when you allocate/deallocate memory
#define MESSAGES_ON true
// change this limits if you do more than 10,000 allocations
#define LIMIT 10000

/*
*
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
*/

/* HERE BEGINS THE CODE (DON'T CHANGE IT IF YOU DON'T KNOW WHAT YOU ARE DOING)*/

/* This dissables 2 warnings related to this file in MSVC */
/* but you also need to uncomment line 214 "//#pragma warning(pop)" */
//#pragma warning(push)
//#pragma warning(disable : 28251)

#include <iostream>
#include <cassert>
#include <cstring>

class MemoryHandler {
public:
    static MemoryHandler& Get() {
        static MemoryHandler handler;
        return handler;
    }

    static void Allocate(void* const adress, const std::size_t size, const bool isArray) {
        if (Get().printMessages) printf("Allocating %zu bytes\n", size);
        if (size == 0) printf("!!!WARNING: YOU ALLOCATED 0 BYTES\n");
        Get().blocks[Get().nr_of_allocations].adress = adress;
        Get().blocks[Get().nr_of_allocations].size = size;
        Get().blocks[Get().nr_of_allocations].isArray = isArray;
        Get().memory += size;
        ++Get().nr_of_allocations;
    }

    static void Deallocate(const void* const adress, const bool isArray) {
        for (int32_t i = 0; i < Get().nr_of_allocations - Get().nr_of_deallocations; ++i) {
            if (adress == Get().blocks[i].adress) {
                if (isArray != Get().blocks[i].isArray) {
                    printf("!!!ERROR: YOU DELETED AN ARRAY LIKE A VARIABLE, USE []\n");
                    break;
                }

                if (Get().printMessages) printf("Freeing %zu bytes\n", Get().blocks[i].size);

                Get().memory -= Get().blocks[i].size;
                ++Get().nr_of_deallocations;

                for (int32_t j = i + 1; j < Get().nr_of_allocations; ++j) {
                    Get().blocks[j - 1] = Get().blocks[j];
                }
                break;
            }
        }
    }

    static void MemoryUsage() {
        printf("Memory usage: %lld bytes\n", Get().memory);
    }

    static void Allocations() {
        printf("You allocated memory %d times\n", Get().nr_of_allocations);
        printf("You freed memory %d times\n", Get().nr_of_deallocations);
    }

    static void UnfreedAllocations() {
        printf("You have %d unfreed allocations\n", Get().nr_of_allocations - Get().nr_of_deallocations);
    }

    static void ShowAll() {
        printf("\n");
        MemoryUsage();
        Allocations();
        UnfreedAllocations();
        printf("\n");
    }

private:
    struct memory_block {
        void* adress;
        size_t size;
        bool isArray;
    };

    const bool printMessages = MESSAGES_ON;
    int64_t memory;
    int32_t nr_of_allocations;
    int32_t nr_of_deallocations;
    memory_block* blocks;

    MemoryHandler() {
        memory = 0;
        nr_of_allocations = 0;
        nr_of_deallocations = 0;
        blocks = (memory_block*) malloc(LIMIT * sizeof(memory_block));
        assert(blocks);
    }
};

void* operator new(const std::size_t size) {
    void* ptr = malloc(size);
    assert(ptr);
    MemoryHandler::Allocate(ptr, size, false);
    return ptr;
}

void* operator new[](const std::size_t size) {
    void* ptr = malloc(size);
    assert(ptr);
    MemoryHandler::Allocate(ptr, size, true);
    return ptr;
}

void operator delete(void* const adress) {
    assert(adress);
    MemoryHandler::Deallocate(adress, false);
    free(adress);
}

void operator delete[](void* const adress) {
    assert(adress);
    MemoryHandler::Deallocate(adress, true);
    free(adress);
}

void* operator new(const std::size_t size, const int line, const char* const file) {
    printf("%s %d: ", file, line);
    return ::operator new(size);
}

void* operator new[](const std::size_t size, const int line, const char* const file) {
    printf("%s %d: ", file, line);
    return ::operator new[](size);
}

#if MESSAGES_ON
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define new new(__LINE__, __FILENAME__)
#define delete printf("%s %d: ", __FILENAME__, __LINE__), delete
#endif

//#pragma warning(pop)
#endif // MEMORYHANDLER_H_INCLUDED
