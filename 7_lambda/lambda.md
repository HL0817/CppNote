# C++11新特性——lambda

*lambda 与 std::function*

## 目录
+ lambda
    + 什么是lambda
    + lambda的基本结构
    + 例子详解
+ std::function
    + std::function简介
    + 使用说明


## 目录
+ [lambda](#lambda)
    + [什么是lambda](#什么是lambda)
    + [lambda的基本结构](#lambda的基本结构)
    + [例子详解](#例子详解)
+ [std::function](#std::function)
    + [std::function简介](#std::function简介)
    + [使用说明](#使用说明)

## lambda

### 什么是lambda
*C++11 introduced lambdas, allowing the definition of inline functionality, which can be used as a parameter or a local object. Lambdas change the way C++standard library is used. A lambda is a definition of functionaality that can be defined inside statements and expressions. Thus, you can use a lambda as an inline function.*

c++11引入了新特性——lambda，利用lambda表达式，可以方便的进行参数传递和匿名函数的定义。lambda常被用于封装算法、执行异步方法，比较适用于少量的代码。

让我们来看一下最简单的 lambda function 是什么样子的：
```c++
[]{
    std::cout << "hello lambda" << std::endl;
}
```
我们可以这样调用它：
```c++
[]{
    std::cout << "hello lambda" << std::endl;
}();
```
或者将它赋值给一个对象，然后通过对象调用它：
```c++
auto func = []{
    std::cout << "hello lambda" << std::endl;
};

func();
```

### lambda的基本结构
接下来让我们来了解一下lambda的基本结构和使用方法。

lambda structure
```
[capture list] (parameter list) mutable throwSpec -> retType {function body}
```
+ `capture list` : capture to access nonstatic outside objects inside the lambda. Static objects such as `std::cout` can be used.
    捕获外部变量列表，这里可以指定获取一些外部变量给函数主体使用，有一下几种捕获方式：
    + `[]`，表示不捕获任何外部变量；
    + `[=]`，表示以值传递的形式捕获 lambda 所在范围的所有可见的外部变量（包括lambda所在类的 this 指针），在 lambda 的函数体内不能修改捕获的变量；
    + `[&]`，表示以引用的形式捕获 lambda 所在范围的所有可见的外部变量（包括lambda所在类的 this 指针），在 lambda 的函数体内可以修改捕获的变量；
    + `[=, &a, &b]`，也可以组合使用，以值传递捕获除 a 和 b 外的其他可见外部变量，以引用传递的形式捕获 a 和 b 。
+ `parameter list` : All of them(mutable throwSpec retType) are optional, but if one of them occurs, the parentheses for the parameters manddatory.
    指示符、异常捕获和返回类型是可选项，但如果使用了他们中的任意一个或多个，参数列表都是必须要的，例如 `[=] () mutable {...}`。
+ （可选项）`mutable`，objects are passed by value, but inside the function object defined by the lambda, you have write access to the passed value.
    对象以值的形式被传入 lambda 函数对象中时，我们可以改写参数值。
    + 使用 `mutable` 之后，按值捕获的变量由 `const` 转变为非 `const`；
    + `mutable` 把我们捕获的变量存储在函数对象中，我们针对变量的修改都被应用到了函数对象中的变量上。（详见例子）
+ （可选项）`throwSpec` ： 异常设定，可以为 lambda 指定异常处理方式。
+ （可选项）`retType` ： Without any specific definition of the return type, it is deduced from the return value.
    如果没有指定返回值类型，那么编译器会自动从返回的值中推导返回值类型。
+ `function body` ： lambda 函数对象的函数主体。

### 例子详解
本章节将举几个 lambda 使用中比较特殊的例子。
#### mutable
先上例子：
```c++
#include <iostream>
using namespace std;
int main(void)
{
	int a = 0;
	int b = 10;
	auto func = [a, &b] () mutable {
		cout << "lambda print a = " << a << endl;
		cout << "lambda print b = " << b << endl;
		++a;
		++b;
	};
	a = 20;
	b = 30;
	func();
	func();
	func();
	cout << "print a = " << a << endl;
	cout << "print a = " << b << endl;
 	return 0;
}
```
结果如下：
```
lambda print a = 0
lambda print b = 30
lambda print a = 1
lambda print b = 31
lambda print a = 2
lambda print b = 32
print a = 20
print a = 33
```
其中值得注意的点：
+ `mutable` 只对值传递捕获的变量生效；
+ 一旦使用 `mutable` 捕获了一个变，那么 lambda 对象中就有一个对应的 `func.a`，lambda 所有操作都是对 `func.a`，外部的对象 `a` 没有受到影响。

#### lambda 与 functor 对比
```c++
/**********lambda**********/
vector<int> vi {5, 28, 50, 83, 70, 590, 245, 59, 24};
int x = 30;
int y = 100;
vi.erase(
    remove_if(
        vi.begin(),
        vi.end(),
        [x, y](int n){ return x < n && n < y; }),
    vi.end());
for (auto i : vi) {
    cout << i << ' '; // 5 28 590 245 24
}
/**********functor**********/
class LambdaFunctor {
public:
    LambdaFunctor(int a, int b) : m_a(a), m_b(b) {}
    bool operator() (int n)  const {
        return m_a < n && n > m_b;
    }
private:
    int m_a;
    int m_b;
};
vi.erase(
    remove_if(
        vi.begin(),
        vi.end(),
        LambdaFunctor(x, y)),
    vi.end());
```
其实这里就可以看出，lambda 较仿函数有简单快捷易用的巨大优势，这直接导致了c++对仿函数机制的慢慢删除。

## std::function
稍微扩展一下 `std::function`，lambda 可以被 `std::function` 封装。
### std::function简介
*Class that can wrap any kind of callable element (such as functions and function objects) into a copyable object, and whose type depends solely on its call signature (and not on the callable element type itself).*
`std::function` 能对可调用对象（函数和函数对象等）进行封装，将他们封装成可复制的类对象，对象类型由 `std::function` 的调用签名决定（并不是由可调用对象的类型决定）。
简单理解，`std::function` 就是代替函数指针的模板类，让我们更安全和明确的使用函数指针。
c++中的可调用对象有一下这几种：
+ 函数；
+ lambda表达式；
+ 绑定表达式或其他函数对象；
+ 指向成员函数和数据成员的指针。

### 使用说明
接下来，分别针对不同的可调用对象进行举例说明。

#### function
普通函数的封装如下：
```c++
#include <functional>
using namespace std;

std::function<int(int)> func;

int TestFunc(int i)
{
	cout << "this is TestFunc" << endl;
	return i;
}

int main(void)
{
	func = TestFunc;
	auto result = func(1);
    return 0;
}
```
结果如下：
```
this is TestFunc
```

#### lambda
lambda的封装如下：
```c++
#include <functional>
using namespace std;

std::function<int(int)> func;
auto TestLambda = [](int i) {
	cout << "this is TestLambda" << endl;
	return i;
};

int main(void)
{
	func = TestLambda;
	auto result = func(1);
    return 0;
}
```
结果如下：
```
this is TestLambda
```
我们可以看到，虽然我们没有指定lambda表达式的返回值，但是可以被 `std::function` 封装。

#### functor
functor的封装如下：
```c++
#include <functional>
using namespace std;

std::function<int(int)> func;

class TestFunctor {
public:
	int operator() (int i) {
		cout << "this is TestFunctor" << endl;
		return i;
	}
};

int main(void)
{
	TestFunctor functor;
	func = functor;
	auto result = func(1);
    return 0;
}
```
结果如下：
```
this is TestFunctor
```

#### classmember
类成员函数的封装如下：
```c++
#include <functional>
using namespace std;

std::function<int(int)> func;

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
	TestClassMember myClass;
	func = std::bind(&TestClassMember::ClassFunc, &myClass, std::placeholders::_1);
	auto result = func(1);
	func = TestClassMember::ClassStaticFunc;
	result = func(1);
    return 0;
}
```
结果如下：
```
this is ClassFunc      
this is ClassStaticFunc
```
`std::bing` 在这里不展开，后面如果有时间可以讲一讲。


其实从整套例子看下来，我们发现不同的可调用实体（函数，仿函数等）被统一封装成了统一类型的对象。
通过这个统一类型的对象，我们可以调用到不同的可调用实体，这种功能是什么？——多态！所以，我也把这种东西称为c++面向对象的多态特性。