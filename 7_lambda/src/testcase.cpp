#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <cstring>
#include <vector>
#include <functional>

using namespace std;

std::function<int(int)> func;

int TestFunc(int i)
{
	cout << "this is TestFunc" << endl;
	return i;
}

auto TestLambda = [](int i) {
	cout << "this is TestLambda" << endl;
	return i;
};

class TestFunctor {
public:
	int operator() (int i) {
		cout << "this is TestFunctor" << endl;
		return i;
	}
};

class TestClassMember {
public:
	int ClassFunc(int i) {
		cout << "this is ClassFunc" << endl;
		return i;
	}
	static int ClassStaticFunc(int i) {
		cout << "this is ClassStaticFunc" << endl;
		return i;
	}
};

int main(void)
{
	func = TestFunc;
	auto result = func(1);

	func = TestLambda;
	result = func(1);

	TestFunctor functor;
	func = functor;
	result = func(1);

	TestClassMember myClass;
	func = std::bind(&TestClassMember::ClassFunc, &myClass, std::placeholders::_1);
	result = func(1);
	func = TestClassMember::ClassStaticFunc;
	result = func(1);

	return 0;
}