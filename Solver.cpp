#include <iostream>
#include <chrono>

#include "Vector.h"
#include "Matrix.h"

#define PI 3.141592653589793


template <class T>
Vector<T> operator* (const Matrix<T>& m, const Vector<T>& u)
{
    size_t row = m.rows();

    Vector<T> res(row, 0);

    for (size_t i = 0; i < row; ++i)
        for (size_t k = 0; k < m.cols(); ++k)
            res.vect[i] += m.mat[i*m.cols() + k] * u.vect[k];
    return res;
}

template<typename T>
void solve (const Matrix<T>& A, const Vector<T>& b, Vector<T>& u) {
	const size_t numGridPoints = u.size();

    //Vector<T> V1 = (b - A * u);
	double initRes = (b - A * u).l2Norm( ); // determine the initial residual
	double curRes = initRes;
	std::cout << "Initial residual:\t\t" << initRes << std::endl;

	unsigned int curIt = 0; // store the current iteration index

	while (curRes > 1.e-5 * initRes) { // solve until the residual is reduced by a certain amount
		++curIt;

		Matrix<T> A1 = A.inverseDiagonal( );
		Vector<T> V1 = b-A*u;
//		std::cout<<"A1:"<<std::endl <<A1 <<"======" <<std::endl;
//		std::cout<<"b:" <<std::endl <<b <<"======" <<std::endl;
//        std::cout<<"V1:"<<std::endl <<V1 <<"======" <<std::endl;

		u += A.inverseDiagonal( ) * (b - A * u); // Jacobi step
		//u += A1*V1;

//        std::cout<<u <<"======" <<std::endl;
//
//        Vector<T> V2 = A*u;
//        std::cout<<V2 <<"======" <<std::endl;
//        std::cout<< (b-V2) <<"======" <<std::endl;

		curRes = (b - A * u).l2Norm( ); // update the residual

		if (0 == curIt % 500) // print some info every few steps
			std::cout << "Residual after iteration " << curIt << ":\t" << curRes << std::endl;
	}

	std::cout << "Residual after iteration " << curIt << ":\t" << curRes << std::endl << std::endl; // print the final number of iterations and the final residual
}


void testFullMatrix (const int numGridPoints) {
	const double hx = 1. / (numGridPoints - 1);
	const double hxSq = hx * hx;

//	std::cout << "Starting full matrix solver for " << numGridPoints << " grid points" << std::endl;

	Matrix<double> A(numGridPoints, numGridPoints, 0.);
	Vector<double> u(numGridPoints, 0.);
	Vector<double> b(numGridPoints, 0.);

	A(0, 0) = 1.;
	for (int x = 1; x < numGridPoints - 1; ++x) {
		A(x, x - 1) = 1. / hxSq;
		A(x, x) = -2. / hxSq;
		A(x, x + 1) = 1. / hxSq;
	}
	A(numGridPoints - 1, numGridPoints - 1) = 1.;
//    std::cout << A;
//    std::cout <<std::endl <<"--------------" <<std::endl;
	for (int x = 0; x < numGridPoints; ++x) {
		b(x) = sin(2. * PI * (x / (double)(numGridPoints - 1)));
	}
//    std::cout << b;
//    std::cout <<std::endl <<"--------------" <<std::endl;


	std::cout << "Initialization complete\n";

	// TODO: start timing
    auto start = std::chrono::system_clock::now();

	solve(A, b, u);

	// TODO: end timing and print elapsed time
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}

void testStencil (const int numGridPoints)
{
    const double hx = 1. / (numGridPoints - 1);
    const double hxSq = hx * hx;
	// TODO: add stencil code
	// the stencil can be set up using
			//Stencil<double> ASten({ { 0, 1. } }, { { -1, 1. / hxSq },{ 0, -2. / hxSq },{ 1, 1. / hxSq } });

}

int main(int argc, char** argv) {
	testFullMatrix( 129 );
	//testStencil( 17 );
}
