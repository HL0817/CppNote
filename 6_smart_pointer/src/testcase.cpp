#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <memory>

using namespace std;

namespace test_auto_ptr
{
    // c98的智能指针 现已被unique_ptr替代
    // void test_auto_ptr() {
    //     auto_ptr<string> ps1(new string("This is a test for auto_ptr"));
    //     auto_ptr<string> ps2;
    //     p2 = p1;
    // }
}

namespace test_unique_ptr
{
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
        cout << "*p1" << *p1 << endl;
        delete p1;

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
}

namespace test_shared_ptr
{
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

        cout << "==========test swap()==========" << endl;
        auto p3 = p1;
        cout << "p1.get() = " << p1.get() << ", p1.use_count() = " << p1.use_count() << endl;
        cout << "p2.get() = " << p2.get() << ", p2.use_count() = " << p2.use_count() << endl;
        cout << "p1.unique() = " << p1.unique() << ", p2.unique() = " << p2.unique() << endl;
    }
}

namespace test_weak_ptr
{
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
}


int main(void)
{
    // test_unique_ptr::test_unique_ptr();
    // test_shared_ptr::test_shared_ptr();
    test_weak_ptr::test_weak_ptr();
 	return 0;
}