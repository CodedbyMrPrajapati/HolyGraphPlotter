# Week 0 – C++ Refresher

Topics covered:
- References
- RAII (Resource Acquisition Is Initialization)
- Smart Pointers
- Function Overloading
- Operator Overloading
- Templates (Introduction)

---

## Resources Used

- LearnCpp [https://www.learncpp.com]
- cppreference[https://en.cppreference.com]
- The Cherno[https://www.youtube.com/@TheCherno]
- C++ Weekly[https://www.youtube.com/@cppweekly]

## Notes
### References

References act as aliases to existing variables.

Example:

```cpp
int x = 10;
int& ref = x;
```

Important takeaways:

- References cannot be null.
- Use `const T&` when passing large objects.
- Helps avoid unnecessary copying.

### RAII

RAII stands for Resource Acquisition Is Initialization.

Resources are tied to object lifetime.

Example:

```cpp
std::ofstream file("data.txt");
```

The file automatically closes when the object goes out of scope.

Important takeaway:

Instead of manually cleaning resources, let destructors handle cleanup automatically.

### Smart Pointers

#### unique_ptr

```cpp
auto player = std::make_unique<Player>();
```

- Single owner
- Automatic cleanup
- Preferred ownership model

#### shared_ptr

```cpp
auto p1 = std::make_shared<Player>();
auto p2 = p1;
```

- Multiple owners
- Uses reference counting

#### weak_ptr

```cpp
std::weak_ptr<Player> observer;
```

- Non-owning reference
- Prevents circular ownership

### Function Overloading

Multiple functions can have the same name if their parameter lists differ.

```cpp
void print(int x);
void print(double x);
```

### Operator Overloading

Allows custom types to behave like built-in types.

```cpp
Vec2 c = a + b;
```

Implemented using:

```cpp
Vec2 operator+(const Vec2& other) const;
```

### Templates

Templates allow generic code.

```cpp
template<typename T>
T max(T a, T b)
{
    return a > b ? a : b;
}
```


## Mini Project: Entity Manager

To reinforce all concepts learned this week, I implemented a simple Entity Manager.

### Features

- Add entities
- Remove entities
- Search entities by ID
- Search entities by name
- Print entities
- Sort entities

### Concepts Practiced

| Concept | How it was Used |
|----------|----------------|
| References | Function parameters |
| RAII | Automatic destruction |
| Smart Pointers | `std::unique_ptr<Entity>` |
| Function Overloading | `find(int)` and `find(std::string)` |
| Operator Overloading | `operator<<` |
| Templates | Generic utility functions |
| STL | `std::vector`, `std::algorithm` |
| Lambdas | Sorting and filtering |

---




## Build Instructions

```bash
g++ -std=c++20 -Wall -Wextra -pedantic src/Entity_Manager.cpp -o entity_manager
```

Run:

```bash
./entity_manager
```

---

## Completion Status

- [x] References
- [x] RAII
- [x] Smart Pointers
- [x] Function Overloading
- [x] Operator Overloading
- [x] Templates
- [x] Entity Manager

Week 0 completed.