# complex_numbers
Learning C++ and playing around with template classes.

# Notes

## Helper function speed tests
I got to the point where I wanted helper functions to not have code duplication for the calculations
in the operator and respective assignment operator overloads.

Because these helpers have to return not one, but two values `re` and `im`, I tried several variants.
Naively, I tested them in debug mode and got interesting results (out-params were fastest, followed by
custom structs, followed by returning a full `Complex` object and then followed by `std:tuple`.

Alas, when I tried the same in release mode, they were all equally fast. I even had to tweak the
tests so that the loops don't get optimized out completely.

So, I did not learn what runs the fastest, but I learned that benchmarking is hard. :) I also learned that
structured bindings (`auto [a, b] = ...`) are awesome, as fast as it gets, and work with tuples,
structs, and even some objects.

(I still like to think that paying attention to reference vs copy, lvalue/rvalues etc. paid off --
but if I'm completely honest I have a feeling that my newbie's hubris would not survive a test of that assumption, either :) ) 

The variants under tests (this now includes variants for usage):
```cpp
// ... inside header of class Complex (made everything public temporarily for the test) ...

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
//    struct re_im {
//        T re;
//        T im;
//    };
inline re_im add3(const T& re1, const T& im1, const T& re2, const T& im2) const {
    return {re1 + re2, im1 + im2};
}
// Variant 4: oh what the heck, let's try returning a Complex object, too
inline Complex add4(const T& re1, const T& im1, const T& re2, const T& im2) const {
    return {re1 + re2, im1 + im2};
}

// for testing the operators which use our helpers:
Complex plus0(const Complex& x) const;
Complex plus1(const Complex& x) const;
Complex plus2(const Complex& x) const;
Complex plus3(const Complex& x) const;
Complex plus4(const Complex& x) const;
Complex pluseq0(const Complex& x);
Complex pluseq1(const Complex& x);
Complex pluseq2(const Complex& x);
Complex pluseq3(const Complex& x);
Complex pluseq4(const Complex& x);

// inside the class definition in complex.cpp:
// By the way, moving the plusX/pluseqX definitions into the header made no difference.
template<typename T>
Complex<T> Complex<T>::plus0(const Complex& x) const {  // baseline (should be fastest)
    return Complex(re_ + x.re_, im_ + x.im_);
}
template<typename T>
Complex<T> Complex<T>::plus1(const Complex& x) const { // using struct constructor
    return Complex(add3(re_, im_, x.re_, x.im_)); // "add" here is our best choice from above (structs)
}
template<typename T>
Complex<T> Complex<T>::plus2(const Complex& x) const { // using structured bindings
    auto [re, im] = add3(re_, im_, x.re_, x.im_);
    return Complex(re, im);
}
template<typename T>
Complex<T> Complex<T>::plus3(const Complex& x) const { // using out-param calculation function
    int re, im;
    add2(re_, im_, x.re_, x.im_, re, im);
    return Complex(re, im);
}
template<typename T>
Complex<T> Complex<T>::plus4(const Complex& x) const { // using Complex object
    return add4(re_, im_, x.re_, x.im_);
}
template<typename T>
Complex<T> Complex<T>::pluseq0(const Complex& x) {  // baseline (should be fastest)
    re_ += x.re_;
    im_ += x.im_;
    return *this;
}
template<typename T>
Complex<T> Complex<T>::pluseq1(const Complex& x) { // using structured bindings
    auto [re, im] = add3(re_, im_, x.re_, x.im_);
    re_ = re;
    im_ = im;
    return *this;
}
template<typename T>
Complex<T> Complex<T>::pluseq2(const Complex& x) { // using struct members
    auto re_im = add3(re_, im_, x.re_, x.im_);
    re_ = re_im.re;
    im_ = re_im.im;
    return *this;
}
template<typename T>
Complex<T> Complex<T>::pluseq3(const Complex& x) { // using out-param calculation function
    add2(re_, im_, x.re_, x.im_, re_, im_);
    return *this;
}
template<typename T>
Complex<T> Complex<T>::pluseq4(const Complex& x) { // using Complex object
    auto [re_out, im_out, unused] = add4(re_, im_, x.re_, x.im_);
    re_ = re_out;
    im_ = im_out;
    return *this;
}
```

The quick-and-dirty timing code and results:
```cpp
int x_result{3}, y_result{1};
Complex<int> t(0, 0);
auto start = std::chrono::high_resolution_clock::now();
//    // sanity check:
//    std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    for (uint64_t i = 0; i < 1000000000; ++i) {  // Tuple with structured bindings (unpacking): ~648 (debug mode: ~155 (debug mode had slightly different setup as I did not have to thwart release mode optimization))
//        // Added weird calculations in an attempt to avoid a time of zero due to optimization.
//        // I don't really care about int overflows here.
//        auto [x, y] = t.add0(x_result,y_result,2/i/4,-i/2);
//        x_result += x;
//        y_result += y;
//    }
//    for (uint64_t i = 0; i < 1000000000; ++i) {  // Tuple with tie: ~649 (debug mode: ~295)
//        int x, y;
//        std::tie(x, y) = t.add0(x_result,y_result,2/i/4,-i/2);
//        x_result += x;
//        y_result += y;
//    }
//    for (uint64_t i = 0; i < 1000000000; ++i) {  // Tuple with std::get: ~654 (debug mode: ~150)
//        auto tup = t.add0(x_result,y_result,2/i/4,-i/2);
//        x_result += (std::get<0>(tup));
//        y_result += (std::get<1>(tup));
//    }
//    for (uint64_t i = 0; i < 1000000000; ++i) {  // Out-parameters: ~635 (debug mode: ~34)
//        int x, y;
//        t.add2(x_result,y_result,2/i/4,-i/2, x, y);
//        x_result += x;
//        y_result += y;
//    }
//    for (uint64_t i = 0; i < 1000000000; ++i) {  // Custom struct: ~635 (debug mode: ~38)
//        Complex<int>::re_im res = t.add3(x_result,y_result,2/i/4,-i/2);
//        x_result += res.re;
//        y_result += res.im;
//    }
//    for (uint64_t i = 0; i < 1000000000; ++i) {  // Custom struct: ~635 (debug mode: ~38)
//        auto [x, y] = t.add3(x_result,y_result,2/i/4,-i/2);
//        x_result += x;
//        y_result += y;
//    }
//    for (uint64_t i = 0; i < 1000000000; ++i) {  // Class Complex: ~630 (debug mode: ~82)
//        auto res = t.add4(x_result,y_result,2/i/4,-i/2);
//        x_result += res.re_;
//        y_result += res.im_;
//    }
//    for (uint64_t i = 0; i < 1000000000; ++i) {  // Class Complex with structured bindings: ~630 (debug mode: ~82)
//        [[maybe_unused]] auto [x, y, unused] = t.add4(x_result,y_result,2/i/4,-i/2);
//        // (void)unused; // actually faster without this
//        x_result += x;
//        y_result += y;
//    }
//
// for testing the operators which use our helpers:
//    for (int i = 0; i < 500000000; ++i) {  // Baseline addition (calculation directly in body): ~480 (debug mode: ~133 (optimal))
//        Complex<int> t2(1, 2);
//        auto res = t2.plus0(Complex(x_result,-i/2));
//        x_result += res.re();
//        y_result += res.im();
//    }
//    for (int i = 0; i < 500000000; ++i) {  // using struct constructor: ~480 - delegating constructor: also ~9! (debug mode: ~166 -- 33 over optimal (naive delegating struct constructor: 180 -- 47 over optimal))
//        Complex<int> t2(1, 2);
//        auto res = t2.plus1(Complex(x_result,-i/2));
//        x_result += res.re();
//        y_result += res.im();
//    }
//    for (int i = 0; i < 500000000; ++i) {  // using structured bindings: ~480 (debug mode: ~166 -- 33 over optimal)
//        Complex<int> t2(1, 2);
//        auto res = t2.plus2(Complex(x_result,-i/2));
//        x_result += res.re();
//        y_result += res.im();
//    }
//    for (int i = 0; i < 500000000; ++i) {  // using out-param calculation function: ~480 (debug mode: ~157 -- 24 over optimal)
//        Complex<int> t2(1, 2);
//        auto res = t2.plus3(Complex(x_result,-i/2));
//        x_result += res.re();
//        y_result += res.im();
//    }
//    for (int i = 0; i < 500000000; ++i) {  // Baseline addition (calculation directly in body): ~480 (debug mode: ~110 (optimal))
//        Complex<int> t2(1, 2);
//        auto res = t2.pluseq0(Complex(x_result,-i/2));
//        x_result += res.re();
//        y_result += res.im();
//    }
//    for (int i = 0; i < 500000000; ++i) {  // using structured bindings: ~480 (debug mode: ~144 -- 34 over optimal)
//        Complex<int> t2(1, 2);
//        auto res = t2.pluseq1(Complex(x_result,-i/2));
//        x_result += res.re();
//        y_result += res.im();
//    }
//    for (int i = 0; i < 500000000; ++i) {  // using struct members: ~480 (debug mode: ~144 -- 34 over optimal)
//        Complex<int> t2(1, 2);
//        auto res = t2.pluseq2(Complex(x_result,-i/2));
//        x_result += res.re();
//        y_result += res.im();
//    }
//    for (int i = 0; i < 500000000; ++i) {  // using out-param calculation function: ~480 (debug mode: ~127 -- 17 over optimal)
//        Complex<int> t2(1, 2);
//        auto res = t2.pluseq3(Complex(x_result,-i/2));
//        x_result += res.re();
//        y_result += res.im();
//    }
for (int i = 0; i < 500000000; ++i) {  // using Class Complex: ~480 (debug mode: ~127 -- 17 over optimal)
    Complex<int> t2(1, 2);
    auto res = t2.pluseq4(Complex(x_result,-i/2));
    x_result += res.re();
    y_result += res.im();
}
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::high_resolution_clock::now() - start);
// It's important to do something with the result:
cout << duration.count() << " (result: " << x_result << "," << y_result << ")" << endl;
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
