#include <iostream>
#include <string>
#include <sstream>
#include "complex.h"


// Constructors

template<typename T>
Complex<T>::Complex() : Complex{0, 0} {};

template<typename T>
Complex<T>::Complex(T re, T im) : re_{re}, im_{im} {
    id_ = next_id_++;
}
template<typename T>
Complex<T>::Complex(calc_result val): Complex{val.re, val.im} {};


// Getters

template<typename T>
const T Complex<T>::re() const {
    return re_;
}
template<typename T>
const T Complex<T>::im() const {
    return im_;
}


// Operators

template<typename T>
Complex<T> Complex<T>::operator+(const Complex& x) const {
    return Complex(add(re_, im_, x.re_, x.im_));
}

template<typename T>
Complex<T> Complex<T>::operator-(const Complex& x) const {
    return Complex(sub(re_, im_, x.re_, x.im_));
}

template<typename T>
Complex<T> Complex<T>::operator*(const Complex& x) const {
    return Complex(mult(re_, im_, x.re_, x.im_));
}

template<typename T>
Complex<T> Complex<T>::operator/(const Complex& x) const {
    return Complex(div(re_, im_, x.re_, x.im_));
}

template<typename T>
Complex<T> Complex<T>::operator-() const {
    return Complex(-re_, -im_);
}

template<typename T>
Complex<T> Complex<T>::operator+=(const Complex& x) {
    // Which one to use: `auto result` or `auto result`? The returned value is an rvalue,
    // so should be moved (not copied) anyways.
    // Does `const auto&` help anything in this case? Or is it unnecessary?
    // const auto& result = add(re_, im_, x.re_, x.im_);
    auto result = add(re_, im_, x.re_, x.im_);  // could also have used std::tuple, tie, structured binding, ...
    re_ = result.re;
    im_ = result.im;
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator-=(const Complex& x) {
    auto result = sub(re_, im_, x.re_, x.im_);
    re_ = result.re;
    im_ = result.im;
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator*=(const Complex& x) {
    auto result = mult(re_, im_, x.re_, x.im_);
    re_ = result.re;
    im_ = result.im;
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator/=(const Complex& x) {
    auto result = div(re_, im_, x.re_, x.im_);
    re_ = result.re;
    im_ = result.im;
    return *this;
}


// Helpers

template<typename T>
Complex<T> Complex<T>::conjugate() const {
    return Complex(re_, -im_);
}

template<typename T>
std::string Complex<T>::str() const {
    std::stringstream stream;
    stream << "Complex<" << this << ">(" << re_ << ", " << im_ << ", id="<< id_ << ")";
    return stream.str();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Complex<T> &c) {
    return (os << c.str());
}
