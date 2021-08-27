#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>

using namespace std;

class Base
{
public:
    int para;
    int* point;
public:
    Base() {
        cout << "Base default constuctor" << endl;
    }
    Base(int i) {
        para = i;
        point = &para;
        cout << "Base one parameter constuctor" << endl;
    }
    Base(int para, int* point) : para(para), point(point) {
        cout << "Base Initialization list constuctor" << endl;
    };

    ~Base() {
        cout << "Base default destructor" << endl;
    }
    void print_class_member() {
        cout << "Base info: para = " << para 
            << " " << "point = " << point 
            << " " << "*point = " << *point << endl;
    }
};

void default_constructor()
{
    Base base0;
    Base base1 = {};
    Base base2 = Base();

    int i = 3;
    Base base3 = Base(i);

    Base base4(i, &i);
    Base base5({i, &i});
    Base base6 = {i, &i};
    base0.print_class_member();
    base1.print_class_member();
}

class MyString
{
public:
    uint32_t m_size;
    char* m_data;

    void print_class_member() {
        cout << "MyString info: m_size = " << m_size 
        << " " << "m_data = " << m_data 
        << " " << "m_data addr = " << (void*)m_data << endl;
    }

    MyString(const char* cstr = nullptr, const uint32_t size = 0) {
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
        cout << m_data << " MyString constuctor is invoked" << endl;
    }
    MyString(const MyString& str) {
        // cout << "MyString copy constuctor" << endl;
        m_size = str.m_size;
        m_data = new char[m_size + 1];
        strcpy(m_data, str.m_data);
    }
    MyString operator=(const MyString& str) {
        // cout << "MyString copy assignment" << endl;
        if (this == &str) {
            return *this;
        }
        delete[] m_data;
        m_size = str.m_size;
        m_data = new char[m_size + 1];
        strcpy(m_data, str.m_data);
        return *this;
    }
    virtual ~MyString() {
        cout << m_data << " MyString destuctor is invoked" << endl;
        delete[] m_data;
    }
};

// void copy_test()
// {
//     MyString str0("abcd");
//     MyString str1 = str0;
//     str0.m_data[0] = 'f';
//     MyString str2;
//     str2 = str0;
//     str0.m_data[0] = 'e';

//     str0.print_class_member();
//     str1.print_class_member();
//     str2.print_class_member();
// }

// // MyString g_str("global str");
// // MyString g_static_str("global static str");
// // MyString* g_point_to_str = new MyString("global point to heap str");

// void destructor_test()
// {
//     cout << "----------func start----------" << endl;
//     MyString m_local_str("local str");
//     cout << "----------func start----------" << endl;
// }

class SubMyString : public MyString
{
public:
    uint32_t m_subSize;
    char* m_subData;
    MyString m_myString;

    SubMyString(const char* cstr = nullptr, uint32_t i = 0) : MyString(cstr, i) {
        if (cstr != nullptr) {
            m_subSize = strlen(cstr);
            m_subData = new char[m_subSize + 1];
            strcpy(m_subData, cstr);
        }
        else {
            m_subSize = 0;
            m_subData = new char[m_subSize + 1];
            m_subData[0] = '\0';
        }
        cout << m_subData << " SubMyString constuctor is invoked" << endl;
    }
    ~SubMyString() {
        cout << m_subData << " SubMyString destuctor is invoked" << endl;
        delete[] m_subData;
    }
};

using func = void(*)();                         // func为函数指针类
using virtual_table = func;	                    // 虚表中保存函数指针,因此虚表的值即为函数指针
using virtual_table_pointer = virtual_table*;   // 虚表地址,虚表地址保存在类的实例中
void virtual_destructor_test()
{
    MyString* myStr = new MyString("myStr");
    SubMyString* subStr = new SubMyString("subStr");
    MyString* str = new SubMyString("str");

    cout << "str = " << static_cast<void*>(str) << endl;
    cout << "str->m_size = " << &(str->m_size) << endl;
    cout << "str->m_data = " << static_cast<void*>(str->m_data) << endl;
    cout << "str->m_subSize = " << &(dynamic_cast<SubMyString*>(str)->m_subSize) << endl;
    cout << "str->m_subData = " <<static_cast<void*>(dynamic_cast<SubMyString*>(str)->m_subData) << endl;

    void* objPointer = static_cast<void*>(str);
    func subClassVirtualDestructor, baseClassVirtualDestructor;
    virtual_table_pointer vtableAddr;
	vtableAddr = *(virtual_table_pointer*)objPointer;                   // myStr首地址内的值即为虚表的地址

	virtual_table vtable = *vtableAddr;                                 // 虚表地址取值得到函数指针地址
	subClassVirtualDestructor = **(virtual_table_pointer*)objPointer;   // 将指针类型转换为指向vtblAddr,然后进行两次解引用
	subClassVirtualDestructor = vtable;                                 // 等价于上面一行

    vtableAddr += 0x4;                                                  // 析构函数指针只占4个字节
    virtual_table vtable1 = *vtableAddr;
    baseClassVirtualDestructor = vtable1;
	cout << "-------------------------------------------" << endl;

    delete myStr;
    delete subStr;
    delete str;
}

class PrivateDestructor
{
public:
    PrivateDestructor() {}
    void destroy() const {
        cout << "destroy self" << endl;
        delete this;
    }
private:
    ~PrivateDestructor() {}
};

void private_destructor_test()
{
    PrivateDestructor* p = new PrivateDestructor();
    p->destroy();
    // PrivateDestructor pp; ERROR：不能访问私有成员函数
}

void constructor_destructor_test()
{
    SubMyString* subStr = new SubMyString("subStr");
    delete subStr;
}

void placement_new_test()
{
    char* buf = new char[sizeof(MyString) * 3];
    MyString* str = new(buf) MyString("str");
    MyString* str1 = new(buf + sizeof(MyString)) MyString("str1");
    MyString* str2 = new(buf + sizeof(MyString) * 2) MyString("str2");
    cout << "buf addr = " << (void*)buf << endl;
    cout << "str = " << str->m_data << endl;
    cout << "str1 = " << str1->m_data << endl;
    cout << "str2 = " << str2->m_data << endl;
    delete str;
    // str->~MyString();
    cout << "str1 = " << str1->m_data << endl;
    cout << "str2 = " << str2->m_data << endl;
    delete[] buf;
}

int main(void)
{
    // default_constructor();
    // copy_test();
    // virtual_destructor_test();
    // constructor_destructor_test();
    placement_new_test();
	return 0;
}