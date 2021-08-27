# 智能指针

*简单介绍几种智能指针*

## 目录
+  智能指针
    + auto_ptr
    + unique_ptr
    + shared_ptr
    + weak_ptr
+  智能指针与多线程

**智能指针浅析**

## 目录
+ [智能指针](#智能指针)
    + [auto_ptr](#auto_ptr)
    + [unique_ptr](#unique_ptr)
    + [shared_ptr](#shared_ptr)
    + [weak_ptr](#weak_ptr)

## 智能指针
智能指针不是指针，它是一种带着指针相关功能的类，pointer-like classes。它主要是被用于管理在堆上分配的内存，当它生命周期结束时，会在析构函数中释放掉申请的内存，从而防止内存泄漏。因为智能指针是类不是真正的指针，所以不能将普通指针赋值给智能指针，我们只能使用 `make` 函数或者智能指针的构造函数来传入普通指针，并在使用过程中通过 `get` 函数来获取普通指针。

### auto_ptr
`auto_ptr` 是c98提出的智能指针方案，并在c++11的时候被删除了，这是一种采用所有权方案的智能指针类。
让我们看一下例子：
```c++
void test_auto_ptr() {
    auto_ptr<string> ps1(new string("This is a test for auto_ptr"));
    auto_ptr<string> ps2;
    ps2 = ps1;
}
```
+ `new string("This is a test for auto_ptr")` 指针只能被 `ps1` 或 `ps2` 独占；
+ `ps2 = ps1` 语句将指针的所有权从 `ps1` 转移到了 `ps2`;
+ 我们不能再通过 `ps1` 去访问指针。

其实 `auto_ptr` 就只是拥有生命周期结束，自动释放内存的简单功能。接下来让我们看一下它为什么被废除，它有哪些不可接受的缺点：
+ `auto_ptr` 缺乏拷贝语义，使用所有权方案来持有指针。它独占了指针的资源，并且在每次拷贝发生时，指针也就被转移到了拷贝对象那里，导致原来的智能指针失效；
+ `auto_ptr` 的另一个缺点就是不能管理对象数组，它的析构函数只有 `delete` 一种指针释放方式，不能使用 `delete[]` 去释放数组。


### unique_ptr
`unique_ptr` 是c++11提出的用于代替原有c98 `auto_ptr` 的新型智能指针。它保留和强化了 `auto_ptr` 的所有权特性，直接采用独占的严格拥有方式去持有所需要的指针对象；同时还使用重载数组版本解决掉了 `auto_ptr` 的不能释放数组的缺点。
通过例子来体会一下独占式智能指针的优缺点：
```c++
void test_unique_ptr() {
    unique_ptr<string> sp;
    if (sp == nullptr) {
        cout << "sp is null." << endl;
    }

    unique_ptr<string> sp1(new string("This is sp1."));
    cout << "*sp1 = " << *sp1 << endl;
    // unique_ptr<string> sp2 = sp1; // [ERROR] : Could not reference

    unique_ptr<string> sp3(new unique_ptr<string>::element_type("This is sp3."));
    cout << "*sp3 = " << *sp3 << endl;
    unique_ptr<string> sp4 = make_unique<string>("This is sp4.");
    cout << "*sp4 = " << *sp4 << endl;

    string* p1 = sp1.get();
    cout << "*p1 = " << *p1 << endl;

    sp3.reset();
    if (sp3 == nullptr) {
        cout << "sp3 is null." << endl;
    }

    string* p4 = sp4.release();
    if (sp4 == nullptr) {
        cout << "sp4 is null." << endl;
    }

    unique_ptr<string> sp5(p4);
    unique_ptr<string> sp6;
    sp6 = std::move(sp5);
    if (sp5 == nullptr) {
        cout << "sp5 is null." << endl;
    }
    cout << "*sp6 = " << *sp6 << endl;
}
```
结果如下：
```
sp is null.        
*sp1 = This is sp1.
*sp3 = This is sp3.
*sp4 = This is sp4.
*p1 = This is sp1.    
sp3 is null.       
sp4 is null.       
sp5 is null.       
*sp6 = This is sp4.
```

`unique_ptr` 的创建方式：
+ `unique_ptr<string> sp1(new string("This is sp1."));` 构造函数创建；
+ `unique_ptr<string> sp3(new unique_ptr<string>::element_type("This is sp3."));` 萃取出类型让调用构造函数；
+ `unique_ptr<string> sp4 = make_unique<string>("This is sp4.");` c++14新加入的 `make` 函数创建。

`unique_ptr` 获取原始指针：
+ `string* p1 = sp1.get();`

`unique_ptr` 主动释放指针的资源：
+ `sp3.reset();` ，这个操作把智能指针给置空并释放掉了原来的资源，后续可以重新使用这个空智能指针。

`unique_ptr` 放弃对指针资源的所有权：
+ `string* p4 = sp4.release();` ，将智能指针置空并返回原有的指针资源另作他用。

`unique_ptr` 转移对指针的所有权：
+ `sp6 = std::move(sp5);` ，将智能指针置空并把指针资源的所有权转交给另一个智能指针。


### shared_ptr
`shared_ptr` 是c++11提出的用于代替原有c98 `auto_ptr` 的新型智能指针。它舍弃了对指针的独占，实现了多个智能指针对一个指针资源的共享式拥有。`shared_ptr` 本质上是计数指针，它采用计数机制来表明被共享的对象资源正被几个指针共享。当我们释放 `shared_ptr` 时，当前指针会释放对资源的所有权，并将指针的资源引用计数减一；如果计数减为0的时候，资源就会被释放。
通过例子来体会一下共享式智能指针的优缺点：
```c++
struct Base
{
    Base() { std::cout << "Base::Base()\n"; }
    ~Base() { std::cout << "Base::~Base()\n"; }
};

struct Derived: public Base
{
    Derived() { std::cout << "Derived::Derived()\n"; }
    ~Derived() { std::cout << "Derived::~Derived()\n"; }
};

void test_shared_ptr() {
    cout << "==========test create==========" << endl;
    // std::shared_ptr<Base> p1 = std::make_shared<Derived>();
    std::shared_ptr<Base> p1(new Derived());
    std::shared_ptr<Base> p2 = p1;

    cout << "==========test auto release==========" << endl;
    {
        auto p3 = p1;
        cout << "p1.get() = " << p1.get() << ", p1.use_count() = " << p1.use_count() << endl;
    }
    cout << "p1.get() = " << p1.get() << ", p1.use_count() = " << p1.use_count() << endl;

    cout << "==========test reset()==========" << endl;
    p1.reset();
    if (p1 == nullptr) {
        cout << "p1 is null." << endl;
    }
    cout << "p2.get() = " << p2.get() << ", p2.use_count() = " << p2.use_count() << endl;

    cout << "==========test reset(new ptr)==========" << endl;
    p1 = std::make_shared<Derived>();
    cout << "p1.get() = " << p1.get() << ", p1.use_count() = " << p1.use_count() << endl;
    p1.reset(new Derived());
    cout << "p1.get() = " << p1.get() << ", p1.use_count() = " << p1.use_count() << endl;

    cout << "==========test swap()==========" << endl;
    p1.swap(p2);
    cout << "p1.get() = " << p1.get() << ", p1.use_count() = " << p1.use_count() << endl;
    cout << "p2.get() = " << p2.get() << ", p2.use_count() = " << p2.use_count() << endl;

    cout << "==========test unique()==========" << endl;
    auto p3 = p1;
    cout << "p1.get() = " << p1.get() << ", p1.use_count() = " << p1.use_count() << endl;
    cout << "p2.get() = " << p2.get() << ", p2.use_count() = " << p2.use_count() << endl;
    cout << "p1.unique() = " << p1.unique() << ", p2.unique() = " << p2.unique() << endl;
}
```
结果如下：
```
==========test create==========
Base::Base()
Derived::Derived()
==========test auto release==========
p1.get() = 0x25c1e60, p1.use_count() = 3
p1.get() = 0x25c1e60, p1.use_count() = 2
==========test reset()==========
p1 is null.
p2.get() = 0x25c1e60, p2.use_count() = 1
==========test reset(new ptr)==========
Base::Base()
Derived::Derived()
p1.get() = 0x25c1eb0, p1.use_count() = 1
Base::Base()
Derived::Derived()
Derived::~Derived()
Base::~Base()
p1.get() = 0x25c1ec0, p1.use_count() = 1
==========test swap()==========
p1.get() = 0x25c1e60, p1.use_count() = 1
p2.get() = 0x25c1ec0, p2.use_count() = 1
==========test swap()==========
p1.get() = 0x25c1e60, p1.use_count() = 2
p2.get() = 0x25c1ec0, p2.use_count() = 1
p1.unique() = 0, p2.unique() = 1
Derived::~Derived()
Base::~Base()
Derived::~Derived()
Base::~Base()
```

`shared_ptr` 的创建方式：
+ `std::shared_ptr<Base> p1(new Derived());` 构造函数创建；
+ `std::shared_ptr<Base> p2 = p1;` 拷贝构造函数创建；
+ `std::shared_ptr<Base> p1 = std::make_shared<Derived>();` c++14新加入的 `make` 函数创建。

`shared_ptr` 获取原始指针：
+ `p1.get()`

`shared_ptr` 获取引用计数：
+ `p1.use_count()`

`shared_ptr` 主动释放指针的资源：
+ `p1.reset();` ，这个操作把智能指针给置空，并将资源的引用计数减一，如果引用计数为0就释放资源，后续可以重新使用这个空智能指针；
+ `p1.reset(new Derived());` ，带参数的重置操作会使用新的资源来替换原来引用的资源，并且将资源的引用计数减一，如果引用计数为0就释放资源。

`shared_ptr` 判断当前释放独享资源：
+ `p1.unique()` ，如果当前智能指针的资源引用计数为1返回 `true`，否则就会返回 `false`。

### weak_ptr
虽然 `shared_ptr` 功能强大安全可靠，但还是有一种特殊情况会造成严重的内存泄漏问题，那就是 `shared_ptr` 的循环引用，他们之间形成的了死锁，导致不能正常的将引用计数减为0，不能将资源给释放掉。
通过例子来看一下内存泄漏是如何产生的：
```c++
class B;
class A {
public:
    shared_ptr<B> _pb;
    A() { cout << "create class A." << endl; }
    ~A() { cout << "delete class A." << endl; }
};
class B {
public:
    shared_ptr<A> _pa;
    B() { cout << "create class B." << endl; }
    ~B() { cout << "delete class B." << endl; }
};

void test_weak_ptr() {
    shared_ptr<B> pb(new B());
	shared_ptr<A> pa(new A());
	cout << "pa.get() = " << pa.get() << ", pa.use_count() = " << pa.use_count() << endl;
    cout << "pb.get() = " << pb.get() << ", pb.use_count() = " << pb.use_count() << endl;
	pb->_pa = pa;
	pa->_pb = pb;
	cout << "pa.get() = " << pa.get() << ", pa.use_count() = " << pa.use_count() << endl;
    cout << "pb.get() = " << pb.get() << ", pb.use_count() = " << pb.use_count() << endl;
}
```
结果如下：
```
create class B.
create class A.
pa.get() = 0x631ea0, pa.use_count() = 1
pb.get() = 0x631e60, pb.use_count() = 1
pa.get() = 0x631ea0, pa.use_count() = 2
pb.get() = 0x631e60, pb.use_count() = 2
```
我们可以看到，程序结束之后，A 和 B 两个类并没有调用析构函数，造成了累成泄漏。


为了解决这个问题，c++11引入了 `weak_ptr` 弱引用智能指针。`weak_ptr` 是一种对资源没有所有权的智能指针，它指向 `shared_ptr` 的管理对象，但是只提供对这个对象的访问手段，对象生命周期实际上是 `shared_ptr` 在管理。
通过例子来看一下 `weak_ptr` 是如何工作的：
```c++
class B;
class A {
public:
    shared_ptr<B> _pb;
    A() { cout << "create class A." << endl; }
    ~A() { cout << "delete class A." << endl; }
};
class B {
public:
    weak_ptr<A> _pa;
    B() { cout << "create class B." << endl; }
    ~B() { cout << "delete class B." << endl; }
};

void test_weak_ptr() {
    shared_ptr<B> pb(new B());
	shared_ptr<A> pa(new A());
	cout << "pa.get() = " << pa.get() << ", pa.use_count() = " << pa.use_count() << endl;
    cout << "pb.get() = " << pb.get() << ", pb.use_count() = " << pb.use_count() << endl;
	pb->_pa = pa;
	pa->_pb = pb;
	cout << "pa.get() = " << pa.get() << ", pa.use_count() = " << pa.use_count() << endl;
    cout << "pb.get() = " << pb.get() << ", pb.use_count() = " << pb.use_count() << endl;

    if (!pb->_pa.expired()) {
        auto spt = pb->_pa.lock();
        cout << "pa.get() = " << pa.get() << ", pa.use_count() = " << pa.use_count() << endl;
        cout << "pb.get() = " << pb.get() << ", pb.use_count() = " << pb.use_count() << endl;
    }
}
```
结果如下：
```
create class B.
create class A.
pa.get() = 0x25a1ea0, pa.use_count() = 1
pb.get() = 0x25a1e60, pb.use_count() = 1
pa.get() = 0x25a1ea0, pa.use_count() = 1
pb.get() = 0x25a1e60, pb.use_count() = 2
pa.get() = 0x2571ea0, pa.use_count() = 2
pb.get() = 0x2571e60, pb.use_count() = 2
delete class A.
delete class B.
```
我们可以看到 `weak_ptr` 不会增加资源的引用计数，它只是资源的弱引用。

`weak_ptr` 只是对资源的一种弱引用，本身并不能访问资源内的相关属性。
我们可以通过 `if (pb->_pa.expired())` 来判断资源的强引用计数是否为0，从而得知 `weak_ptr` 是否失效。
`auto spt = pb->_pa.lock();` 我们可以通过 `lock` 函数，获得一个指向 `weak_ptr` 所指资源的 `shared_ptr` 只能指针。


## 智能指针与多线程
c++20标准加入了安全的多线程智能指针接口，先埋坑，有空就填
specializes atomic operations for std::shared_ptr