// Week 7 demo: build math expressions with the operator-overloaded C++ DSL,
// print their tree form, and evaluate them.

#include "expr.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace dsl;

int main()
{
    // The variables that surfaces are built from.
    Expression x("x"), y("y");

    // The Week 6 surface, written directly in C++:
    //     z = sin(x) * cos(y)
    Expression z = sin(x) * cos(y);

    // A slightly richer expression mixing literals, operators and functions.
    Expression w = pow(x, 2.0) + Expression(0.5) * sin(x + y) - cos(y);

    std::cout << "Expression z = " << z.str() << "\n";
    std::cout << "Expression w = " << w.str() << "\n\n";

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  x      y      z=sin(x)cos(y)   (reference)\n";
    std::cout << "  ---------------------------------------------\n";

    double samples[][2] = { {0.0, 0.0}, {1.0, 2.0}, {3.14159265, 0.0}, {1.5708, 1.5708} };
    for (auto& s : samples) {
        Env env{ {"x", s[0]}, {"y", s[1]} };
        double got = z.eval(env);
        double ref = std::sin(s[0]) * std::cos(s[1]);
        std::cout << "  " << std::setw(6) << s[0]
                  << " " << std::setw(6) << s[1]
                  << "   " << std::setw(10) << got
                  << "     " << std::setw(10) << ref
                  << ((std::fabs(got - ref) < 1e-9) ? "   OK" : "   MISMATCH")
                  << "\n";
    }

    std::cout << "\n  w(x=2, y=1) = " << w.eval({ {"x", 2.0}, {"y", 1.0} }) << "\n";

    return 0;
}
