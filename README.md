# complex_numbers
Playing around with C++ template classes and trying to figure out how to modularize them.

# Notes

## Helper function speed tests
I got to the point where I wanted helper functions to not have code duplication for the calculations
in the operator and respective assignment operator overloads.

Because these helpers have to return not one, but two values `re` and `im`, I tried several variants.
I found the results quite interesting:
 - A custom struct is the best combination of convenience and speed. Partucilarly intuitive to use with structured bindings (`auto [a, b] = ...`)
 - Slightly faster are out-parameters, but they are quite verbose to use. Although they end up taking half the time of structs for assignment operators, so I'm torn there.
 - Surprisingly, returning a full `Complex` class took only twice the time as above options (but then again, it's a very simple class with no heap-allocated members).
 - std::tuple did not work out, speedwise. Using tuples with std::get or structured bindings took 4-5 times as long of the best options.
 - Tuples with `tie(a, b)` were slowest, more than 8 times as slow as the best options.
 
TL;DR: Use structs and use structured bindings (`auto [a, b] = ...`) -- they are fast and intuitive. If the interface
does not matter, use out-parameters for maximum speed.
 

The variants (this now includes variants for usage):
```cpp
// ... inside class Complex (made everything public temporarily for the test) ...

// The "inline" keyword is superfluous, but I want to make my intentions clear.
// There are several ways to get a result of two values out of a function (I want to avoid instantiating a new Complex object):
// Variant 1: tuples. Nice to use (auto [re, im] = sub(...) or tie(re, im) = sub(...)). And returns rvalue which should be moved.
inline std::tuple<T, T> add0(const T& re1, const T& im1, const T& re2, const T& im2) const {
    return {re1 + re2, im1 + im2};
}
// Variant 2: out params. More or less guaranteed to not be copied.
inline void add2(const T& re1, const T& im1, const T& re2, const T& im2, T& out_re, T& out_im) const {
    out_re = re1 + re2;
    out_im = im1 + im2;
}
// Variant 3: struct. Should not be more expensive than instantiating our Complex class IMO, but hey...
struct re_im {
    T re;
    T im;
};
inline re_im add3(const T& re1, const T& im1, const T& re2, const T& im2) const {
    return {re1 + re2, im1 + im2};
}
// Variant 4: oh what the heck, let's try returning a Complex object, too
inline Complex add4(const T& re1, const T& im1, const T& re2, const T& im2) const {
    return {re1 + re2, im1 + im2};
}

// for testing the operators which use our helpers:
template<typename T>
Complex<T> Complex<T>::plus0(const Complex& x) const {  // baseline (should be fastest)
    return Complex(re_ + x.re_, im_ + x.im_);
}
template<typename T>
Complex<T> Complex<T>::plus1(const Complex& x) const { // using struct constructor
    return Complex(add(re_, im_, x.re_, x.im_)); // "add" here is our best choice from above (structs, "add3")
}
template<typename T>
Complex<T> Complex<T>::plus2(const Complex& x) const { // using structured bindings
    auto [re, im] = add(re_, im_, x.re_, x.im_);
    return Complex(re, im);
}
template<typename T>
Complex<T> Complex<T>::plus3(const Complex& x) const { // using out-param calculation function
    int re, im;
    add_out_params(re_, im_, x.re_, x.im_, re, im);
    return Complex(re, im);
}
template<typename T>
Complex<T> Complex<T>::pluseq0(const Complex& x) {  // baseline (should be fastest)
    re_ += x.re_;
    im_ += x.im_;
    return *this;
}
template<typename T>
Complex<T> Complex<T>::pluseq1(const Complex& x) { // using structured bindings
    auto [re, im] = add(re_, im_, x.re_, x.im_);
    re_ = re;
    im_ = im;
    return *this;
}
template<typename T>
Complex<T> Complex<T>::pluseq2(const Complex& x) { // using struct members
    auto re_im = add(re_, im_, x.re_, x.im_);
    re_ = re_im.re;
    im_ = re_im.im;
    return *this;
}
template<typename T>
Complex<T> Complex<T>::pluseq3(const Complex& x) { // using out-param calculation function
    add_out_params(re_, im_, x.re_, x.im_, re_, im_);
    return *this;
}

```

The quick-and-dirty timing code and results:
```cpp
auto start = std::chrono::high_resolution_clock::now();
Complex<int> t(0, 0);
//    for (int i = 0; i < 10000000; ++i) {  // Tuple with structured bindings (unpacking): ~155
//        auto [x, y] = t.add0(1,2,3,4);
//        x++, y++;
//    }
//    for (int i = 0; i < 10000000; ++i) {  // Tuple with tie: ~295
//        int x, y;
//        std::tie(x, y) = t.add0(1,2,3,4);
//        x++, y++;
//    }
//    for (int i = 0; i < 10000000; ++i) {  // Tuple with std::get: ~150
//        auto tup = t.add0(1,2,3,4);
//        (std::get<0>(tup))++, (std::get<1>(tup))++;
//    }
//    for (int i = 0; i < 10000000; ++i) {  // Out-parameters: ~34
//        int x, y;
//        t.add2(1,2,3,4,x,y);
//        x++, y++;
//    }
//    for (int i = 0; i < 10000000; ++i) {  // Custom struct: ~38  <-- speed/convenience winners!
//        Complex<int>::re_im result = t.add3(1,2,3,4);
//        result.re++, result.im++;
//    }
//  for (int i = 0; i < 10000000; ++i) {  // Custom struct: ~38  <-- speed/convenience winners!
//      auto [x, y] = t.add3(1,2,3,4);
//      x++, y++;
//  }
//    for (int i = 0; i < 10000000; ++i) {  // Class Complex: ~82
//        auto res = t.add4(1,2,3,4);
//        res.re_++, res.im_++;
//    }
//    for (int i = 0; i < 10000000; ++i) {  // Class Complex: ~82 with structured bindings
//        [[maybe_unused]] auto [x, y, unused] = t.add4(1,2,3,4);
//        // (void)unused; // actually faster without this
//        x++, y++;
//    }

// for testing the operators which use our helpers:
//    for (int i = 0; i < 10000000; ++i) {  // Baseline addition (calculation directly in body): ~133 (optimal)
//        Complex<int> t2(1, 2);
//        auto res = t2.plus0(Complex(3,4));
//        int x = res.im();
//    }
//    for (int i = 0; i < 10000000; ++i) {  // using struct constructor: ~166 -- 33 over optimal (naive delegating struct constructor: 180 -- 47 over optimal)
//        Complex<int> t2(1, 2);
//        auto res = t2.plus1(Complex(3,4));
//        int x = res.im();
//    }
//    for (int i = 0; i < 10000000; ++i) {  // using structured bindings: ~166 -- 33 over optimal
//        Complex<int> t2(1, 2);
//        auto res = t2.plus2(Complex(3,4));
//        int x = res.im();
//    }
//    for (int i = 0; i < 10000000; ++i) {  // using out-param calculation function: ~157 -- 24 over optimal
//        Complex<int> t2(1, 2);
//        auto res = t2.plus3(Complex(3,4));
//        int x = res.im();
//    }
//    for (int i = 0; i < 10000000; ++i) {  // Baseline addition (calculation directly in body): ~110 (optimal)
//        Complex<int> t2(1, 2);
//        auto res = t2.pluseq0(Complex(3,4));
//        int x = res.im();
//    }
    for (int i = 0; i < 10000000; ++i) {  // using structured bindings: ~144 -- 34 over optimal
        Complex<int> t2(1, 2);
        auto res = t2.pluseq1(Complex(3,4));
        int x = res.im();
    }
//    for (int i = 0; i < 10000000; ++i) {  // using struct members: ~144 -- 34 over optimal
//        Complex<int> t2(1, 2);
//        auto res = t2.pluseq2(Complex(3,4));
//        int x = res.im();
//    }
//    for (int i = 0; i < 10000000; ++i) {  // using out-param calculation function: ~127 -- 17 over optimal
//        Complex<int> t2(1, 2);
//        auto res = t2.pluseq3(Complex(3,4));
//        int x = res.im();
//    }
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::high_resolution_clock::now() - start);
cout << duration.count() << endl;
```

Here's some temporary code I used to check whether something is an lvalue or rvalue:
```cpp
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

void main() {
    sub(2, 5, 4, 10);
    check_lvalue_or_rvalue(a.re());
    int i = 3;
    check_lvalue_or_rvalue(7);
    check_lvalue_or_rvalue(i);
    
    return 0;
}

```
