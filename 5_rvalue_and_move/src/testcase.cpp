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

namespace copy_elison {
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

    MyStringNoMove createNonMoveable(const char* str) {
        MyStringNoMove s{str};
        cout << "createNonMoveable(): " << s.m_data << endl;
        return s;
    }

    void getNonMoveable(const char* str) {
        auto s = createNonMoveable(str);
        cout << "getNonMoveable(): " << s.m_data << endl;
        output_static_data(s);
    }
};

int main(void)
{
    /*千万级别的拷贝构造和移动构造对比*/
    long cnt = 30000000;
    test_moveable(vector<MyString>(), vector<MyStringNoMove>(), cnt);

    /*移动构造和移动赋值*/
    // MyString s1 = "hello world!";
    // MyString s2(s1);
    // MyString s3(std::move(s1));
    // MyString s4 = "hello world!";
    // MyString s5;
    // s5 = s4;
    // MyString s6;
    // s6 = std::move(s4);

    /*不完美转发*/
    // test_forward::test_forward();

    // copy elison
    //copy_elison::get("moveable string");
    //copy_elison::getNonMoveable("non-moveable string");
 	return 0;
}