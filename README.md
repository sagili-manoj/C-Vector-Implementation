# Custom C++ Vector (Dynamic Array)

A robust and efficient custom implementation of a dynamic array (similar to `std::vector` in the C++ Standard Library), designed from scratch using modern C++ principles. This project demonstrates deep understanding of memory management, allocator concepts, exception safety, move semantics, and iterator design.

## Features

* **Custom Allocator Support (`SimpleAllocator`):** Integrates with a custom-compliant allocator for flexible memory management, adhering to `std::allocator_traits`.
* **Dynamic Resizing:** Automatically grows its capacity when elements are added (`push_back`, `emplace_back`, `resize`).
* **Exception Safety:** Implements strong exception guarantees for operations like `reserve_more` to prevent memory leaks and ensure data integrity in case of exceptions during element construction.
* **Move Semantics:** Efficiently handles element movement during reallocations and construction using `std::move_if_noexcept` for performance and safety.
* **Rich Constructor Set:**
    * Default constructor
    * Size-based constructors (default-initialized or value-initialized)
    * **Optimized Iterator-Range Construction:** Utilizes C++11 tag dispatching (`std::input_iterator_tag`, `std::random_access_iterator_tag`) for efficient construction from various iterator types, including `std::uninitialized_copy` for random-access iterators.
    * Copy and Move Constructors.
    * `std::initializer_list` constructor.
* **Standard Container Interface:** Provides methods and operators consistent with `std::vector`, including:
    * Element access: `operator[]`, `at()`, `front()`, `back()`, `data()`
    * Capacity: `size()`, `capacity()`, `empty()`, `max_size()`, `reserve()`, `shrink_to_fit()`
    * Modifiers: `push_back()`, `emplace_back()`, `pop_back()`, `resize()`, `clear()`
    * Iterators: `begin()`, `end()`, `cbegin()`, `cend()` (both `Iterator` and `ConstIterator` with proper traits)
* **Comparison Operators:** Supports `==` and `!=` for comparing `Vector` instances.
* **Non-Member `swap`:** Provides a non-member `swap` function with allocator propagation awareness.

## Project Structure

* `customVector.hpp`: Contains the full definition of the `SimpleAllocator` and `Vector` classes, including all member functions and nested iterator types.
* `main.cpp`: A sample application that demonstrates how to use the `Vector` class and tests various functionalities.

## Technologies Used

* **C++17 (or newer):** Leverages modern C++ features like `if constexpr`.
* **Standard Library Components:** `std::allocator_traits`, `std::uninitialized_copy`, `std::move_if_noexcept`, `std::distance`, `std::iterator_traits`, `std::true_type`.

## How to Build and Run

This project can be compiled using any C++17 (or newer) compatible compiler (like GCC or Clang).

1.  **Save the files:** Ensure `customVector.hpp` and `main.cpp` are in the same directory.
2.  **Compile:**
    ```bash
    g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o vector_test
    ```
    * `-std=c++17`: Specifies the C++17 standard.
    * `-Wall -Wextra -pedantic`: Enable common warnings and strict adherence to the standard.
    * `main.cpp`: The source file containing the `main` function. It will automatically include `customVector.hpp`.
    * `-o vector_test`: Names the output executable `vector_test`.
3.  **Run:**
    ```bash
    ./vector_test
    ```

## Usage Examples

The `main.cpp` file provides a comprehensive example of how to use the `Vector` class. Here are some snippets from it:

```cpp
#include "customVector.hpp" // Include your custom Vector header
#include <iostream>
#include <vector> // Used here for std::vector iterators in an example

int main() {
    // Example 1: Basic usage
    Vector<int> v;
    v.push_back(1);
    v.emplace_back(2);
    v.push_back(3);
    std::cout << "Size: " << v.size() << ", Capacity: " << v.capacity() << "\n";
    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    // Example 2: From iterator range (random access)
    std::vector<int> src = {10, 20, 30, 40}; // Using std::vector iterators for demonstration
    Vector<int> v2(src.begin(), src.end());
    std::cout << "Vector from iterator range: ";
    for (const auto& x : v2) {
        std::cout << x << " ";
    }
    std::cout << "\nSize: " << v2.size() << ", Capacity: " << v2.capacity() << "\n";

    // Example 3: Resize and shrink_to_fit
    v.resize(5, 42);
    std::cout << "After resize(5, 42): ";
    for (const auto& x : v) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    v.shrink_to_fit();
    std::cout << "After shrink_to_fit: Size: " << v.size() << ", Capacity: " << v.capacity() << "\n";

    return 0;
}
