# Containers

An in-progress template container library where it's simple and easy to change the underlying data structure of common container types.

Example syntax:

Map<std::string, int, RedBlackTree>

Set<int, BinaryTree>

Map<int, int>

## Current data structures

### Ordered-Set/Map implementations

Red-Black Tree

Binary Search Tree (not compliant with the template traits yet)

### Unordered-Set/Map implementations

Open-Address hash table.

Robinhood-Hashing Open Address hash table (Unfinished).

### Vector

Small Vector. 

- Based off of LLVM's [similar class](http://llvm.org/doxygen/classllvm_1_1SmallVector.html).

- Has [Visual Studio Natvis file](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects) for debugging.


### A work in progress.
