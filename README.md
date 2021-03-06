# CppNote

*记录c++的部分重要笔记（游戏引擎向）*

## 简介
从大学开始，C++就是笔者使用的主力编程语言，一直没有进行过系统性的笔记整理，借当前项目整理C++相关的笔记。
笔者当前在某游戏公司的前端引擎组做游戏引擎开发，对c++的要求颇高。因此，以游戏引擎开发中需要注意的c++知识点为入手方向，重新温习一遍c++并做上对应的总结笔记。

## 文件结构
文件结构示例（以内存为例）：
+ memory：内存笔记文件夹
    + images：内存笔记所用到图片
        + [...]：图片若干
    + src：内存笔记中涉及的源代码
    + [memory.md]：内存笔记

## 目录（笔记标题）
+ [C/C++程序内存结构](/1_memory/memory.md)
+ [浅谈构造、析构以及 new/delete](/2_construction_and_destruction/construction_and_destruction.md)
+ [C/C++泛型编程：模板](/3_template/template.md)
+ [STL容器详解](/4_containers/containers.md)
+ [C++2.0新特性：右值引用、移动语义和完美转发](5_rvalue_and_move/rvalue_and_move.md)
+ [智能指针浅析](/6_smart_pointer/smart_pointer.md)
+ [C++11新特性——lambda](/7_lambda/lambda.md)
+ [C++多线程编程](/8_thread/thread.md)

## 关于更新
不定期更新，大体结构如目录所示。
笔记若有错漏，还请指出。

## 关于src文件下代码
在动笔写总结文章之前，笔者会先收集整理资料，并自行编写一些测试的小用例。
这个用例仅用于测试笔者文章中的一些例子，不具有严格规范。
后续有时间，笔者会重新编写相对完善且可读性强的案例。（埋坑）