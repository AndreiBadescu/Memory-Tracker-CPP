/*
Functions:
- MemoryTracker::Checkpoint()     : starts/resets the tracking from that that point of the code
- MemoryTracker::MessagesOn(bool) : turns on/off console messages when calling new or delete
- MemoryTracker::MemoryUsage()    : to show how much memory is currently dynamic allocated;
- MemoryTracker::Allocations()    : to show how many allocations and deallocations you've done;
- MemoryTracker::UnfreedAllocations() : to show how many allocations are still alive (unfreed);
- MemoryTracker::ShowAll() : calls all the above functions (in this order).
Option:
If you want to print a message with how much memory you allocate every time
go to line 25 and change "#define MESSAGES_ON false" to "#define MESSAGES_ON true".
Format of a printed message in console is: "<FILENAME> <LINE>: <Message>".
How is done:
I overloaded the new, new[], delete and delete[] operators.
You can do maximum 1,000 unfreed allocations before you exceed the limit
(if you want to increase / decrease this limit go to line 27 and change the number).
Others:
If you try to delete an array like a variable (basically, just deleting the 1st element)
you'll get an error message saying "YOU DELETED AN ARRAY LIKE A VARIABLE, USE []".
Also if you try to allocate 0 bytes you'll get a warning in console.
*/

#ifndef MEMORYTRACKER_H_INCLUDED_
#define MEMORYTRACKER_H_INCLUDED_

// change to true to show messages in console by default when you allocate/deallocate memory
#define MESSAGES_ON false
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
#pragma warning(push)
#pragma warning(disable : 28251)
#pragma warning(disable : 28251)

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdint>

#define MAX_FILENAME_LENGTH 256
#define MAX_MESSAGES_ON_STACK 30

static bool Messages = MESSAGES_ON;

class MemoryTracker {
public:
    static void MemoryUsage() {
        printf("Memory usage: %lld bytes\n", Get().memory);
    }

    static void Checkpoint() {
        Get().ResetAll();
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

    static void MessagesOn(bool ok) {
        Messages = ok;
    }

    static void AddMessage(const char* const str, const int32_t line) {
        assert(Get().stack_size < MAX_MESSAGES_ON_STACK);
        if (Get().stack_size >= MAX_MESSAGES_ON_STACK) { return; }
        Get().CopyStr(Get().messages[Get().stack_size].filename, str);
        Get().messages[Get().stack_size].line = line;
        ++Get().stack_size;
    }

    static void PrintMessage() noexcept {
        //assert(Get().stack_size > 0);
        if (Get().stack_size <= 0 || !Messages) { return; }
        --Get().stack_size;
        printf("%s %d: ", Get().messages[Get().stack_size].filename, 
                          Get().messages[Get().stack_size].line);
    }

    friend void Allocate(void* const adress, const std::size_t size, const bool isArray);
    friend void Deallocate(const void* const adress, const bool isArray);

private:
    int64_t memory;
    int32_t nr_of_allocations;
    int32_t nr_of_deallocations;
    struct {
        void* adress;
        size_t size;
        bool isArray;
    } blocks[LIMIT];

    int stack_size;
    struct {
        char filename[MAX_FILENAME_LENGTH] = {'\0'};
        int32_t line;
    } messages[MAX_MESSAGES_ON_STACK];

    inline void ResetAll() {
        memory = 0;
        nr_of_allocations = 0;
        nr_of_deallocations = 0;
        stack_size = 0;
    }

    MemoryTracker() noexcept {
        ResetAll();
    }

    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    ~MemoryTracker() = default;

    static MemoryTracker& Get() noexcept {
        static MemoryTracker tracker;
        //printf("%d\n", sizeof(tracker));
        return tracker;
    }

    void CopyStr(char* dest, const char* src) {
        for (uint32_t i = 0; i < MAX_FILENAME_LENGTH && (*dest++ = *src++); ++i) {}
    }
};

static void Allocate(void* const adress, const std::size_t size, const bool isArray) {
    MemoryTracker* ptr = &MemoryTracker::Get();
    const int unfreed_allocs = ptr->nr_of_allocations - ptr->nr_of_deallocations;
    assert(unfreed_allocs < LIMIT);
    if (unfreed_allocs >= LIMIT) {
        printf("<WARNING> YOU EXCEEDED THE LIMIT OF %d MAXIMUM UNFREED ALLOCATIONS\n", LIMIT);
        return;
    }
    if (Messages) { 
        if (isArray) {
            printf("Allocating %zu bytes (array)\n", size); 
        } else {
            printf("Allocating %zu bytes\n", size);
        }
    }
    if (size == 0) { printf("<WARNING> YOU ALLOCATED 0 BYTES !\n"); }
    ptr->blocks[unfreed_allocs].adress = adress;
    ptr->blocks[unfreed_allocs].size = size;
    ptr->blocks[unfreed_allocs].isArray = isArray;
    ptr->memory += size;
    ++ptr->nr_of_allocations;
}

static void Deallocate(const void* const adress, const bool isArray) {
    MemoryTracker* ptr = &MemoryTracker::Get();
    const int unfreed_allocs = ptr->nr_of_allocations - ptr->nr_of_deallocations;
    for (int32_t i = 0; i < unfreed_allocs; ++i) {
        if (adress == ptr->blocks[i].adress) {
            if (isArray != ptr->blocks[i].isArray) {
                printf("<WARNING> YOU DELETED AN ARRAY LIKE A VARIABLE, USE [] !!!\n");
                break;
            }

            if (Messages) {
                if (isArray) {
                    printf("Freeing %zu bytes (array)\n", ptr->blocks[i].size);
                } else {
                    printf("Freeing %zu bytes (array)\n", ptr->blocks[i].size);
                }
            }

            ptr->memory -= ptr->blocks[i].size;
            ++ptr->nr_of_deallocations;

            memmove(ptr->blocks + i,
                    ptr->blocks + i + 1,
                    (ptr->nr_of_allocations - i - 1) * sizeof(ptr->blocks[0]));
            break;
        }
    }
}

void* operator new(const std::size_t size) {
    void* ptr = malloc(size);
    assert(ptr && "<ERROR> Couldn't allocate memory!");
    Allocate(ptr, size, false);
    return ptr;
}

void* operator new[](const std::size_t size) {
    void* ptr = malloc(size);
    assert(ptr && "<ERROR> Couldn't allocate memory!");
    Allocate(ptr, size, true);
    return ptr;
}

void operator delete(void* const adress) {
    MemoryTracker::PrintMessage();
    if (adress == NULL) {
        printf("<WARNING> Freeing 0 bytes (NULL adress) !\n");
        return;
    }
    Deallocate(adress, false);
    free(adress);
}

void operator delete[](void* const adress) {
    MemoryTracker::PrintMessage();
    if (adress == NULL) {
        printf("<WARNING> Freeing 0 bytes (NULL adress) !\n");
        return;
    }
    Deallocate(adress, true);
    free(adress);
}

void* operator new(const std::size_t size, const char* const file, const int line) {
    if (Messages) { printf("%s %d: ", file, line); }
    return ::operator new(size);
}

void* operator new[](const std::size_t size, const char* const file, const int line) {
    if (Messages) { printf("%s %d: ", file, line); }
    return ::operator new[](size);
}

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define new new(__FILENAME__, __LINE__)
#define delete MemoryTracker::AddMessage(__FILENAME__, __LINE__), delete

#pragma warning(pop)
#endif // MEMORYTRACKER_H_INCLUDED_
