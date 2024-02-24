#include <iostream>

#include <algorithm>
#include <chrono>
#include <functional>
#include <list>
#include <random>

#include "Matrix.h"
#include "Vector.h"
#include "Stencil.h"

#define PI 3.141592653589793

// util hx expressions

template<size_t numPoints> constexpr double hxCalc( ) { return 1. / (numPoints - 1); }
template<size_t numPoints> constexpr double hxSqCalc( ) { return hxCalc<numPoints>( ) * hxCalc<numPoints>( ); }

// util timer

double measureTime(std::function<void( )> toMeasure) {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now( );
	toMeasure( );
	end = std::chrono::system_clock::now( );
	std::chrono::duration<double> elapsed = end - start;
	return elapsed.count( );
}

// util shuffle

template<typename container_t>
container_t shuffled(container_t container) {
	std::random_device rd;
	std::mt19937 gen(rd( ));
	std::shuffle(container.begin( ), container.end( ), gen);
	return container;
}

// actual solver
// return number of iterations required

template<typename T, typename MatrixImpl, size_t numPoints>
int solve (const MatrixLike<T, MatrixImpl, numPoints, numPoints>& A, const Vector<T, numPoints>& b, Vector<T, numPoints>& u) {
	double initRes = (b - A * u).l2Norm( ); // determine the initial residual
	double curRes = initRes;

	unsigned int curIt = 0; // store the current iteration index

	while (curRes > 1.e-5 * initRes) { // solve until the residual is reduced by a certain amount
		++curIt;
		u += A.inverseDiagonal( ) * (b - A * u); // Jacobi step
		curRes = (b - A * u).l2Norm( ); // update the residual
	}

	return curIt;
}

// tests solver using a full matrix
// returns number of iterations and runtime required

template<size_t numPoints>
std::pair<int, double> testFullMatrix (const Vector<double, numPoints> b) {
	constexpr double hxSq = hxSqCalc<numPoints>( );

	Matrix<double, numPoints, numPoints> A(0.);
	Vector<double, numPoints> u(0.);

	A(0, 0) = 1.;
	for (size_t x = 1; x < numPoints - 1; ++x) {
		A(x, x - 1) = 1. / hxSq;
		A(x, x) = -2. / hxSq;
		A(x, x + 1) = 1. / hxSq;
	}
	A(numPoints - 1, numPoints - 1) = 1.;

	int numIts = 0;
	double time = measureTime ([&] { numIts = solve(A, b, u); });
	return std::make_pair(numIts, time);
}


// tests solver using the stencil class
// returns number of iterations and runtime required

template<size_t numPoints>
std::pair<int, double> testStencil (const Vector<double, numPoints> b) {
	constexpr double hxSq = hxSqCalc<numPoints>( );

	Vector<double, numPoints> u(0.);
	std::vector<StencilEntry<double> > innerStencil{ { -1, 1. / hxSq },{ 0, -2. / hxSq },{ 1, 1. / hxSq } };
	Stencil<double, numPoints, numPoints> A ({ { 0, 1. } }, shuffled(innerStencil));

	int numIts = 0;
	double time = measureTime ([&] { numIts = solve(A, b, u); });
	return std::make_pair(numIts, time);
}

// test function implementation

template<size_t numPoints>
void testImpl (int expectedNumIts) {
	std::cout << "Checking " << numPoints << " grid points and expecting " << expectedNumIts << " iterations:" << std::endl;

	Vector<double, numPoints> b(0.0);
	for (size_t x = 0; x < numPoints; ++x) {
		b(x) = sin(2. * PI * (1. + x / (double)(numPoints - 1))) + cos(PI * (x / (double)(numPoints - 1)));
	}

	auto resMatrix = testFullMatrix<numPoints>(b);
	auto resStencil = testStencil<numPoints>(b);

	std::cout << "\tThe matrix implementation required  " << resMatrix.first << " iterations and " << resMatrix.second << " seconds" << std::endl;
	std::cout << "\tThe stencil implementation required " << resStencil.first << " iterations and " << resStencil.second << " seconds" << std::endl;
	std::cout << "\tThis means a speedup factor of " << resMatrix.second / resStencil.second << std::endl;

	assert(resMatrix.first == resStencil.first && "Number of iterations not equivalent for matrix-stencil comparison");
	assert(resStencil.first == expectedNumIts && "Number of iterations required does not match expected result");
	assert(resMatrix.second > resStencil.second && "Runtime of the stencil test case is too high");
}

// recursive test function wrapper
// gcc requires double wrapping

template<size_t... testcases> void runTests(std::list<int>& expected);

template<size_t curTestCase, size_t nextTestCase, size_t... remaining>
void runTestsImpl(std::list<int>& expected) {
	testImpl<curTestCase>(expected.front( ));
	expected.pop_front( );
	runTests<nextTestCase, remaining...>(expected);
}

template<size_t curTestCase>
void runTestsImpl(std::list<int>& expected) {
	testImpl<curTestCase>(expected.front( ));
}

template<size_t... testcases> void runTests(std::list<int>& expected) { runTestsImpl<testcases...>(expected); }

// actual main

int main(int argc, char** argv) {
	std::list<int> expected{ 743, 1676, 2982, 9142, 11941, 26874 };
	runTestsImpl<33, 49, 65, 113, 129, 193>(expected);
}
