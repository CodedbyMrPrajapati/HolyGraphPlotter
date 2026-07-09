# Week 7 - Expression Tree Design & C++ DSL

The math engine behind **The Plotter**. This week designs the expression-tree node types and an
operator-overloaded C++ Domain Specific Language (DSL) so surfaces can be described as math
expressions. Week 8 reuses these exact nodes as the output of a runtime string parser.

Topics covered:
- Expression trees and the interpreter pattern
- Node types: `Expr`, `Constant`, `Variable`, `BinaryOp`, `FuncCall`
- Virtual dispatch (`eval`, `str`) over a polymorphic node hierarchy
- `std::shared_ptr` for shared subtree ownership
- Operator overloading to build an embedded DSL

---

## Resources Used

- Crafting Interpreters - Representing Code (https://craftinginterpreters.com/representing-code.html)
- cppreference - Operator overloading (https://en.cppreference.com/w/cpp/language/operators)
- cppreference - `std::shared_ptr` (https://en.cppreference.com/w/cpp/memory/shared_ptr)

---

## Notes

### The Node Hierarchy

Every node derives from an abstract `Expr` with two virtual methods:

```cpp
struct Expr {
    virtual double eval(const Env& env) const = 0;  // evaluate given variable bindings
    virtual std::string str() const = 0;            // pretty-print the subtree
    virtual ~Expr() = default;
};
```

`Env` is a `std::map<std::string, double>` of variable bindings (`x`, `y`, and later `t`).

Concrete nodes:

| Node | Meaning |
|------|---------|
| `Constant` | A literal number |
| `Variable` | A named variable looked up in the `Env` |
| `BinaryOp` | `+ - * / ^` over two child expressions |
| `FuncCall` | `sin`, `cos`, `tan`, `exp`, `log`, `sqrt`, `abs` of one argument |

Important takeaway:

`eval` walks the tree recursively — this is the classic interpreter pattern.

### The Operator-Overloaded DSL

An `Expression` handle wraps `std::shared_ptr<Expr>`. Overloaded operators and free functions build
the tree, so a surface is written as ordinary C++:

```cpp
Expression x("x"), y("y");
Expression z = sin(x) * cos(y);           // builds FuncCall/BinaryOp nodes
z.eval({{"x", 1.0}, {"y", 2.0}});         // -> -0.3502...
```

Implicit `Expression(double)` / `Expression(int)` constructors let literals mix with expressions
(`pow(x, 2.0) + 0.5 * sin(x + y)`).

Important takeaway:

Operator overloading turns tree construction into readable math, and `shared_ptr` lets subtrees be
shared without manual memory management.

---

## Mini Project: Build & Evaluate an Expression DSL

`main.cpp` builds `z = sin(x)cos(y)` with the DSL, prints the tree, and evaluates it against the
reference C++ computation to confirm it reproduces the Week 6 surface formula.

### Concepts Practiced

| Concept | How it was Used |
|----------|----------------|
| Polymorphism | Virtual `eval` / `str` per node |
| Interpreter Pattern | Recursive tree evaluation |
| Smart Pointers | `shared_ptr<Expr>` subtree ownership |
| Operator Overloading | `+ - * / ^` and `sin/cos/...` build nodes |
| Templates/Conversions | Implicit `Expression(double)` for literals |

---

## Build Instructions

### Build with Make

```bash
make
./expr_demo
```

### Build with CMake

```bash
mkdir -p build
cd build
cmake .. -G Ninja
cmake --build .
cd ..
./build/expr_demo
```

---

## Completion Status

- [x] `Expr` base + `Constant` / `Variable` / `BinaryOp` / `FuncCall`
- [x] Recursive `eval` (interpreter pattern)
- [x] `str()` pretty-printing
- [x] `shared_ptr` subtree ownership
- [x] Operator-overloaded `Expression` DSL
- [x] Reusable by the Week 8 parser

Week 7 completed.
