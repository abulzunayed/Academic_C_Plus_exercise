#include <iostream>
#include <string>
#include <cassert>
#include <utility>
#include <algorithm>
#include <functional>
#include <sstream>
#include <cmath>
#include "Matrix.hpp"

using std::size_t;

class TestCase {
public:
    TestCase(std::string name) :
    name_(name) {
        std::cout << "Test Case START:\t" << name_ << std::endl;
    }
    ~TestCase() {
        std::cout << "Test Case END:  \t" << name_ << std::endl;
    }
private:
    std::string name_;
};
#define TESTCASE(name) TestCase _testcase(name)

//bool almostEqual(double a, double b, double epsilon = 1e-13)
//{
//    return std::abs(a - b) <= epsilon;
//}

// this function allows only matrices of equal dimensions
/*bool almostEqual(const Matrix& a, const Matrix& b, double epsilon = 1e-13) {
 assert(a.rows() == b.rows());
 assert(a.cols() == b.cols());
 for (size_t i = 0; i < a.rows(); ++i) {
 for (size_t j = 0; j < a.cols(); ++j) {
 if (!almostEqual(a(i, j), b(i, j), epsilon)) {
 return false;
 }
 }
 }
 return true;
 }
 */

 bool almostEqual(const Matrix& a, const Matrix& b, double epsilon = 1e-13)
{
     assert(a.rows() == b.rows());
     assert(a.cols() == b.cols());
     for (size_t i = 0; i < a.rows(); ++i)
     {
         for (size_t j = 0; j < a.cols(); ++j)
         {
             // if (!almostEqual(a(i, j), b(i, j), epsilon))
             if (!(std::abs(a(i, j) - b(i, j)) <= epsilon))
             {
                 return false;
             }
         }
     }
    return true;
 }

void test_get_set(size_t rows = 2, size_t cols = 4, double initValue = 1.0) {
    TESTCASE("test_get_set");
    const Matrix m1(rows, cols, initValue);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            assert("const get check" && m1(i, j) == initValue);
        }
    }
    // compile time check: taking address of const reference
    //std::cout<<*&m1(0 ,0)<<endl;
    //assert(* &m1(0, 0) == m1(0, 0));

    std::swap(rows, cols);
    Matrix m2(rows, cols, 0.0);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            m2(i, j) = initValue;
            assert("set check" && m2(i, j) == initValue);
        }
    }
    auto begin = &m2(0, 0);
    auto end = &m2(rows - 1, cols - 1) + 1;
    size_t diff = end - begin;
    assert("1D memory layout check" && (diff == rows * cols));
    assert("row wise order check" && (&m2(0, 1) - &m2(0, 0) == 1));
    assert("row wise order check" && (size_t(&m2(1, 0) - &m2(0, 0)) == cols));
}

void test_memory(size_t rows = 2, size_t cols = 4, double initValue = 1.0)
{
    TESTCASE("test_memory");
    {
        Matrix m1(rows, cols, initValue);
        assert("copy ctr check" && almostEqual(Matrix(m1), m1, 0));
        Matrix m7(5, 5, 5);
        assert("no alias check" && &Matrix(m1)(0, 0) != &m1(0, 0));
        Matrix m2(rows, cols, initValue);
        m2 = m1;
        assert("assignment check" && almostEqual(m1, m2, 0));
        assert("no alias check" && &m1(0, 0) != &m2(0, 0));
        // allow zero dimensions
        Matrix m3(0, 0, 0.0);
        // allow assignment of matrices of different dimensions
        //m3 = Matrix(1, 1, 0.0);
        //m3 = m1;
        //assert("check assignment" && almostEqual(m1, m3, 0));
        auto initValue2 = 1.5;
        Matrix m4(rows, cols, initValue2);
        Matrix m4copy(rows, cols, initValue2);
        m4 = m4;
        // check self assignment
        assert("self assignment check" && almostEqual(m4, m4copy, 0));
    }
    // important: implementing move constructor / move assignment is optional
    // important: if your code does not support move constructor / move assignment the following will work anyway
    // uses move constructor if present else: uses copy ctor
    {
        Matrix m1(rows, cols, initValue);
        Matrix move_constructed(std::move(Matrix(m1)));
        assert("testing copy/move constructor" && almostEqual(m1, move_constructed, 0.0));
    }
    // uses move assignment if present else: uses assignment operator
    {
    Matrix m1(rows, cols, initValue);
    Matrix m2(1, 1, 0);
//    m2 = std::move(Matrix(m1));
//    assert("testing assignment/move assignment" && almostEqual(m1, m2, 0.0));
    }
}

void test_compare(size_t rows = 2, size_t cols = 4, double v = 1.0) {
    TESTCASE("test_compare");
    {
        Matrix m1(rows, cols, v);
        assert("compare check" && m1 == m1);
        assert("compare check" && !(m1 != m1));
        assert("compare check" && m1 == Matrix(m1));
        assert("compare check" && !(m1 != Matrix(m1)));
        const Matrix m2 = m1;
        m1(rows - 1, cols - 1) += 1;
        assert("compare check" && m1 != m2);
        assert("compare check" && !(m1 == m2));
    }
    std::swap(rows, cols);
    assert("compare check" && Matrix(rows, cols, v) == Matrix(rows, cols, v));
    assert(    "compare check"    && !(Matrix(rows, cols, v) != Matrix(rows, cols, v)));
    // allow comparison of matrices of different dimensions
    assert("compare check" && Matrix(cols, rows, v) != Matrix(rows, cols, v));
    assert(
           "compare check"
           && !(Matrix(cols, rows, v) == Matrix(rows, cols, v)));
    assert("compare check" && Matrix(0, 0, 0) != Matrix(rows, cols, v));
    assert("compare check" && !(Matrix(0, 0, 0) == Matrix(rows, cols, v)));
    assert("compare check" && Matrix(0, 0, 0) == Matrix(0, 0, 0));
    assert("compare check" && !(Matrix(0, 0, 0) != Matrix(0, 0, 0)));
}

void test_arithmetic(size_t rows = 2, size_t cols = 4) {
    TESTCASE("test_arithmetic");
    std::function<double(size_t, size_t)> f =
    [](size_t i, size_t j) {return j * 100 + i;};
    Matrix m1(rows, cols, 0);
    Matrix m2(rows, cols, 0);
    Matrix sum_result(rows, cols, 0);
    print(sum_result);
    Matrix diff_result(rows, cols, 0);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j)
        {
            m1(i, j) = f(i, j);
            m2(i, j) = 2 * f(i, j);
            sum_result(i, j) = m1(i, j) + m2(i, j);
            diff_result(i, j) = m1(i, j) - m2(i, j);
        }
    }
    Matrix sum1 = m1 + m2;
    Matrix sum2 = m1;
    sum2 += m2;
    Matrix diff1 = m1 - m2;
    Matrix diff2 = m1;
    diff2 -= m2;
    assert("check operator+" && almostEqual(sum_result, sum1));
    assert("check operator+=" && almostEqual(sum_result, sum2));
    assert("check operator-" && almostEqual(diff_result, diff1));
    assert("check operator-=" && almostEqual(diff_result, diff2));

    double a = 3, b = 4;
    Matrix prod = Matrix(1, 1, a) * Matrix(1, 1, b);
    assert("check operator*" && almostEqual(prod, Matrix(1, 1, a * b)));
    size_t n = 10;
    prod = Matrix(1, n, a) * Matrix(n, 1, b);
    assert("check operator*" && almostEqual(prod, Matrix(1, 1, n * a * b)));
    //prod = Matrix(2, n, a) * Matrix(n, 3, b);
    //assert("check operator*" && almostEqual(prod, Matrix(2, 3, n * a * b)));
    //prod = Matrix(n, 1, a) * Matrix(1, n, b);
    //assert("check operator*" && almostEqual(prod, Matrix(n, n, a * b)));
    n = 100000;
    prod = Matrix(1, n, a) * Matrix(n, 1, b);
    assert("check operator*" && almostEqual(prod, Matrix(1, 1, n * a * b)));
    n = 10;
    // prod = Matrix(2, n, a);
    //prod *= Matrix(n, 3, b);
    // assert("check operator*=" && almostEqual(prod, Matrix(2, 3, n * a * b)));
    Matrix unity(n, n, 0);
    Matrix v(n, 1, 0);
    for (size_t i = 0; i < n; ++i) {
        unity(i, i) = 1;
        v(i, 0) = f(i, i);
    }
    assert("check operator*" && almostEqual(v, unity * v));
}

void test_input_output_self_consistency(size_t rows = 2, size_t cols = 4) {
    TESTCASE("test_input_output_self_consistency");
    Matrix m1(rows, cols, 0);
    Matrix m2(rows, cols, 0);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            m1(i, j) = (j * 100 + i) / 9.0;
        }
    }
    std::stringstream ss;
    ss << m1;
    //ss >> m2;
    // checking if your operator>> can parse your own output generated by operator<<
    // assert("check output and input operator" && almostEqual(m1, m2, 1e-4));
}

int main() {
    //test_get_set();
    test_memory();
    test_compare();
    test_arithmetic();
    test_input_output_self_consistency();
    std::cout << "all tests finished without assertion errors" << std::endl;
}
