/**
 * @file type_traits.h
 * @author zx
 * @brief 萃取迭代器型别的特性，帮助对构造、析构、拷贝等操作的效率最大化
 * @version 0.1
 * @date 2025-06-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _MYSTL_TYPE_TRAITS_H_
#define _MYSTL_TYPE_TRAITS_H_

#include<type_traits>

namespace MYSTL
{   
    //判断真假传会的结构，用来标注真假
    struct __true_type {
    };

    struct __false_type {
    };

    //保守定义为__true_type之后，在针对每一个标量型别特化
    template <class _Tp>
    struct __type_traits { 
    typedef __true_type     this_dummy_member_must_be_first;//占位
    typedef __false_type    has_trivial_default_constructor;//对象是否平凡构造函数
    typedef __false_type    has_trivial_copy_constructor;//对象是否拥有拷贝构造函数
    typedef __false_type    has_trivial_assignment_operator;
    typedef __false_type    has_trivial_destructor;//析构函数
    typedef __false_type    is_POD_type;//是否是POD类型
    };


    #ifndef __STL_NO_BOOL

    __STL_TEMPLATE_NULL struct __type_traits<bool> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    #endif /* __STL_NO_BOOL */
    //char类型的全特化
    __STL_TEMPLATE_NULL struct __type_traits<char> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };
    //signed char的全特化
    __STL_TEMPLATE_NULL struct __type_traits<signed char> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };
    //unsigned char的全特化
    __STL_TEMPLATE_NULL struct __type_traits<unsigned char> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    #ifdef __STL_HAS_WCHAR_T

    __STL_TEMPLATE_NULL struct __type_traits<wchar_t> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    #endif /* __STL_HAS_WCHAR_T */
    //针对整数类型的特化：
    //1.有符号整数：short、int、long
    //2.无符号整数:unsigned short、unsigned int、unsigned long
    __STL_TEMPLATE_NULL struct __type_traits<short> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<unsigned short> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<int> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<unsigned int> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<long> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<unsigned long> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    #ifdef __STL_LONG_LONG

    __STL_TEMPLATE_NULL struct __type_traits<long long> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<unsigned long long> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    #endif /* __STL_LONG_LONG */
    //浮点数类型的特化
    //1. float、double 、long double 
    __STL_TEMPLATE_NULL struct __type_traits<float> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<double> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<long double> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    #ifdef __STL_CLASS_PARTIAL_SPECIALIZATION

    template <class _Tp>
    struct __type_traits<_Tp*> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    #else /* __STL_CLASS_PARTIAL_SPECIALIZATION */


    //这六个结构体是对__type_traits模板的全特化，分别针对以下指针类型：
    //普通指针：char*、signed char*、unsigned char*
    //常量指针：const char*、const signed char*、const unsigned char*
    __STL_TEMPLATE_NULL struct __type_traits<char*> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<signed char*> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<unsigned char*> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<const char*> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<const signed char*> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    __STL_TEMPLATE_NULL struct __type_traits<const unsigned char*> {
    typedef __true_type    has_trivial_default_constructor;
    typedef __true_type    has_trivial_copy_constructor;
    typedef __true_type    has_trivial_assignment_operator;
    typedef __true_type    has_trivial_destructor;
    typedef __true_type    is_POD_type;
    };

    #endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */


    // The following could be written in terms of numeric_limits.  
    // We're doing it separately to reduce the number of dependencies.

    template <class _Tp> struct _Is_integer {
    typedef __false_type _Integral;
    };

    #ifndef __STL_NO_BOOL

    __STL_TEMPLATE_NULL struct _Is_integer<bool> {
    typedef __true_type _Integral;
    };

    #endif /* __STL_NO_BOOL */

    __STL_TEMPLATE_NULL struct _Is_integer<char> {
    typedef __true_type _Integral;
    };

    __STL_TEMPLATE_NULL struct _Is_integer<signed char> {
    typedef __true_type _Integral;
    };

    __STL_TEMPLATE_NULL struct _Is_integer<unsigned char> {
    typedef __true_type _Integral;
    };

    #ifdef __STL_HAS_WCHAR_T

    __STL_TEMPLATE_NULL struct _Is_integer<wchar_t> {
    typedef __true_type _Integral;
    };

    #endif /* __STL_HAS_WCHAR_T */

    __STL_TEMPLATE_NULL struct _Is_integer<short> {
    typedef __true_type _Integral;
    };

    __STL_TEMPLATE_NULL struct _Is_integer<unsigned short> {
    typedef __true_type _Integral;
    };

    __STL_TEMPLATE_NULL struct _Is_integer<int> {
    typedef __true_type _Integral;
    };

    __STL_TEMPLATE_NULL struct _Is_integer<unsigned int> {
    typedef __true_type _Integral;
    };

    __STL_TEMPLATE_NULL struct _Is_integer<long> {
    typedef __true_type _Integral;
    };

    __STL_TEMPLATE_NULL struct _Is_integer<unsigned long> {
    typedef __true_type _Integral;
    };

    #ifdef __STL_LONG_LONG

    __STL_TEMPLATE_NULL struct _Is_integer<long long> {
    typedef __true_type _Integral;
    };

    __STL_TEMPLATE_NULL struct _Is_integer<unsigned long long> {
    typedef __true_type _Integral;
    };

    #endif /* __STL_LONG_LONG */


}
#endif