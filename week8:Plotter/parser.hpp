#ifndef _PARSER_HPP_
#define _PARSER_HPP_

// Week 8 - runtime string parser.
//
// Turns a math string such as "sin(x)*cos(y) + 0.5*sin(t)" into a Week 7
// expression tree (dsl::Expression), so equations can be loaded from a text
// file at runtime instead of being hardcoded in C++.

#include "../week7:ExprDSL/expr.hpp"
#include <string>

namespace plotter {

// Parse a math expression string into a dsl::Expression tree.
// Supported: + - * / ^, parentheses, unary minus, the variables x/y/t, the
// constant pi, numeric literals, and the functions sin cos tan exp log sqrt abs.
// Throws std::runtime_error on a syntax error.
dsl::Expression parseExpression(const std::string& text);

} // namespace plotter

#endif
