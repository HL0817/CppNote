# C++2.0新特性：右值引用、移动语义和完美转发

*C++11 之前的有一个TR1版本，所以 C++11 又被称为 C++2.0， 本文主要围绕右值引用、移动语义和完美转发三个2.0新增的特性进行讨论*

## 目录
+ 右值引用
    + 左右值
    + 右值引用
    + std::move
+ 移动构造和移动拷贝
    + 移动构造
    + 移动拷贝
    + 核心代码
+ 完美转发
    + 不完美的转发
    + 完美转发
+ 例子
+ URVO和NRVO
    + copy elision
    + 什么是URVO和NRVO
    + copy elision的时机

**本文将根据c++的右值引用为大家介绍与右值引用相关的c++11新增特性。**

## 目录
+ [右值引用](#右值引用)
    + [左右值](#左右值)
    + [右值引用](#右值引用)
    + [std::move](#std::move)
+ [移动构造和移动赋值](#移动构造和移动赋值)
    + [移动构造](#移动构造)
    + [移动赋值](#移动赋值)
    + [核心代码](#核心代码)
+ [完美转发](#完美转发)
    + [不完美的转发](#不完美的转发)
    + [完美转发](#完美转发)
+ [例子](#例子)
+ [URVO和NRVO](#urvo和nrvo)
    + [copy elision](#copy-elision)
    + [什么是URVO和NRVO](#什么是urvo和nrvo)
    + [copy elision的时机](#copy-elision的时机)

## 右值引用
*Rvalue references are a new reference type introduced in C++0x that help solve the problem of unnecessary copying and enable perfect forwarding.When the right-hand side of an assignment is an rvalue, then the left-hand side object can steal resource from the right-hand side object rather than performing a saparate allocation, thus enabling move semantics.*

*右值引用是 c++0x 版本加入的新特性，用于优化一些不重要的拷贝场景和处理完美转发问题。当等式右边是一个右值的时候，左边的对象可以偷取右值的对象从而避免触发空间分配，这个操作被称为移动语义。*

### 左右值
让我们先给出左值和右值的定义：
Lvalue：**可以出现于 `operator=` 左侧**
Rvalue：**只能出现于 `operator=` 右侧**

让我们用几个例子来理解，到底什么是左右值。
以 `int` 为例：
```c++
int a = 9;
int b = 4;
int c = int();

a = b;
b = a;
a = a + b;

a + b = 42; // <-[Error]lvalue required as left operand of assignment
int() = 42; // <-[Error]lvalue required as left operand of assignment
```
我们可以看到，`a + b` 和 `int()` 这样只能出现在 `operator=` 右边的值一旦被放到左边就会产生错误，那么在这个例子中他们就被称呼为右值。
整理可得：
+ 临时变量是右值：`int()`；
+ 常量是右值：`9`、`4`；
+ 算术表达式是右值：`a + b`。
*tips：算术表达式其实也可以看成是临时变量。*

### 右值引用
首先，来了解一下常规的引用类型——左值引用（&）。
```c++
int a = 9;
int b = 4;

int& c = a; 
int& d = a + b; // <-[Error]cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'
int& e = 9; // <-[Error]cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'
int& f = int(); // <-[Error]cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'
```
使用 `operator&` 来定义的变量被称为左值引用，是**左值的引用**，它不能和右值绑定到一起。
c++推出来了对右值进行引用操作的特性——右值引用（&&），使用 `operator&&` 来定义变量。这种变量被称为右值引用，它是专门用来绑定右值的。
让我们修改上述代码：
```c++
int a = 9;
int b = 4;

int&& c = a; // <-[Error]cannot bind rvalue reference of type 'int&&' to lvalue of type 'int'
int&& d = a + b;
int&& e = 9;
int&& f = int();
```
我们可以看到，右值引用只能和右值绑定，不能去绑定左值。

### std::move
如果我们想让右值引用去绑定左值，或者想把左值转换为右值的时候该怎么办呢？——使用 `std::move`。
`std::move` 是c++11的新增特性，用于我们对左值变换为右值的需求。
```c++
int&& c = std::move(a);
```
在这里我们使用 `std::move` 成功将 `a` 转换为右值。
让我们看一下底层实现：
```c++
template<typename _Tp>
constexpr typename std::remove_reference<_Tp>::type&&
move(_Tp&& __t) noexcept {
    return static_cast<typename std::remove_reference<_Tp>::type&&>(__t);
}
```
它的核心实质上就是强制转换，将左值转换为右值。

## 移动构造和移动赋值
这一节我们将会讨论右值引用、`std::move` 这些新特性有什么用，会为我们的程序带来哪些好处。
右值引用开头引用的定义里有这么一句话：*When the right-hand side of an assignment is an rvalue, then the left-hand side object can steal resource from the right-hand side object rather than performing a saparate allocation, thus enabling move semantics.*
将右值赋值给左值的时候，不必去给左值重新开辟空间。我们让左值去偷走右边对象的资源，这样可以省略掉空间申请和赋值两个操作。
让我们来看一下例子：
```c++
class MyString
{
public:
    uint32_t m_size;
    char* m_data;

    MyString(const char* cstr = nullptr, const uint32_t size = 0);
    // copy constructor
    MyString(const MyString& str) {
        cout << "MyString copy constuctor" << endl;
        m_size = str.m_size;
        m_data = new char[m_size + 1];
        strcpy(m_data, str.m_data);
    }
    // move constructor
    MyString(MyString&& str) noexcept {
        cout << "MyString move constuctor" << endl;
        m_size = str.m_size;
        m_data = str.m_data;
        str.m_size = 0;
        str.m_data = nullptr;
    }
    MyString operator=(const MyString& str);
    virtual ~MyString();
};

int main(void)
{
    MyString s1 = "hello world!";
    MyString s2(s1);
    MyString s3(std::move(s1));
 	return 0;
}
```
结果如下：
```
MyString copy constuctor
MyString move constuctor
```
可以看到有两组不同的拷贝构造，`MyString(const MyString& str)` 和 `MyString(MyString&& str)`，我们将前者称为拷贝构造，后者称为移动构造。
拷贝构造函数是将传入的左值引用作为数据，重新 `new` 出内存空间，将左值引用赋值给新开辟的内存空间；而移动构造是将传入的右值引用的地址给拿过来直接用，让构造函数的成员直接指向右值引用对象的地址，然后将右值引用给摧毁掉。**拷贝构造需要分配空间给构造的对象，移动构造直接使用右值引用所使用的内存空间，作为代价右值就失效了。**
让我来看一下两者的效率对比：
```c++
template<typename M>
void test_moveable(M c, long& value)
{
    char buf[10];
    typedef typename iterator_traits<typename M::iterator>::value_type Vtype;
    
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i) {
        snprintf(buf, 10, "%d", rand());
        auto it = c.end();
        c.insert(it, Vtype(buf));
    }

    cout << "milli-seconds: " << (clock() - timeStart) << endl;
    cout << "c.size() = " << c.size() << endl;

    timeStart = clock();
    M c1(c);
    cout << "coyp constructor milli-seconds: " << (clock() - timeStart) << endl;

    timeStart = clock();
    M c2(std::move(c));
    cout << "move constructor milli-seconds: " << (clock() - timeStart) << endl;
}

int main(void)
{
    long cnt = 30000000;
    test_moveable(vector<MyString>(), cnt);
 	return 0;
}
```
结果如下：
```
milli-seconds: 7225
c.size() = 30000000
coyp constructor milli-seconds: 3554
move constructor milli-seconds: 0
```
移动构造几乎不消耗时间，毕竟是把已经开辟好的内存拿过来直接使用。

### 移动构造
```c++
class MyString
{
public:
    uint32_t m_size;
    char* m_data;

    MyString(const char* cstr = nullptr, const uint32_t size = 0);
    MyString(const MyString& str);
    // move constructor
    MyString(MyString&& str) noexcept {
        cout << "MyString move constuctor" << endl;
        m_size = str.m_size;
        m_data = str.m_data;
        str.m_size = 0;
        str.m_data = nullptr;
    }
    MyString& operator=(const MyString& str);
    virtual ~MyString();
};
```
最核心的步骤就是把传进来的右值的数据和数据的内存空间拿来自己用，然后将右值与内存空间的指针给置空。

### 移动赋值
```c++
class MyString
{
public:
    uint32_t m_size;
    char* m_data;

    MyString(const char* cstr = nullptr, const uint32_t size = 0);
    MyString(const MyString& str);
    MyString(MyString&& str) noexcept {...}
    MyString& operator=(const MyString& str);
    // move assignment
    MyString& operator=(MyString&& str) noexcept {
        if (this == &str) {
            return *this;
        }
        delete[] m_data;
        m_size = str.m_size;
        m_data = str.m_data;
        str.m_size = 0;
        str.m_data = nullptr;
        return *this;
    }
    virtual ~MyString();
};
```
最核心的步骤就是把传进来的右值的数据和数据的内存空间拿来自己用，然后将右值与内存空间的指针给置空。

## 完美转发
接下来让我们来了解一下，辅助右值引用、`std::move` 的新增特性—— `std::forward`，完美转发。
### 不完美转发
先让我们来了解一下什么是不完美转发：
```c++
namespace test_forward {
    void process(int& i) {
        cout << "process(int&): " << i << endl;
    }

    void process(int&& i) {
        cout << "process(int&&): " << i << endl;
    }

    void forward(int&& i) {
        cout << "forward(int&&): " << i << endl;
        process(i);
    }

    void test_forward() {
        int a = 0;
        process(a);

        process(1);

        process(move(a));

        forward(2);

        forward(move(a));
    }
};

int main(void)
{
    test_forward::test_forward();
 	return 0;
}
```
结果如下：
```
process(int&): 0
process(int&&): 1
process(int&&): 0
forward(int&&): 2
process(int&): 2
forward(int&&): 0
process(int&): 0
```
当我们直接使用左值或右值调用函数时，左值会调用到左值引用的函数，右值会调用到右值引用的函数。
而当我们给这个调用关系加一个中间层的时候，先让右值调用到右值引用的转发函数，然后在转发函数中调用处理函数，这个时候就出了问题：右值经过中转后被当做左值传入了处理函数。

### 完美转发
`std::forward` 特性就是为了解决函数的参数传递时出现的右值被当做左值进行传递的问题。
```c++
namespace test_forward {
    void process(int& i) {
        cout << "process(int&): " << i << endl;
    }

    void process(int&& i) {
        cout << "process(int&&): " << i << endl;
    }

    void forward(int&& i) {
        cout << "forward(int&&): " << i << endl;
        process(i);
    }

    void perfect_forward(int&& i) {
        cout << "perfect_forward(int&&): " << i << endl;
        process(std::forward<int>(i));
    }

    void test_forward() {
        int a = 0;
        process(a);

        process(1);

        process(move(a));

        forward(2);

        forward(move(a));

        perfect_forward(2);

        perfect_forward(move(a));
    }
};

int main(void)
{
    test_forward::test_forward();
 	return 0;
}
```
结果如下：
```
process(int&): 0 
process(int&&): 1
process(int&&): 0
forward(int&&): 2
process(int&): 2
forward(int&&): 0
process(int&): 0
perfect_forward(int&&): 2
process(int&&): 2
perfect_forward(int&&): 0
process(int&&): 0
```
我们使用 `std::forward`来告诉传递函数，我们会将值按照右值来进行传递。

### 核心代码
```c++
template<typename _Tp>
constexpr _Tp&&
forward(typename std::remove_reference<_Tp>::type& __t) noexcept
{ return static_cast<_Tp&&>(__t); }

template<typename _Tp>
constexpr _Tp&&
forward(typename std::remove_reference<_Tp>::type&& __t) noexcept
{
    static_assert(!std::is_lvalue_reference<_Tp>::value, "template argument substituting _Tp is an lvalue reference type");
    return static_cast<_Tp&&>(__t);
}
```
`std::forward` 的本质还是类型转换。

## 例子
这里贴出完整的 `MyString` 的例子，并使用容器对其进行测试。
```c++
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <cstring>
#include <vector>

using namespace std;

class MyString
{
public:
    static size_t DCtor;    // 累计 default-ctor 呼叫次数
    static size_t Ctor;     // 累计  ctor 呼叫次数
    static size_t CCtor;    // 累计 copy-ctor 呼叫次数
    static size_t MCtor;    // 累计 move-ctor 呼叫次数
    static size_t CAsgn;    // 累计 copy-asgn 呼叫次数
    static size_t MAsgn;    // 累计 move-asgn 呼叫次数
    static size_t Dtor;     // 累计 dtor 呼叫次数

    uint32_t m_size;
    char* m_data;

    MyString() : m_data(nullptr), m_size(0) { ++DCtor; }
    MyString(const char* cstr = nullptr, const uint32_t size = 0) {
        ++Ctor;
        if (cstr != nullptr) {
            m_size = strlen(cstr);
            m_data = new char[m_size + 1];
            strcpy(m_data, cstr);
        }
        else {
            m_size = 10;
            m_data = new char[m_size + 1];
            strcpy(m_data, "defaultStr");
        }
    }
    MyString(const MyString& str) {
        ++CCtor;
        m_size = str.m_size;
        m_data = new char[m_size + 1];
        strcpy(m_data, str.m_data);
    }
    MyString(MyString&& str) noexcept {
        ++MCtor;
        m_size = str.m_size;
        m_data = str.m_data;
        str.m_size = 0;
        str.m_data = nullptr;
    }
    MyString& operator=(const MyString& str) {
        ++CAsgn;
        if (this == &str) {
            return *this;
        }
        delete[] m_data;
        m_size = str.m_size;
        m_data = new char[m_size + 1];
        strcpy(m_data, str.m_data);
        return *this;
    }
    MyString& operator=(MyString&& str) noexcept {
        ++MAsgn;
        if (this == &str) {
            return *this;
        }
        delete[] m_data;
        m_size = str.m_size;
        m_data = str.m_data;
        str.m_size = 0;
        str.m_data = nullptr;
        return *this;
    }
    virtual ~MyString() {
        ++Dtor;
        delete[] m_data;
    }
};

size_t MyString::DCtor = 0;    // 累计 default-ctor 呼叫次数
size_t MyString::Ctor = 0;     // 累计  ctor 呼叫次数
size_t MyString::CCtor = 0;    // 累计 copy-ctor 呼叫次数
size_t MyString::MCtor = 0;    // 累计 move-ctor 呼叫次数
size_t MyString::CAsgn = 0;    // 累计 copy-asgn 呼叫次数
size_t MyString::MAsgn = 0;    // 累计 move-asgn 呼叫次数
size_t MyString::Dtor = 0;     // 累计 dtor 呼叫次数

class MyStringNoMove
{
public:
    static size_t DCtor;    // 累计 default-ctor 呼叫次数
    static size_t Ctor;     // 累计  ctor 呼叫次数
    static size_t CCtor;    // 累计 copy-ctor 呼叫次数
    static size_t MCtor;    // 累计 move-ctor 呼叫次数
    static size_t CAsgn;    // 累计 copy-asgn 呼叫次数
    static size_t MAsgn;    // 累计 move-asgn 呼叫次数
    static size_t Dtor;     // 累计 dtor 呼叫次数

    uint32_t m_size;
    char* m_data;

    MyStringNoMove() : m_data(nullptr), m_size(0) { ++DCtor; }
    MyStringNoMove(const char* cstr = nullptr, const uint32_t size = 0) {
        ++Ctor;
        if (cstr != nullptr) {
            m_size = strlen(cstr);
            m_data = new char[m_size + 1];
            strcpy(m_data, cstr);
        }
        else {
            m_size = 10;
            m_data = new char[m_size + 1];
            strcpy(m_data, "defaultStr");
        }
    }
    MyStringNoMove(const MyStringNoMove& str) {
        ++CCtor;
        m_size = str.m_size;
        m_data = new char[m_size + 1];
        strcpy(m_data, str.m_data);
    }
    MyStringNoMove& operator=(const MyStringNoMove& str) {
        ++CAsgn;
        if (this == &str) {
            return *this;
        }
        delete[] m_data;
        m_size = str.m_size;
        m_data = new char[m_size + 1];
        strcpy(m_data, str.m_data);
        return *this;
    }
    virtual ~MyStringNoMove() {
        ++Dtor;
        delete[] m_data;
    }
};

size_t MyStringNoMove::DCtor = 0;    // 累计 default-ctor 呼叫次数
size_t MyStringNoMove::Ctor = 0;     // 累计  ctor 呼叫次数
size_t MyStringNoMove::CCtor = 0;    // 累计 copy-ctor 呼叫次数
size_t MyStringNoMove::MCtor = 0;    // 累计 move-ctor 呼叫次数
size_t MyStringNoMove::CAsgn = 0;    // 累计 copy-asgn 呼叫次数
size_t MyStringNoMove::MAsgn = 0;    // 累计 move-asgn 呼叫次数
size_t MyStringNoMove::Dtor = 0;     // 累计 dtor 呼叫次数

#include <typeinfo>
template<typename T>
void output_static_data(const T& myStr)
{
    cout << typeid(myStr).name() << " ---- " <<endl;
    cout << "CCtor=" << T::CCtor
         << " MCtor=" << T::MCtor
         << " CAsgn=" << T::CAsgn
         << " MAsgn=" << T::MAsgn
         << " Dtor=" << T::Dtor
         << " Ctor=" << T::Ctor
         << " DCtor=" << T::DCtor
         << endl;
}

template<typename M, typename NM>
void test_moveable(M c, NM nc, long& value)
{
    char buf[10];

    cout << "==========test class with move==========" << endl;
    typedef typename iterator_traits<typename M::iterator>::value_type V1type;
    
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i) {
        snprintf(buf, 10, "%d", rand());
        auto it = c.end();
        c.insert(it, V1type(buf));
    }

    cout << "milli-seconds: " << (clock() - timeStart) << endl;
    cout << "c.size() = " << c.size() << endl;
    output_static_data(*(c.begin()));

    timeStart = clock();
    M c1(c);
    cout << "coyp constructor milli-seconds: " << (clock() - timeStart) << endl;

    timeStart = clock();
    M c2(std::move(c));
    cout << "move constructor milli-seconds: " << (clock() - timeStart) << endl;

    cout << endl;
    cout << "==========test class no move==========" << endl;
    typedef typename iterator_traits<typename NM::iterator>::value_type V2type;
    
    timeStart = clock();
    for (long i = 0; i < value; ++i) {
        snprintf(buf, 10, "%d", rand());
        auto it = nc.end();
        nc.insert(it, V2type(buf));
    }

    cout << "milli-seconds: " << (clock() - timeStart) << endl;
    cout << "nc.size() = " << nc.size() << endl;
    output_static_data(*(nc.begin()));

    timeStart = clock();
    NM nc1(nc);
    cout << "coyp constructor milli-seconds: " << (clock() - timeStart) << endl;
}

int main(void)
{
    long cnt = 30000000;
    test_moveable(vector<MyString>(), vector<MyStringNoMove>(), cnt);
 	return 0;
}
```
结果如下：
```
==========test class with move==========
milli-seconds: 7377
c.size() = 30000000
8MyString ----
CCtor=0 MCtor=63554431 CAsgn=0 MAsgn=0 Dtor=63554431 Ctor=30000000 DCtor=0
coyp constructor milli-seconds: 3541  
move constructor milli-seconds: 1     

==========test class no move==========
milli-seconds: 15329
nc.size() = 30000000
14MyStringNoMove ----
CCtor=63554431 MCtor=0 CAsgn=0 MAsgn=0 Dtor=63554431 Ctor=30000000 DCtor=0
coyp constructor milli-seconds: 3590
```
为什么拷贝构造和移动构造会被调用，并且次数大于普通构造函数的调用次数
答：`vector` 会发生容量成长的“reallocation”操作，会发生大量的拷贝，这时候就会调用拷贝构造或者移动构造，可以通过耗时看出移动构造明显更胜一筹。

## URVO和NRVO

### copy elision
编程时经常会写一种叫做 named constructor 的函数，这种函数的返回值是某个类的实例，其实本质上就是一种构造函数，但是因为可能需要在构建时执行一些其他的步骤，所以没有写成constructor的形式。比如：
```c++
MyString create(const char* str) {
    MyString s{str};
    cout << "create(): " << s.m_data << endl;
    return s;
}

void get(const char* str) {
    auto s = create(str);
    cout << "get(): " << s.m_data << endl;
    output_static_data(s);
}
```
这里的 `create` 就是一个 named constructor。

我们已经了解的 `std::move` 和 右值引用，肯定会觉得这里写得不够好，这里有太多次的拷贝发生在实例返回的时候，可以使用右值引用思想来优化它，减少拷贝次数。 但事实上，编译器比我们想的要更聪明一些，他会在这个时候把 `create::s` 直接创建到 `get::s` 里，做到只调用一次构造函数，完全不调用拷贝相关的操作。这种编译器主动进行的优化被叫做——copy elision。
让我们看一下copy elision在这里起到的作用：
```
get(): moveable string
8MyString ----
CCtor=0 MCtor=0 CAsgn=0 MAsgn=0 Dtor=0 Ctor=1 DCtor=0
```
神奇的只调用了一次构造函数，帮我们避免了大量的浪费。
那什么时候编译器会去做copy elision呢？——当这个返回值类型是URVO或NRVO时，就会触发copy elision。

### 什么是URVO和NRVO
URVO（Unnamed Return Value Optimization)：函数的所有执行路径都返回同一个类型的匿名变量。
```c++
MyString create(const char* str) {
    if (...)
        return s("if");
    else if (...)
        return s("else if");
    else
        return s("else");
}
```
NRVO（Named Return Value Optimization）：函数的所有路径都返回同一个非匿名变量。
```c++
MyString create(const char* str) {
    MyString s{str};
    if (...)
        return s;
    else if (...)
        return s;
    else
        return s;
}
```

### copy elision的时机
URVO 和 NRVO所返回的都是同一个类型的实例，编译器一定会触发copy elision，对代码进行优化。
其他时候，据需要我们手动进行 `std::move` 来减少返回值的拷贝情况了。