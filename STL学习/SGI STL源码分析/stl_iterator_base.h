/**
 * @file stl_iterator_base.h
 * @author your name (you@domain.com)
 * @brief 主要定义了迭代器的基础架构和相关工具。
 * 这个文件是 STL 迭代器系统的基石，为所有迭代器类型提供了统一的接口和实现机制。
 * 主要定义了迭代器的基础架构和相关工具。这个文件是 STL 迭代器系统的基石，
 * 为所有迭代器类型提供了统一的接口和实现机制。
 * @version 0.1
 * @date 2025-05-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __SGI_STL_INTERNAL_ITERATOR_BASE_H
#define __SGI_STL_INTERNAL_ITERATOR_BASE_H
#include <concept_checks.h>

//迭代器标签
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

//五种迭代器类
template <class _Tp, class _Distance> struct input_iterator {
  typedef input_iterator_tag iterator_category;
  typedef _Tp                value_type;
  typedef _Distance          difference_type;
  typedef _Tp*               pointer;
  typedef _Tp&               reference;
};

struct output_iterator {
  typedef output_iterator_tag iterator_category;
  typedef void                value_type;
  typedef void                difference_type;
  typedef void                pointer;
  typedef void                reference;
};

template <class _Tp, class _Distance> struct forward_iterator {
  typedef forward_iterator_tag iterator_category;
  typedef _Tp                  value_type;
  typedef _Distance            difference_type;
  typedef _Tp*                 pointer;
  typedef _Tp&                 reference;
};

template <class _Tp, class _Distance> struct bidirectional_iterator {
  typedef bidirectional_iterator_tag iterator_category;
  typedef _Tp                        value_type;
  typedef _Distance                  difference_type;
  typedef _Tp*                       pointer;
  typedef _Tp&                       reference;
};

template <class _Tp, class _Distance> struct random_access_iterator {
  typedef random_access_iterator_tag iterator_category;
  typedef _Tp                        value_type;
  typedef _Distance                  difference_type;
  typedef _Tp*                       pointer;
  typedef _Tp&                       reference;
};

//迭代器基本类定义，提供迭代器的统一接口规范，所有自定义迭代器应继承自该类
template <class _Category, class _Tp, class _Distance = ptrdiff_t,class _Pointer = _Tp*, class _Reference = _Tp&>
struct iterator {
  typedef _Category  iterator_category;
  typedef _Tp        value_type;
  typedef _Distance  difference_type;
  typedef _Pointer   pointer;
  typedef _Reference reference;
};

//迭代器特征  萃取器

template <class _Iterator>
struct iterator_traits {
  typedef typename _Iterator::iterator_category iterator_category;
  typedef typename _Iterator::value_type        value_type;
  typedef typename _Iterator::difference_type   difference_type;
  typedef typename _Iterator::pointer           pointer;
  typedef typename _Iterator::reference         reference;
};

//原生指针的特化版本萃取器
template <class _Tp>
struct iterator_traits<_Tp*> {
  typedef random_access_iterator_tag iterator_category;
  typedef _Tp                         value_type;
  typedef ptrdiff_t                   difference_type;
  typedef _Tp*                        pointer;
  typedef _Tp&                        reference;
};
//const 指针的特化版萃取器
template <class _Tp>
struct iterator_traits<const _Tp*> {
  typedef random_access_iterator_tag iterator_category;
  typedef _Tp                         value_type;
  typedef ptrdiff_t                   difference_type;
  typedef const _Tp*                  pointer;
  typedef const _Tp&                  reference;
};

//辅助函数：获取迭代器类别、获取距离类型、值类类型
//内部实现
template <class _Iter>
inline typename iterator_traits<_Iter>::iterator_category
__iterator_category(const _Iter&)
{
  typedef typename iterator_traits<_Iter>::iterator_category _Category;
  return _Category();
}

template <class _Iter>
inline typename iterator_traits<_Iter>::difference_type*
__distance_type(const _Iter&)
{
  return static_cast<typename iterator_traits<_Iter>::difference_type*>(0);
}

template <class _Iter>
inline typename iterator_traits<_Iter>::value_type*
__value_type(const _Iter&)
{
  return static_cast<typename iterator_traits<_Iter>::value_type*>(0);
}

//外部接口
template <class _Iter>
inline typename iterator_traits<_Iter>::iterator_category
iterator_category(const _Iter& __i) { return __iterator_category(__i); }

template <class _Iter>
inline typename iterator_traits<_Iter>::difference_type*
distance_type(const _Iter& __i) { return __distance_type(__i); }

template <class _Iter>
inline typename iterator_traits<_Iter>::value_type*
value_type(const _Iter& __i) { return __value_type(__i); }


/**
 * @brief STL 迭代器特性萃取的一部分，用于在编译期快速获取迭代器的类别
 * 1. 函数模板的特化：针对下面迭代器input_iterator、output_iterator、forward_iterator、
 *      bidirectional_iterator、random_access_iterator的特化（特化迭代器但是不特化数据类型）
 * 2. 全特化：原始指针（特化版本）
 */
template <class _Tp, class _Distance> 
inline input_iterator_tag 
iterator_category(const input_iterator<_Tp, _Distance>&)
{ return input_iterator_tag(); }

inline output_iterator_tag 
iterator_category(const output_iterator&)
{ return output_iterator_tag(); }

template <class _Tp, class _Distance> 
inline forward_iterator_tag
iterator_category(const forward_iterator<_Tp, _Distance>&)
{ return forward_iterator_tag(); }

template <class _Tp, class _Distance> 
inline bidirectional_iterator_tag
iterator_category(const bidirectional_iterator<_Tp, _Distance>&)
{ return bidirectional_iterator_tag(); }

template <class _Tp, class _Distance> 
inline random_access_iterator_tag
iterator_category(const random_access_iterator<_Tp, _Distance>&)
{ return random_access_iterator_tag(); }

//原生指针的特化：原始指针被视为随即访问迭代器
template <class _Tp>
inline random_access_iterator_tag iterator_category(const _Tp*)
{ return random_access_iterator_tag(); }



/**
 * @brief 获取迭代器指向的数据类型（即value_type），并通过返回空指针的方式在编译期推导该类型
 */

template <class _Tp, class _Distance> 
inline _Tp* value_type(const input_iterator<_Tp, _Distance>&)
{ return (_Tp*)(0); }

template <class _Tp, class _Distance> 
inline _Tp* value_type(const forward_iterator<_Tp, _Distance>&)
{ return (_Tp*)(0); }

template <class _Tp, class _Distance> 
inline _Tp* value_type(const bidirectional_iterator<_Tp, _Distance>&)
{ return (_Tp*)(0); }

template <class _Tp, class _Distance> 
inline _Tp* value_type(const random_access_iterator<_Tp, _Distance>&)
{ return (_Tp*)(0); }

template <class _Tp>
inline _Tp* value_type(const _Tp*) { return (_Tp*)(0); }


/**
 * @brief 距离类型的特化（同上）
 */
template <class _Tp, class _Distance> 
inline _Distance* distance_type(const input_iterator<_Tp, _Distance>&)
{ return (_Distance*)(0);}

template <class _Tp, class _Distance> 
inline _Distance* distance_type(const forward_iterator<_Tp, _Distance>&)
{ return (_Distance*)(0);}

template <class _Tp, class _Distance> 
inline _Distance* 
distance_type(const bidirectional_iterator<_Tp, _Distance>&)
{ return (_Distance*)(0); }

template <class _Tp, class _Distance> 
inline _Distance* 
distance_type(const random_access_iterator<_Tp, _Distance>&)
{ return (_Distance*)(0);}

template <class _Tp>
inline ptrdiff_t* distance_type(const _Tp*) 
{ return (ptrdiff_t*)(0); }



/**
 * @brief ：通过标签分派机制实现STL计算两个迭代器之间的距离
 */

template <class _InputIterator, class _Distance>
inline void __distance(_InputIterator __first, _InputIterator __last,_Distance& __n, input_iterator_tag)
{
  while (__first != __last) { ++__first; ++__n; }
}

template <class _RandomAccessIterator, class _Distance>
inline void __distance(_RandomAccessIterator __first, _RandomAccessIterator __last, _Distance& __n, random_access_iterator_tag)
{
  __STL_REQUIRES(_RandomAccessIterator, _RandomAccessIterator);
  __n += __last - __first;
}

template <class _InputIterator, class _Distance>
inline void distance(_InputIterator __first, _InputIterator __last, _Distance& __n)
{
  __STL_REQUIRES(_InputIterator, _InputIterator);
  __distance(__first, __last, __n, iterator_category(__first));
}

template <class _InputIterator>
inline typename iterator_traits<_InputIterator>::difference_type
__distance(_InputIterator __first, _InputIterator __last, input_iterator_tag)
{
  typename iterator_traits<_InputIterator>::difference_type __n = 0;
  while (__first != __last) {
    ++__first; ++__n;
  }
  return __n;
}

template <class _RandomAccessIterator>
inline typename iterator_traits<_RandomAccessIterator>::difference_type
__distance(_RandomAccessIterator __first, _RandomAccessIterator __last,random_access_iterator_tag) {
  __STL_REQUIRES(_RandomAccessIterator, _RandomAccessIterator);
  return __last - __first;
}

template <class _InputIterator>
inline typename iterator_traits<_InputIterator>::difference_type
distance(_InputIterator __first, _InputIterator __last) {
  typedef typename iterator_traits<_InputIterator>::iterator_category _Category;//获取迭代器类型
  __STL_REQUIRES(_InputIterator, _InputIterator);
  return __distance(__first, __last, _Category());
}

/**
 * @brief 
 */
template <class _InputIter, class _Distance>
inline void __advance(_InputIter& __i, _Distance __n, input_iterator_tag) {
  while (__n--) ++__i;
}


template <class _BidirectionalIterator, class _Distance>
inline void __advance(_BidirectionalIterator& __i, _Distance __n, 
                      bidirectional_iterator_tag) {
  __STL_REQUIRES(_BidirectionalIterator, _BidirectionalIterator);
  if (__n >= 0)
    while (__n--) ++__i;
  else
    while (__n++) --__i;
}


template <class _RandomAccessIterator, class _Distance>
inline void __advance(_RandomAccessIterator& __i, _Distance __n, 
                      random_access_iterator_tag) {
  __STL_REQUIRES(_RandomAccessIterator, _RandomAccessIterator);
  __i += __n;
}

template <class _InputIterator, class _Distance>
inline void advance(_InputIterator& __i, _Distance __n) {
  __STL_REQUIRES(_InputIterator, _InputIterator);
  __advance(__i, __n, iterator_category(__i));
}


#endif /* __SGI_STL_INTERNAL_ITERATOR_BASE_H */