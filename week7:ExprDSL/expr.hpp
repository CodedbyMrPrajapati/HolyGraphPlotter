#ifndef _EXPR_HPP_
#define _EXPR_HPP_

// Week 7 - Expression Tree Design & operator-overloaded C++ DSL.
//
// This header defines a small expression-tree library plus an `Expression`
// handle whose overloaded operators let you build math expressions directly in
// C++:
//
//     Expression x("x"), y("y");
//     Expression z = sin(x) * cos(y);
//     z.eval({{"x", 1.0}, {"y", 2.0}});
//
// Week 8 reuses these nodes as the output of its runtime string parser.

#include <map>
#include <string>
#include <memory>
#include <cmath>
#include <stdexcept>

namespace dsl {

// Variable bindings passed in at evaluation time (e.g. x, y, t).
using Env = std::map<std::string, double>;

// ---------------------------------------------------------------------------
// Expression tree node types
// ---------------------------------------------------------------------------

struct Expr {
    virtual double eval(const Env& env) const = 0;
    virtual std::string str() const = 0;
    virtual ~Expr() = default;
};

struct Constant : Expr {
    double value;
    explicit Constant(double v) : value(v) {}
    double eval(const Env&) const override { return value; }
    std::string str() const override {
        // Trim trailing zeros for readability.
        std::string s = std::to_string(value);
        auto dot = s.find('.');
        if (dot != std::string::npos) {
            size_t last = s.find_last_not_of('0');
            if (last == dot) last = dot - 1;
            s.erase(last + 1);
        }
        return s;
    }
};

struct Variable : Expr {
    std::string name;
    explicit Variable(std::string n) : name(std::move(n)) {}
    double eval(const Env& env) const override {
        auto it = env.find(name);
        if (it == env.end())
            throw std::runtime_error("Unbound variable: " + name);
        return it->second;
    }
    std::string str() const override { return name; }
};

struct BinaryOp : Expr {
    char op;   // one of + - * / ^
    std::shared_ptr<Expr> lhs, rhs;
    BinaryOp(char o, std::shared_ptr<Expr> l, std::shared_ptr<Expr> r)
        : op(o), lhs(std::move(l)), rhs(std::move(r)) {}
    double eval(const Env& env) const override {
        double a = lhs->eval(env);
        double b = rhs->eval(env);
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return a / b;
            case '^': return std::pow(a, b);
        }
        throw std::runtime_error(std::string("Unknown operator: ") + op);
    }
    std::string str() const override {
        return "(" + lhs->str() + " " + op + " " + rhs->str() + ")";
    }
};

struct FuncCall : Expr {
    std::string name;
    std::shared_ptr<Expr> arg;
    FuncCall(std::string n, std::shared_ptr<Expr> a)
        : name(std::move(n)), arg(std::move(a)) {}
    double eval(const Env& env) const override {
        double v = arg->eval(env);
        if (name == "sin")  return std::sin(v);
        if (name == "cos")  return std::cos(v);
        if (name == "tan")  return std::tan(v);
        if (name == "exp")  return std::exp(v);
        if (name == "log")  return std::log(v);
        if (name == "sqrt") return std::sqrt(v);
        if (name == "abs")  return std::fabs(v);
        throw std::runtime_error("Unknown function: " + name);
    }
    std::string str() const override { return name + "(" + arg->str() + ")"; }
};

// ---------------------------------------------------------------------------
// Expression handle + operator-overloaded DSL
// ---------------------------------------------------------------------------

class Expression {
public:
    std::shared_ptr<Expr> node;

    Expression() : node(nullptr) {}
    Expression(std::shared_ptr<Expr> n) : node(std::move(n)) {}
    // Implicit conversions let literals mix with expressions: x + 2.0
    Expression(double v) : node(std::make_shared<Constant>(v)) {}
    Expression(int v) : node(std::make_shared<Constant>(double(v))) {}
    // A named variable, e.g. Expression x("x");
    explicit Expression(const std::string& name)
        : node(std::make_shared<Variable>(name)) {}
    explicit Expression(const char* name)
        : node(std::make_shared<Variable>(std::string(name))) {}

    double eval(const Env& env) const {
        if (!node) throw std::runtime_error("Empty expression");
        return node->eval(env);
    }
    std::string str() const { return node ? node->str() : "<empty>"; }
};

inline Expression makeBinary(char op, const Expression& a, const Expression& b) {
    return Expression(std::make_shared<BinaryOp>(op, a.node, b.node));
}

inline Expression operator+(const Expression& a, const Expression& b) { return makeBinary('+', a, b); }
inline Expression operator-(const Expression& a, const Expression& b) { return makeBinary('-', a, b); }
inline Expression operator*(const Expression& a, const Expression& b) { return makeBinary('*', a, b); }
inline Expression operator/(const Expression& a, const Expression& b) { return makeBinary('/', a, b); }

// Unary minus: -x  ==  0 - x
inline Expression operator-(const Expression& a) { return makeBinary('-', Expression(0.0), a); }

// pow(a, b) and the a ^ b spelling both build a '^' node.
inline Expression pow(const Expression& a, const Expression& b) { return makeBinary('^', a, b); }
inline Expression operator^(const Expression& a, const Expression& b) { return makeBinary('^', a, b); }

inline Expression makeFunc(const std::string& name, const Expression& a) {
    return Expression(std::make_shared<FuncCall>(name, a.node));
}

inline Expression sin(const Expression& a)  { return makeFunc("sin", a); }
inline Expression cos(const Expression& a)  { return makeFunc("cos", a); }
inline Expression tan(const Expression& a)  { return makeFunc("tan", a); }
inline Expression exp(const Expression& a)  { return makeFunc("exp", a); }
inline Expression log(const Expression& a)  { return makeFunc("log", a); }
inline Expression sqrt(const Expression& a) { return makeFunc("sqrt", a); }
inline Expression abs(const Expression& a)  { return makeFunc("abs", a); }

} // namespace dsl

#endif
