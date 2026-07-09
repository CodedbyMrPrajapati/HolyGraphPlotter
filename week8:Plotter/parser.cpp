#include "parser.hpp"
#include <cctype>
#include <cmath>
#include <stdexcept>

using namespace dsl;

namespace plotter {

namespace {

// A hand-written recursive-descent parser over the grammar:
//
//   expr    := term (('+' | '-') term)*
//   term    := unary (('*' | '/') unary)*
//   unary   := ('-' | '+') unary | power
//   power   := primary ('^' unary)?          // right-associative; looser than unary
//   primary := number
//            | ident '(' expr ')'           // function call
//            | ident                        // variable / named constant
//            | '(' expr ')'
class Parser {
public:
    explicit Parser(const std::string& s) : src(s), pos(0) {}

    Expression parse() {
        Expression e = parseExpr();
        skipSpaces();
        if (pos != src.size())
            fail("unexpected trailing characters");
        return e;
    }

private:
    const std::string& src;
    size_t pos;

    [[noreturn]] void fail(const std::string& msg) const {
        throw std::runtime_error("Parse error at column " + std::to_string(pos + 1) + ": " + msg);
    }

    void skipSpaces() {
        while (pos < src.size() && std::isspace(static_cast<unsigned char>(src[pos])))
            ++pos;
    }

    char peek() {
        skipSpaces();
        return pos < src.size() ? src[pos] : '\0';
    }

    bool match(char c) {
        if (peek() == c) { ++pos; return true; }
        return false;
    }

    Expression parseExpr() {
        Expression left = parseTerm();
        for (;;) {
            char c = peek();
            if (c == '+') { ++pos; left = left + parseTerm(); }
            else if (c == '-') { ++pos; left = left - parseTerm(); }
            else break;
        }
        return left;
    }

    Expression parseTerm() {
        Expression left = parseUnary();
        for (;;) {
            char c = peek();
            if (c == '*') { ++pos; left = left * parseUnary(); }
            else if (c == '/') { ++pos; left = left / parseUnary(); }
            else break;
        }
        return left;
    }

    // Unary minus binds looser than '^', matching maths convention:
    // -2^2 == -(2^2) == -4, not (-2)^2.
    Expression parseUnary() {
        char c = peek();
        if (c == '-') { ++pos; return -parseUnary(); }
        if (c == '+') { ++pos; return parseUnary(); }
        return parsePower();
    }

    Expression parsePower() {
        Expression base = parsePrimary();
        if (peek() == '^') {
            ++pos;
            // right-associative (2^3^2 == 2^(3^2)); the exponent may itself
            // be signed, e.g. 2^-3.
            Expression exponent = parseUnary();
            return pow(base, exponent);
        }
        return base;
    }

    Expression parsePrimary() {
        char c = peek();

        if (c == '(') {
            ++pos;
            Expression e = parseExpr();
            if (!match(')')) fail("expected ')'");
            return e;
        }

        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            return parseNumber();
        }

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            return parseIdentifier();
        }

        if (c == '\0') fail("unexpected end of input");
        fail(std::string("unexpected character '") + c + "'");
    }

    Expression parseNumber() {
        skipSpaces();
        size_t start = pos;
        bool seenDot = false, seenDigit = false;
        while (pos < src.size()) {
            char ch = src[pos];
            if (std::isdigit(static_cast<unsigned char>(ch))) { seenDigit = true; ++pos; }
            else if (ch == '.') {
                if (seenDot) fail("malformed number (multiple '.')");
                seenDot = true;
                ++pos;
            }
            else break;
        }
        // optional exponent, e.g. 1e-3
        if (pos < src.size() && (src[pos] == 'e' || src[pos] == 'E')) {
            ++pos;
            if (pos < src.size() && (src[pos] == '+' || src[pos] == '-')) ++pos;
            bool expDigit = false;
            while (pos < src.size() && std::isdigit(static_cast<unsigned char>(src[pos]))) {
                expDigit = true; ++pos;
            }
            if (!expDigit) fail("malformed number (missing exponent digits)");
        }
        // Reject a token with no digits (e.g. a bare '.'), so std::stod is never
        // handed invalid input and the user gets our column-annotated error.
        if (!seenDigit) fail("malformed number (expected a digit)");
        return Expression(std::stod(src.substr(start, pos - start)));
    }

    Expression parseIdentifier() {
        skipSpaces();
        size_t start = pos;
        while (pos < src.size() &&
               (std::isalnum(static_cast<unsigned char>(src[pos])) || src[pos] == '_'))
            ++pos;
        std::string name = src.substr(start, pos - start);

        // Function call: name '(' expr ')'
        if (peek() == '(') {
            ++pos;
            Expression arg = parseExpr();
            if (!match(')')) fail("expected ')' after argument to " + name);
            return makeFunc(name, arg);
        }

        // Named constants
        if (name == "pi") return Expression(M_PI);
        if (name == "e")  return Expression(M_E);

        // Otherwise a variable (x, y, t, ...)
        return Expression(name);
    }
};

} // namespace

Expression parseExpression(const std::string& text) {
    Parser p(text);
    return p.parse();
}

} // namespace plotter
