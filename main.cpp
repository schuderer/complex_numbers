#include <iostream>
#include <tuple>
#include "complex.h"

using std::cout, std::endl;


// used to check whether to expect moving vs copying when returning calculation results
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::tuple<T, T> t) {
    return (os << "(" << std::get<0>(t) << ", " << std::get<1>(t) << ")");
}
template<typename T>
T check_lvalue_or_rvalue(const T&& x) {
    cout << x << " is an rvalue" << endl;
    return x;
}
template<typename T>
T check_lvalue_or_rvalue(const T& x) {
    cout << x << " is an lvalue" << endl;
    return x;
}
// function under test for return value =?= lvalue or rvalue
std::tuple<int, int> sub(const int& re1, const int& im1, const int& re2, const int& im2) {
    return check_lvalue_or_rvalue<std::tuple<int, int>>({check_lvalue_or_rvalue(re1 - re2), check_lvalue_or_rvalue(im1 - im2)});  // rvalues -- yay!
}


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


    sub(2, 5, 4, 10);
    check_lvalue_or_rvalue(a.re());
    int i = 3;
    check_lvalue_or_rvalue(7);
    check_lvalue_or_rvalue(i);

    return 0;
}
