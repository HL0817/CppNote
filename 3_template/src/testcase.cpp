#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <bitset>
using namespace std;

template<typename Type>
class Complex
{
public:
    Complex(Type r = 0, Type i = 0) : re(r), im(i)
    {
        cout << "call default constructor" << endl;
    }
    Complex& operator+= (const Complex& );
    bool operator< (const Complex& complex) const
    {
        if(re < complex.re || (re == complex.re && im < complex.im)) {
            return true;
        }
        else {
            return false;
        }
    }
    template<typename OtherType>
    Complex(const OtherType& complex) : re(complex.Real()), im(complex.Imag())
    {
        cout << "call template constructor" << endl;
    }
    Type Real() const { return re; }
    Type Imag() const { return im; }
private:
    Type re, im;
};

template<typename Type>
class SubComplex : public Complex<Type>
{
public:
    SubComplex(Type a, Type b) : first(a) {}
    Type First() const { return first; }
private:
    Type first;
};

class SubComplexInt : public Complex<int>
{
public:
    SubComplexInt(int a, int b) : Complex<int>(a, b), first(a) {}
    int First() const { return first; }
private:
    int first;
};

template<class Type>
const Type& Min(const Type& a, const Type& b)
{
    return a < b ? a : b;
}

void print() {
    cout << "end print()" << endl;
}

template<typename T, typename... Types>
void print(const T& firstArg, const Types&... args)
{
    cout << "size of args: " << sizeof...(args) << " " << firstArg << endl;
    print(args...);
}

template<typename Type>
void printArg(Type arg)
{
    cout << arg << endl;
}

template<typename... Types>
void expand(const Types&... args)
{
    int arr[] = {(printArg(args), 0)...};
}

template<class F, class... Args>
void expand(const F& func, Args&&...args)
{
    initializer_list<int>{(func(std::forward<Args>(args)),0)...};
}

template class Complex<double>;

int main(void)
{
    Complex<double> c1(1.5, 2.5);
    Complex<int> c2(1, 2);
 	return 0;
}