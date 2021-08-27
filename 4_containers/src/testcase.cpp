#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <time.h>
#include <string>

using namespace std;
/********************STL六大部件工作演示函数********************/
void STLComponentsTest()
{
    int ia[6] = {27, 210, 12, 47, 109, 83};
    vector<int, allocator<int>> vi(ia, ia + 6);
    cout << count_if(vi.begin(), vi.end(), [&](int i){ return i < 40; }) << endl;
}

/********************容器测试辅助函数********************/
long get_a_target_long()
{
    long target = 0;
    cout << "target (0~" << RAND_MAX << "): ";
    cin >> target;
    return target;
}

string get_a_target_string()
{
    long target = 0;
    char buf[10];
    cout << "target (0~" << RAND_MAX << "): ";
    cin >> target;
    snprintf(buf, 10, "%d", target);
    return string(buf);
}

int compareLongs(const void* a, const void*b)
{
    return (*(long*)a - *(long*)b);
}

int compareStrings(const void* a, const void*b)
{
    if (*(string*)a > *(string*)b) {
        return 1;
    } else if (*(string*)a < *(string*)b) {
        return -1;
    } else {
        return 0;
    }
}

const size_t ASIZE = 1000000;

/********************list测试函数********************/
#include <list>
void test_list(long value = ASIZE)
{
    cout << "\ntest_list()...... \n";

    list<string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.push_back(string(buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "list.size() = " << c.size() << endl;
    cout << "list.max_size() = " << c.max_size() << endl;
    cout << "list.front() = " << c.front() << endl;
    cout << "list.back() = " << c.back() << endl;

    string target = get_a_target_string();
    timeStart = clock();
    auto pItem = ::find(c.begin(), c.end(), target);
    cout << "::find(), milli-seconds : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }

    timeStart = clock();
    c.sort();
    cout << "c.sort(), milli-second : " << (clock() - timeStart) << endl;
}

/********************array测试函数********************/
#include <array>
void test_array(long value = ASIZE)
{
    cout << "\ntest_array()...... \n";

    array<long, ASIZE> c;
    clock_t timeStart = clock();
    for (long i = 0; i < ASIZE; ++i)
    {
        c[i] = rand();
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "array.size() = " << c.size() << endl;
    cout << "array.front() = " << c.front() << endl;
    cout << "array.back() = " << c.back() << endl;
    cout << "array.data() = " << c.data() << endl;

    long target = get_a_target_long();
    timeStart = clock();
    qsort(c.data(), ASIZE, sizeof(long), compareLongs);
    long* pItem = (long*)bsearch(&target, c.data(), ASIZE, sizeof(long), compareLongs);
    cout << "qsort()+bsearch(), milli-seconds : " << (clock() - timeStart) << endl;
    if (pItem != NULL) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }
}

/********************vector测试函数********************/
#include <vector>
void test_vector(long value = ASIZE)
{
    cout << "\ntest_vector()...... \n";

    vector<string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.push_back(string(buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "vector.size() = " << c.size() << endl;
    cout << "vector.front() = " << c.front() << endl;
    cout << "vector.back() = " << c.back() << endl;
    cout << "vector.data() = " << c.data() << endl;
    cout << "vector.capacity() = " << c.capacity() << endl;

    string target = get_a_target_string();
    timeStart = clock();
    auto pItem = ::find(c.begin(), c.end(), target);
    cout << "::find(), milli-seconds : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }

    timeStart = clock();
    sort(c.begin(), c.end());
    string* pSortedItem = (string*)bsearch(&target, c.data(), c.size(), sizeof(string), compareStrings);
    cout << "sort()+bsearch(), milli-seconds : " << (clock() - timeStart) << endl;
    if (pSortedItem != nullptr) {
        cout << "found, " << *pSortedItem << endl;
    } else {
        cout << "not found!" << endl;
    }
}

/********************forward_list测试函数********************/
#include <forward_list>
void test_forward_list(long value = ASIZE)
{
    cout << "\ntest_forward_list()...... \n";

    forward_list<string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.push_front(string(buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "list.max_size() = " << c.max_size() << endl;
    cout << "list.front() = " << c.front() << endl;
    // cout << "list.back() = " << c.back() << endl;
    // cout << "list.size() = " << c.size() << endl;

    string target = get_a_target_string();
    timeStart = clock();
    auto pItem = ::find(c.begin(), c.end(), target);
    cout << "::find(), milli-seconds : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }

    timeStart = clock();
    c.sort();
    cout << "c.sort(), milli-second : " << (clock() - timeStart) << endl;
}

/********************deque测试函数********************/
#include <deque>
void test_deque(long value = ASIZE)
{
    cout << "\ntest_deque()...... \n";

    deque<string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.push_back(string(buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "deque.size() = " << c.size() << endl;
    cout << "deque.max_size() = " << c.max_size() << endl;
    cout << "deque.front() = " << c.front() << endl;
    cout << "deque.back() = " << c.back() << endl;

    string target = get_a_target_string();
    timeStart = clock();
    auto pItem = ::find(c.begin(), c.end(), target);
    cout << "::find(), milli-seconds : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }

    timeStart = clock();
    sort(c.begin(), c.end());
    cout << "c.sort(), milli-second : " << (clock() - timeStart) << endl;
}

/********************stack测试函数********************/
#include <stack>
void test_stack(long value = ASIZE)
{
    cout << "\ntest_stack()...... \n";

    stack<string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.push(string(buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "stack.size() = " << c.size() << endl;
    cout << "stack.top() = " << c.top() << endl;
    c.pop();
    cout << "stack.size() = " << c.size() << endl;
    cout << "stack.top() = " << c.top() << endl;
}

/********************queue测试函数********************/
#include <queue>
void test_queue(long value = ASIZE)
{
    cout << "\ntest_queue()...... \n";

    queue<string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.push(string(buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "queue.size() = " << c.size() << endl;
    cout << "queue.front() = " << c.front() << endl;
    cout << "queue.back() = " << c.back() << endl;
    c.pop();
    cout << "queue.size() = " << c.size() << endl;
    cout << "queue.front() = " << c.front() << endl;
    cout << "queue.back() = " << c.back() << endl;
}

/********************multiset测试函数********************/
#include <set>
void test_multiset(long value = ASIZE)
{
    cout << "\ntest_multiset()...... \n";

    multiset<string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.insert(string(buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "multiset.size() = " << c.size() << endl;
    cout << "multiset.max_size() = " << c.max_size() << endl;


    string target = get_a_target_string();
    timeStart = clock();
    auto pItem = ::find(c.begin(), c.end(), target);
    cout << "::find(), milli-seconds : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }

    timeStart = clock();
    pItem = c.find(target);
    cout << "c.find(), milli-second : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }
}

/********************multimap测试函数********************/
#include <map>
void test_multimap(long value = ASIZE)
{
    cout << "\ntest_multiset()...... \n";

    multimap<long, string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.insert(pair<long, string>(i, buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "multimap.size() = " << c.size() << endl;
    cout << "multimap.max_size() = " << c.max_size() << endl;


    long target = get_a_target_long();
    timeStart = clock();
    auto pItem = c.find(target);
    cout << "c.find(), milli-second : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << (*pItem).second << endl;
    } else {
        cout << "not found!" << endl;
    }
}

/********************unordered_set测试函数********************/
#include <unordered_set>
void test_unordered_set(long value = ASIZE)
{
    cout << "\ntest_unordered_set()...... \n";

    unordered_set<string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.insert(string(buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "unordered_set.size() = " << c.size() << endl;
    cout << "unordered_set.max_size() = " << c.max_size() << endl;
    cout << "unordered_set.bucket_count() = " << c.bucket_count() << endl;
    cout << "unordered_set.load_factor() = " << c.load_factor() << endl;
    cout << "unordered_set.max_load_factor() = " << c.max_load_factor() << endl;
    cout << "unordered_set.max_bucket_count() = " << c.max_bucket_count() << endl;
    for (unsigned i = 0; i < 20; ++i) {
        cout << "bucket #" << i << "has " << c.bucket_size(i) << " elements.\n";
    }
    
    string target = get_a_target_string();
    timeStart = clock();
    auto pItem = ::find(c.begin(), c.end(), target);
    cout << "::find(), milli-seconds : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }

    timeStart = clock();
    pItem = c.find(target);
    cout << "c.find(), milli-second : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << *pItem << endl;
    } else {
        cout << "not found!" << endl;
    }
}

/********************unordered_map测试函数********************/
#include <unordered_map>
void test_unordered_map(long value = ASIZE)
{
    cout << "\ntest_unordered_map()...... \n";

    unordered_map<long, string> c;
    char buf[10];
    clock_t timeStart = clock();
    for (long i = 0; i < value; ++i)
    {
        try {
            snprintf(buf, 10, "%d", rand());
            c.insert(pair<long, string>(i, buf));
        }
        catch(exception& p) {
            cout << "i=" << i << " " << p.what() << endl;
            abort();
        }
    }
    
    cout << "milli-seconds : " << (clock() - timeStart) << endl;
    cout << "unordered_map.size() = " << c.size() << endl;
    cout << "unordered_map.max_size() = " << c.max_size() << endl;
    cout << "unordered_map.bucket_count() = " << c.bucket_count() << endl;
    cout << "unordered_map.load_factor() = " << c.load_factor() << endl;
    cout << "unordered_map.max_load_factor() = " << c.max_load_factor() << endl;
    cout << "unordered_map.max_bucket_count() = " << c.max_bucket_count() << endl;
    for (unsigned i = 0; i < 20; ++i) {
        cout << "bucket #" << i << "has " << c.bucket_size(i) << " elements.\n";
    }
    
    long target = get_a_target_long();
    timeStart = clock();
    auto pItem = c.find(target);
    cout << "c.find(), milli-second : " << (clock() - timeStart) << endl;
    if (pItem != c.end()) {
        cout << "found, " << (*pItem).second << endl;
    } else {
        cout << "not found!" << endl;
    }
}

int main(void)
{
    // STLComponentsTest();
    // test_list();
    // test_array();
    // test_vector();
    // test_forward_list();
    // test_deque();
    // test_stack(300000);
    // test_queue(300000);
    // test_multiset();
    // test_multimap();
    // test_unordered_set();
    test_unordered_map();
 	return 0;
}