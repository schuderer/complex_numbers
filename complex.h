#pragma once

#include <string>

template<typename T>
class Complex {
public:
    Complex();
    Complex(T re, T im);

    const T re() const;
    const T im() const;

    Complex operator+(const Complex& x) const;
    Complex operator-(const Complex& x) const;
    Complex operator*(const Complex& x) const;
    Complex operator/(const Complex& x) const;
    Complex operator-() const;
    Complex operator+=(const Complex& x);
    Complex operator-=(const Complex& x);
    Complex operator*=(const Complex& x);
    Complex operator/=(const Complex& x);

    Complex conjugate() const;

    std::string str() const;

protected:
    T re_;
    T im_;

private:
    uint32_t id_{0};
    inline static uint32_t next_id_{0};


};

template<typename T> std::ostream& operator<<(std::ostream &os, const Complex<T> &c);

// https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
//template class Complex<int>;
//template class Complex<float>;
//template class Complex<double>;
#include "complex.cpp"  // must not be compiled independently: https://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
