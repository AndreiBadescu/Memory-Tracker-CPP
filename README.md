# Memory-Handler-CPP
I've created a basic memory handler for c++ programs. This file is a header.


Functions:
- MemoryHandler::MemoryUsage() : to show how much memory is currently dynamic allocated
- MemoryHandler::Allocations() : to show how many allocations and deallocations you've done
- MemoryHandler::UnfreedAllocations() : to show how many allocations are still alive (unfreed)
- MemoryHandler::ShowAll() : calls all the above functions (in this order)


Option:
If you want to print a message with how much memory you allocate every time
go to line 5 and change "#define MESSAGES_ON false" to "#define MESSAGES_ON true"

Format of a printed message in console is: "FILENAME LINE: Message"


How is done:
I overloaded the new, new[], delete and delete[] operators.
You can do maximum 10,000 unfreed allocations before you exceed the limit
(if you want to increase / decrease this limit go to line 4 and change the number)


Others:
If you try to delete an array like a variable (basically, just deleting the 1st element)
you'll get an error message saying "YOU DELETED AN ARRAY LIKE A VARIABLE, USE []"
Also if you try to allocate 0 bytes you'll get a warning in console
