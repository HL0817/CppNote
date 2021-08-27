#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>

using namespace std;

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

void placement_new_test()
{
    char* buf = new char[sizeof(MyString) * 3];
    MyString* str = new(buf) MyString("str");
    MyString* str1 = new(buf + sizeof(MyString)) MyString("str1");
    MyString* str2 = new(buf + sizeof(MyString) * 2) MyString("str2");
    cout << "str = " << str->m_data << endl;
    cout << "str1 = " << str1->m_data << endl;
    cout << "str2 = " << str2->m_data << endl;
    delete str;
    cout << "str1 = " << str1->m_data << endl;
    cout << "str2 = " << str2->m_data << endl;
    delete[] buf;
}

int main(void)
{
    placement_new_test();
	return 0;
}