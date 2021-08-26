#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;

int g_uninit_para;
int g_inited_para = 1;

static int g_static_uninit_para;
static int g_static_inited_para = 2;

const int g_const_para = 3;
const char* g_const_point_to_chars = "abcdefg";
const string g_const_string = "hijklmn";

int* g_uninit_point;
int* g_inited_point_to_para = &g_inited_para;
int* g_inited_point_to_static_para = &g_static_inited_para;

int* g_inited_point_to_heap_para0 = new int(4);
int* g_inited_point_to_heap_para1 = new int(5);
int* g_inited_point_to_heap_para2 = new int(6);
int* g_inited_point_to_heap_para3 = new int(7);

int main(void)
{
	int m_uninit_para;
	int m_inited_para = 8;

	static int m_static_uninit_para;
	static int m_static_inited_para = 9;

	const int m_const_para = 10;
	const char* m_const_point_to_chars = "abcdefg";
	const string m_const_string = "hijklmn";

	int* m_uninit_point;
    int* m_inited_point_to_para = &m_inited_para;
	int* m_inited_point_to_static_para = &m_static_inited_para;

	int* m_inited_point_to_heap_para0 = new int(11);
	int* m_inited_point_to_heap_para1 = new int(12);
	int* m_inited_point_to_heap_para2 = new int(13);
	int* m_inited_point_to_heap_para3 = new int(14);

	cout << "=======================global=======================" << endl;

	cout << "g_uninit_para = " << g_uninit_para << " " << "&g_uninit_para = " << &g_uninit_para << endl;
	cout << "g_inited_para = " << g_inited_para << " " << "&g_inited_para = " << &g_inited_para << endl;

	cout << "g_static_uninit_para = " << g_static_uninit_para << " " << "&g_static_uninit_para = " << &g_static_uninit_para << endl;
	cout << "g_static_inited_para = " << g_static_inited_para << " " << "&g_static_inited_para = " << &g_static_inited_para << endl;

	cout << "g_const_para = " << g_const_para << " " << "&g_const_para = " << &g_const_para << endl;
	cout << "g_const_point_to_chars = " << g_const_point_to_chars << " " << "&g_const_point_to_chars = " << &g_const_point_to_chars << endl;
	cout << "g_const_string = " << g_const_string << " " << "&g_const_string = " << &g_const_string << endl;
	
	cout << "g_uninit_point = " << g_uninit_point << " " << "*g_uninit_point = " << "nullptr" << " " << "&g_uninit_point = " << &g_uninit_point << endl;
    cout << "g_inited_point_to_para = " << g_inited_point_to_para << " " << "*g_inited_point_to_para = " << *g_inited_point_to_para << " " << "&g_inited_point_to_para = " << &g_inited_point_to_para << endl;
	cout << "g_inited_point_to_static_para = " << g_inited_point_to_static_para << " " << "*g_inited_point_to_static_para = " << *g_inited_point_to_static_para << " " << "&g_inited_point_to_static_para = " << &g_inited_point_to_static_para << endl;

	cout << "g_inited_point_to_heap_para0 = " << g_inited_point_to_heap_para0 << " " << "*g_inited_point_to_heap_para0 = " << *g_inited_point_to_heap_para0 << " " << "&g_inited_point_to_heap_para0 = " << &g_inited_point_to_heap_para0 << endl;
	cout << "g_inited_point_to_heap_para1 = " << g_inited_point_to_heap_para1 << " " << "*g_inited_point_to_heap_para1 = " << *g_inited_point_to_heap_para1 << " " << "&g_inited_point_to_heap_para1 = " << &g_inited_point_to_heap_para1 << endl;
	cout << "g_inited_point_to_heap_para2 = " << g_inited_point_to_heap_para2 << " " << "*g_inited_point_to_heap_para2 = " << *g_inited_point_to_heap_para2 << " " << "&g_inited_point_to_heap_para2 = " << &g_inited_point_to_heap_para2 << endl;
	cout << "g_inited_point_to_heap_para3 = " << g_inited_point_to_heap_para3 << " " << "*g_inited_point_to_heap_para3 = " << *g_inited_point_to_heap_para3 << " " << "&g_inited_point_to_heap_para3 = " << &g_inited_point_to_heap_para3 << endl;
	
	cout << "=======================global=======================" << endl;

	cout << "m_uninit_para = " << m_uninit_para << " " << "&m_uninit_para = " << &m_uninit_para << endl;
	cout << "m_inited_para = " << m_inited_para << " " << "&m_inited_para = " << &m_inited_para << endl;

	cout << "m_static_uninit_para = " << m_static_uninit_para << " " << "&m_static_uninit_para = " << &m_static_uninit_para << endl;
	cout << "m_static_inited_para = " << m_static_inited_para << " " << "&m_static_inited_para = " << &m_static_inited_para << endl;
	
	cout << "m_const_para = " << m_const_para << " " << "&m_const_para = " << &m_const_para << endl;
	cout << "g_const_point_to_chars = " << g_const_point_to_chars << " " << "&g_const_point_to_chars = " << &g_const_point_to_chars << endl;
	cout << "m_const_string = " << m_const_string << " " << "&m_const_string = " << &m_const_string << endl;

	cout << "m_uninit_point = " << m_uninit_point << " " << "*m_uninit_point = " << "nullptr" << " " << "&m_uninit_point = " << &m_uninit_point << endl;
    cout << "m_const_point_to_chars = " << m_const_point_to_chars << " " << "*m_const_point_to_chars = " << *m_const_point_to_chars << " " << "&m_inited_point_to_para = " << &m_inited_point_to_para << endl;
	cout << "m_inited_point_to_static_para = " << m_inited_point_to_static_para << " " << "*m_inited_point_to_static_para = " << *m_inited_point_to_static_para << " " << "&m_inited_point_to_static_para = " << &m_inited_point_to_static_para << endl;

	cout << "m_inited_point_to_heap_para0 = " << m_inited_point_to_heap_para0 << " " << "*m_inited_point_to_heap_para0 = " << *m_inited_point_to_heap_para0 << " " << "&m_inited_point_to_heap_para0 = " << &m_inited_point_to_heap_para0 << endl;
	cout << "m_inited_point_to_heap_para1 = " << m_inited_point_to_heap_para1 << " " << "*m_inited_point_to_heap_para1 = " << *m_inited_point_to_heap_para1 << " " << "&m_inited_point_to_heap_para1 = " << &m_inited_point_to_heap_para1 << endl;
	cout << "m_inited_point_to_heap_para2 = " << m_inited_point_to_heap_para2 << " " << "*m_inited_point_to_heap_para2 = " << *m_inited_point_to_heap_para2 << " " << "&m_inited_point_to_heap_para2 = " << &m_inited_point_to_heap_para2 << endl;
	cout << "m_inited_point_to_heap_para3 = " << m_inited_point_to_heap_para3 << " " << "*m_inited_point_to_heap_para3 = " << *m_inited_point_to_heap_para3 << " " << "&m_inited_point_to_heap_para3 = " << &m_inited_point_to_heap_para3 << endl;
 
 	return 0;
}