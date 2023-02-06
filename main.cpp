#include <iostream>
#include "complex.h"

using std::cout, std::endl;

int main()
{
    Complex<int> x{};
    Complex a{3.0f, 2.0f};
    Complex<float> b{1, 4};

    cout << x + Complex{1, 2} << endl;
    auto c = a * b;
    cout << a << " * " << b << " = " << c << endl;

    cout << a << " /= " << c << " = ..."<< endl;
    a /= c;
    cout << " = " << a << endl;

    return 0;
}
