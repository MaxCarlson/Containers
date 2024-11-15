# Containers

A customizable, in-progress template container library that allows seamless swapping of underlying data structures for common container types. This project is designed to give developers control over performance and memory characteristics by selecting specific data structures at compile time.

## Features

- **Template-Based Design**: Enables specifying the underlying data structure for containers.
- **Versatility**: Supports multiple implementations of ordered and unordered containers.
- **Ease of Use**: Simple syntax for creating custom containers.

## Example Usage

```cpp
// Declare a map using a Red-Black Tree as the underlying data structure
Map<std::string, int, RedBlackTree>

// Declare a set using a Binary Tree
Set<int, BinaryTree>

// Declare a map with the default underlying structure
Map<int, int>
```

## Current Implementations

### Ordered-Set/Map Implementations
- **Red-Black Tree**
- **Binary Search Tree** (Note: Not fully compliant with template traits)

### Unordered-Set/Map Implementations
- **Open-Address Hash Table**
- **Robinhood Hashing (Open Address)**

## Getting Started

### Prerequisites
- **C++ Compiler**: Ensure you have a compiler that supports C++17 or later.
- **Visual Studio**: Recommended for working with the provided `.sln` file.
- **CMake (optional)**: For building without Visual Studio.

### Installation
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd Containers
   ```
2. Open `Containers.sln` in Visual Studio.

## Build Instructions

### Using Visual Studio
1. Open the `Containers.sln` solution file.
2. Select the build configuration (`Debug` or `Release`).
3. Build the solution by pressing `Ctrl+Shift+B`.

## Running Tests

### Unit Tests
Unit tests are provided to validate the implementation of various data structures. To run the tests:

1. Build the `UnitTests` project in Visual Studio.
2. Execute the resulting binary.

### Test Coverage
- `TestFlatTree.cpp`: Tests for the `FlatTree` implementation.
- `TestHashTables.cpp`: Tests for hash table implementations.
- `TestSmallVec.cpp`: Tests for the `SmallVec` utility.

## Contributing

Contributions are welcome! If you'd like to contribute:
1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Submit a pull request with a clear description of your changes.

## License
This project is open source. For details, please refer to the `LICENSE` file.

## Contact
For questions or suggestions, please open an issue in the repository.
