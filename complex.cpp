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
    T out_re, out_im;
    add(re_, im_, x.re_, x.im_, out_re, out_im);
    return Complex(out_re, out_im);
}

template<typename T>
Complex<T> Complex<T>::operator-(const Complex& x) const {
    T out_re, out_im;
    sub(re_, im_, x.re_, x.im_, out_re, out_im);
    return Complex(out_re, out_im);
}

template<typename T>
Complex<T> Complex<T>::operator*(const Complex& x) const {
    T out_re, out_im;
    mult(re_, im_, x.re_, x.im_, out_re, out_im);
    return Complex(out_re, out_im);
}

template<typename T>
Complex<T> Complex<T>::operator/(const Complex& x) const {
    T out_re, out_im;
    div(re_, im_, x.re_, x.im_, out_re, out_im);
    return Complex(out_re, out_im);
}

template<typename T>
Complex<T> Complex<T>::operator-() const {
    return Complex(-re_, -im_);
}

template<typename T>
Complex<T> Complex<T>::operator+=(const Complex& x) {
    add(re_, im_, x.re_, x.im_, re_, im_);
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator-=(const Complex& x) {
    sub(re_, im_, x.re_, x.im_, re_, im_);
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator*=(const Complex& x) {
    mult(re_, im_, x.re_, x.im_, re_, im_);
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator/=(const Complex& x) {
    div(re_, im_, x.re_, x.im_, re_, im_);
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
