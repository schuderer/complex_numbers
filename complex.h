#pragma once

#include <string>
#include <sstream>
#include <iostream>

template<typename T>
class Complex {
public:
    struct calc_result {
        T re;
        T im;
    };
    Complex();
    Complex(T re, T im);
    Complex(calc_result val);

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

    // Factor out the calculations proper. Define them here so they get inlined (hopefully)
    // -- I mean, strictly speaking even the cpp is part of the header because it's included below, so it's potentially all inlined...
    // The "inline" keyword is superfluous, but I want to make my intentions clear.
    // I chose the slightly clumsy out-parameter option -- a custom struct with structured bindings would
    // be much nicer and also very fast, but for assignment operators, it adds almost twice the overhead (still much faster than tuples!).
    inline calc_result add(const T& re1, const T& im1, const T& re2, const T& im2) const {
        return {re1 + re2, im1 + im2};
    }
    inline calc_result sub(const T& re1, const T& im1, const T& re2, const T& im2) const {
        return {re1 - re2, im1 - im2};
    }
    inline calc_result mult(const T& re1, const T& im1, const T& re2, const T& im2) const {
        // (re1 + im1) * (re2 + im2) = re1*re2 + re1*im2 + im1*re2 + (-im2*im2) -> re=(re1*re2 - im2*im2), im=(re1*im2 + im1*re2)
        return {re1 * re2 - im1 * im2,
                re1 * im2 + im1 * re2};
    }
    inline calc_result div(const T& re1, const T& im1, const T& re2, const T& im2) const {
        // Multiply nominator and denominator of the fraction by the complex conjugate. Multiplied out, we get two fractions:
        T common_denominator = re2 * re2 - im2 * im2;
        return {(re1 * re2 + im1 * im2) / common_denominator,
                (im1 * re2 - re1 * im2) / common_denominator};
    }


// Making << a friend of this class: (actually not necessary because our << only calls the public method str())
// <typename T> would not work because template declarations cannot be nested. But it still needs
// the template in order to match the signature of the actual << declaration/definition:
//template<typename X>
//friend std::ostream& operator<<(std::ostream &os, const Complex<X> &c);

};

template<typename T> std::ostream& operator<<(std::ostream &os, const Complex<T> &c);

// https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
//template class Complex<int>;
//template class Complex<float>;
//template class Complex<double>;
#include "complex.cpp"  // must not be compiled independently: https://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
