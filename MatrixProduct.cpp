
#include "Matrix.hpp"
#include <stdlib.h>
#include <limits>

using std::size_t;
int main()
{
    size_t s1, s2, s3;
    std::cout<<"Enter matrix dimension s1: ";
    std::cin>>s1;
    assert(s1>0);
    std::cout<<"Enter matrix dimension s2: ";
    std::cin>>s2;
    assert(s2>0);
    std::cout<<"Enter matrix dimension s3: ";
    std::cin>>s3;
    assert(s3>0);

    Matrix m1(s1, s2, 0);
    Matrix m2(s2, s3, 0);
    Matrix m3(s1, s3, 0);
    double e;
    std::cout<<"\n";
    for (int i=0; i<s1; i++){
        for (int j=0; j<s2; j++){
            std::cout<<"Enter the matrix1 entry("<<i<<","<<j<<"): ";
            std::cin>>e;
            // Input Type Error catcher: Ensures entry is of type double without quitting the entire program.
            if (cin.fail()){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(),'\n');
                cout<<"WRONG ENTRY TYPE: entry type should be a double"<<endl;
                cin>>e;
            }
            m1(i, j)=e;
        }
    }
    std::cout<<"\n";
    for (int i=0; i<s2; i++){
        for (int j=0; j<s3; j++){
            std::cout<<"Enter the matrix2 elements("<<i<<","<<j<<"): ";
            std::cin>>e;
            // Input Type Error catcher: Ensures entry is of type double without quitting the entire program.
            if (cin.fail()){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(),'\n');
                cout<<"WRONG ENTRY TYPE: entry type should be a double"<<endl;
                cin>>e;
            }
            m2(i, j)=e;
        }
    }

    m3 = m1 * m2;
    std::cout<<"\nm3: "<<endl;
    print(m3);
    std::cout<<"\n----additional Info----"<<endl;
    std::cout<<"m1: "<<endl;
    print(m1);
    std::cout<<"\n";
    std::cout<<"m2: "<<endl;
    print(m2);
    std::cout<<"\n-----------------------"<<endl;
}



