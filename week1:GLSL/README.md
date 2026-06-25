# Week 1 – Development Tooling

Topics covered:
- Git
- GDB
- Make
- CMake
- AddressSanitizer (ASan)
- UndefinedBehaviorSanitizer (UBSan)

---

## Resources Used

- Pro Git Book (https://git-scm.com/book/en/v2)
- GDB Documentation (https://sourceware.org/gdb/documentation/)
- GNU Make Manual (https://www.gnu.org/software/make/manual/make.html)
- CMake Documentation (https://cmake.org/documentation/)
- Modern CMake (https://cliutils.gitlab.io/modern-cmake/)

---

## Notes

### Git

Version control system used to track changes in source code.

Common commands:

```bash
git init
git add .
git commit -m "message"
git status
git log
```

Important takeaways:

- Commit early and often.
- Use meaningful commit messages.
- Create branches for new features.

### GDB

GNU Debugger used to inspect program execution.

Common commands:

```bash
break main
run
next
step
print variable
backtrace
```

Important takeaways:

- Debuggers are often faster than adding print statements.
- Breakpoints allow inspection of program state.

### Make

Build automation tool.

Example:

```make
app: main.o Entity.o
	g++ main.o Entity.o -o app
```

Important takeaways:

- Automates compilation.
- Rebuilds only modified files.
- Uses targets and dependencies.

### CMake

Cross-platform build system generator.

Example:

```cmake
cmake_minimum_required(VERSION 3.20)

project(EntityManager)

add_executable(
    EntityManager
    src/main.cpp
)
```

Important takeaways:

- Industry standard for C++ projects.
- Generates build files for different platforms.
- Works well with IDEs.

### AddressSanitizer (ASan)

Detects memory errors.

Example:

```cpp
int* p = new int[5];
p[10] = 42;
```

Compile:

```bash
g++ -fsanitize=address -g test.cpp
```

Important takeaways:

- Detects buffer overflows.
- Detects use-after-free errors.
- Provides detailed stack traces.

### UndefinedBehaviorSanitizer (UBSan)

Detects undefined behavior.

Example:

```cpp
int x = INT_MAX;
x += 1;
```

Compile:

```bash
g++ -fsanitize=undefined test.cpp
```

Important takeaways:

- Detects signed integer overflow.
- Detects invalid casts.
- Helps catch subtle runtime bugs.

---

## Mini Project: Tooling Integration

To reinforce all concepts learned this week, I upgraded the Week 0 Entity Manager project.

### Features Added

- Git repository with commit history
- Makefile support
- CMake support
- GDB debugging workflow
- AddressSanitizer checks
- UBSan checks

### Concepts Practiced

| Concept | How it was Used |
|----------|----------------|
| Git | Version control |
| GDB | Debugging runtime issues |
| Make | Build automation |
| CMake | Project configuration |
| ASan | Memory error detection |
| UBSan | Undefined behavior detection |

---

## Build Instructions


### Using CMake
Use this CMake file in Week0

```bash
mkdir build
cd build

cmake ..
cmake --build .

./EntityManager
```

---

## Completion Status

- [x] Git
- [x] GDB
- [x] Make
- [x] CMake
- [x] AddressSanitizer
- [x] UBSan
- [x] Tooling Integration

Week 1 completed.