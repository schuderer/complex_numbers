#include <iostream>
#include <string>
#include <sstream>
#include "complex.h"

template<typename T>
Complex<T>::Complex(): Complex(0, 0) {};

template<typename T>
Complex<T>::Complex(T re, T im) : re_{re}, im_{im} {
    id_ = next_id_++;
}

template<typename T>
const T Complex<T>::re() const {
    return re_;
}
template<typename T>
const T Complex<T>::im() const {
    return im_;
}

template<typename T>
Complex<T> Complex<T>::operator+(const Complex& x) const {
    Complex c = Complex(re_ + x.re_, im_ + x.im_);
    std::cout << "this:" << str() << " + x:" << x.str() << " == c:" << c.str() << std::endl;
    return c;
}

template<typename T>
Complex<T> Complex<T>::operator-(const Complex& x) const {
    Complex c = Complex(re_ - x.re_, im_ - x.im_);
    std::cout << "this:" << str() << " - x:" << x.str() << " == c:" << c.str() << std::endl;
    return c;
}

template<typename T>
Complex<T> Complex<T>::operator*(const Complex& x) const {
    // (re1 + im1) * (re2 + im2) = re1*re2 + re1*im2 + im1*re2 + (-im2*im2) -> re=(re1*re2 - im2*im2), im=(re1*im2 + im1*re2)
    Complex c = Complex(re_ * x.re_ - im_ * x.im_,
                        re_ * x.im_ + im_ * x.re_);
    std::cout << "this:" << str() << " * x:" << x.str() << " = c:" << c.str() << std::endl;
    return c;
}

template<typename T>
Complex<T> Complex<T>::operator/(const Complex& x) const {
    // Multiply nominator and denominator of the fraction by the complex conjugate. Multiplied out, we get two fractions:
    double common_denominator = x.re_ * x.re_ + x.im_ * x.im_;
    Complex c = Complex((re_ * x.re_ + im_ * x.im_) / common_denominator,
                        (im_ * x.re_ - re_ * x.im_) / common_denominator);
    std::cout << "this:" << str() << " / x:" << x.str() << " == c:" << c.str() << std::endl;
    return c;
}

template<typename T>
Complex<T> Complex<T>::operator-() const {
    return Complex(-re_, -im_);
}

template<typename T>
Complex<T> Complex<T>::operator+=(const Complex& x) {
    std::cout << "this:" << str() << " + x:" << x.str() << " =" << std::endl;
    re_ += x.re_;
    im_ += x.im_;
    std::cout << "   = this:" << str() << std::endl;
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator-=(const Complex& x) {
    std::cout << "this:" << str() << " - x:" << x.str() << " =" << std::endl;
    re_ -= x.re_;
    im_ -= x.im_;
    std::cout << "   = this:" << str() << std::endl;
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator*=(const Complex& x) {
    std::cout << "this:" << str() << " * x:" << x.str() << " =" << std::endl;
    re_ = re_ * x.re_ - im_ * x.im_;
    im_ = re_ * x.im_ + im_ * x.re_;
    std::cout << "   = this:" << str() << std::endl;
    return *this;
}

template<typename T>
Complex<T> Complex<T>::operator/=(const Complex& x) {
    std::cout << "this:" << str() << " / x:" << x.str() << " =" << std::endl;
    // Multiply nominator and denominator of the fraction by the complex conjugate. Multiplied out, we get two fractions:
    T common_denominator = x.re_ * x.re_ + x.im_ * x.im_;
    re_ = (re_ * x.re_ + im_ * x.im_) / common_denominator;
    im_ = (im_ * x.re_ - re_ * x.im_) / common_denominator;
    std::cout << "   = this:" << str() << std::endl;
    return *this;
}

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
std::ostream& operator<<(std::ostream &os, const Complex<T> &c) {
    return (os << c.str());
}

/*
 // Ansatz zum Ausgliedern der Berechnung ohne überflüssige Objekt-Instanziierung (nicht fertig)
template<typename T>
void Complex<T>::add(const Complex& x, Complex& out) const {
    out.re_ = re_ + x.re;
    out.im_ = im_ + x.im_;
}
template<typename T>
void Complex<T>::sub(const Complex& x, Complex& out) const {
    out.re_ = re_ - out.re_;
    out.im_ = im_ - out.im_;
}
template<typename T>
void Complex<T>::mult(const Complex& x, Complex& out) const {
    out.re_ = re_ * x.re_ - im_ * x.im_;
    out.im_ = re_ * x.im_ + im_ * x.re_;
}
template<typename T>
void Complex<T>::div(const Complex& x, Complex& out) const {
    T common_denominator = out.re_ * out.re_ - out.im_ * out.im_;
    re_ = (x.re_ * out.re_ + x.im_ * out.im_) / common_denominator;
    im_ = (x.im_ * out.re_ - x.re_ * out.im_) / common_denominator;
}
*/
