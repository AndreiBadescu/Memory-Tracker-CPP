/*
Functions:
- MemoryTracker::MemoryUsage() : to show how much memory is currently dynamic allocated;
- MemoryTracker::Allocations() : to show how many allocations and deallocations you've done;
- MemoryTracker::UnfreedAllocations() : to show how many allocations are still alive (unfreed);
- MemoryTracker::ShowAll() : calls all the above functions (in this order).
Option:
If you want to print a message with how much memory you allocate every time
go to line 25 and change "#define MESSAGES_ON false" to "#define MESSAGES_ON true".
Format of a printed message in console is: "<FILENAME> <LINE>: <Message>".
How is done:
I overloaded the new, new[], delete and delete[] operators.
You can do maximum 1,000 unfreed allocations before you exceed the limit
(if you want to increase / decrease this limit go to line 4 and change the number).
Others:
If you try to delete an array like a variable (basically, just deleting the 1st element)
you'll get an error message saying "YOU DELETED AN ARRAY LIKE A VARIABLE, USE []".
Also if you try to allocate 0 bytes you'll get a warning in console.
*/

#ifndef MEMORYTRACKER_H_INCLUDED_
#define MEMORYTRACKER_H_INCLUDED_

// change to true to show messages in console when you allocate/deallocate memory
#define MESSAGES_ON true
// change this limits if you do more than 1,000 allocations
#define LIMIT 1000

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
*d
*/

/* HERE BEGINS THE CODE (DON'T CHANGE IT IF YOU DON'T KNOW WHAT YOU DOING)*/

/* This dissables 2 warnings related to this file in MSVC */
/* but you also need to uncomment line 214 "//#pragma warning(pop)" */
//#pragma warning(push)
//#pragma warning(disable : 28251)

#include <cassert>
#include <cstring>
#include <cstdlib>

#define MAX_FILENAME_LENGTH 256
#define MAX_MESSAGES_ON_STACK 30

class MemoryTracker {
public:
    static MemoryTracker& Get() noexcept {
        static MemoryTracker tracker;
        //printf("%d\n", sizeof(tracker));
        return tracker;
    }

    static void Allocate(void* const adress, const std::size_t size, const bool isArray) {
        if (Get().print_messages) printf("Allocating %zu bytes\n", size);
        if (size == 0) printf("!!!WARNING!!!: YOU ALLOCATED 0 BYTES\n");
        Get().blocks[Get().nr_of_allocations - Get().nr_of_deallocations].adress = adress;
        Get().blocks[Get().nr_of_allocations - Get().nr_of_deallocations].size = size;
        Get().blocks[Get().nr_of_allocations - Get().nr_of_deallocations].isArray = isArray;
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

                if (Get().print_messages) printf("Freeing %zu bytes\n", Get().blocks[i].size);

                Get().memory -= Get().blocks[i].size;
                ++Get().nr_of_deallocations;

                memmove(Get().blocks + i,
                        Get().blocks + i + 1,
                        (Get().nr_of_allocations - i - 1) * sizeof(memory_block));

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

#if MESSAGES_ON
    void AddMessage(const char* const str, const int32_t line) {
        assert(stack_size < MAX_MESSAGES_ON_STACK);
        if (stack_size >= MAX_MESSAGES_ON_STACK) { return; }
        strcpy(messages[stack_size].filename, str);
        messages[stack_size].line = line;
        ++stack_size;
    }

    void PrintMessage() {
        assert(stack_size > 0);
        if (stack_size <= 0) { return; }
        --stack_size;
        printf("%s %d: ", messages[stack_size].filename, messages[stack_size].line);
    }
#endif

private:
    struct memory_block {
        void* adress;
        size_t size;
        bool isArray;
    };

    const bool print_messages = MESSAGES_ON;
    int64_t memory;
    int32_t nr_of_allocations;
    int32_t nr_of_deallocations;
    memory_block blocks[LIMIT];

    int stack_size;
    struct {
        char filename[MAX_FILENAME_LENGTH] = {'\0'};
        int32_t line;
    } messages[MAX_MESSAGES_ON_STACK];

    MemoryTracker() noexcept {
        memory = 0;
        nr_of_allocations = 0;
        nr_of_deallocations = 0;
        stack_size = 0;
    }
};

void* operator new(const std::size_t size) {
    void* ptr = malloc(size);
    assert(ptr);
    MemoryTracker::Allocate(ptr, size, false);
    return ptr;
}

void* operator new[](const std::size_t size) {
    void* ptr = malloc(size);
    assert(ptr);
    MemoryTracker::Allocate(ptr, size, true);
    return ptr;
}

void operator delete(void* const adress) {
#if MESSAGES_ON
    MemoryTracker::Get().PrintMessage();
    if (adress == NULL) {
        printf("Freeing 0 bytes (NULL adress)\n");
        return;
    }
#endif
    MemoryTracker::Deallocate(adress, false);
    free(adress);
}

void operator delete[](void* const adress) {
#if MESSAGES_ON
    MemoryTracker::Get().PrintMessage();
    if (adress == NULL) {
        printf("Freeing 0 bytes (NULL adress)\n");
        return;
    }
#endif
    MemoryTracker::Deallocate(adress, true);
    free(adress);
}

void* operator new(const std::size_t size, const char* const file, const int line) {
    printf("%s %d: ", file, line);
    return ::operator new(size);
}

void* operator new[](const std::size_t size, const char* const file, const int line) {
    printf("%s %d: ", file, line);
    return ::operator new[](size);
}

#if MESSAGES_ON
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define new new(__FILENAME__, __LINE__)
#define delete MemoryTracker::Get().AddMessage(__FILENAME__, __LINE__), delete
#endif

//#pragma warning(pop)
#endif // MEMORYTRACKER_H_INCLUDED_
