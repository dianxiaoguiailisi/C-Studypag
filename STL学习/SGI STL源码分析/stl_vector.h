/**
 * @file stl_vector.h
 * @author zx
 * @brief vector类的实现:
 * 1. 使用#ifdef __STL_USE_STD_ALLOCATORS分为两个分支（分配器状态）
 *    1.1、_Vector_base类：使用标准分配器，又根据分配策略分为两个
 *          1.1.1、标准分配器的实现：_Vector_alloc_base，动态分配策略
 *          1.1.1、静态分配器的模板特化：_Vector_alloc_base，静态分配策略
 *    1.2、_Vector_base类：使用简单分配器
 * 3.  vector类
 * @version 0.1
 * @date 2025-05-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __SGI_STL_INTERNAL_VECTOR_H
#define __SGI_STL_INTERNAL_VECTOR_H

#include <concept_checks.h>
//vector_base类
#ifdef __STL_USE_STD_ALLOCATORS

/**
     * @brief Vector_alloc_base 基础模板类（非特化版本
     * @tparam Tp 元素类型
     * @tparam Allocator 分配器类型（支持有状态或无状态分配器）
     * @tparam Isstatic 是否为静态分配策略
     */
template <class _Tp, class _Allocator, bool _IsStatic>
class _Vector_alloc_base
{
  public:
    typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type allocator_type;//分配器类型
    
    allocator_type get_allocator() const { return _M_data_allocator; }//返回当前使用的分配器实例
    
    _Vector_alloc_base(const allocator_type& __a): _M_data_allocator(__a)/*初始化分配器*/, _M_start(0), _M_finish(0), _M_end_of_storage(0) {//构造函数}
    
  protected:
    allocator_type _M_data_allocator;//分配器实例
    _Tp* _M_start;//指向 向量首元素
    _Tp* _M_finish;//指向 最后一个有效元素的下一个位置指针
    _Tp* _M_end_of_storage;//指向 已分配内存末尾的下一个位置的指针

    /**
     * @brief 内存分配方法：通过分配器分配 __n 个 _Tp 类型对象的内存
     * 
     * @param __n 数量
     * @return _Tp* 指向分配内存的指针
     */
    _Tp* _M_allocate(size_t __n) 
    { 
      return _M_data_allocator.allocate(__n); 
    }

    /**
     * @brief 内存释放方法：通过分配器释放 __p 指向的内存，大小为 __n 个对象
     * 
     * @param __p 要释放地址的指针
     * @param __n 大小
     */
    void _M_deallocate(_Tp* __p, size_t __n) 
    { 
      if (__p) _M_data_allocator.deallocate(__p, __n); 
    }
};

/**
 * @brief 针对静态分配策略的特化版本
 * 
 * @tparam _Tp 
 * @tparam _Allocator 
 */
template <class _Tp, class _Allocator>
class _Vector_alloc_base<_Tp, _Allocator, true> 
{
  public:
    typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type allocator_type;

    allocator_type get_allocator() const { return allocator_type(); }

    _Vector_alloc_base(const allocator_type&): _M_start(0), _M_finish(0), _M_end_of_storage(0) {}
    
  protected:
    _Tp* _M_start;
    _Tp* _M_finish;
    _Tp* _M_end_of_storage;
    typedef typename _Alloc_traits<_Tp, _Allocator>::_Alloc_type _Alloc_type;

    _Tp* _M_allocate(size_t __n) { return _Alloc_type::allocate(__n); }
    void _M_deallocate(_Tp* __p, size_t __n) { _Alloc_type::deallocate(__p, __n);}
};

/**
 * @brief 使用标准分配器的_Vector_base
 * 
 * @tparam _Tp 
 * @tparam _Alloc 
 */
template <class _Tp, class _Alloc>
struct _Vector_base: public _Vector_alloc_base<_Tp, _Alloc,_Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
  typedef _Vector_alloc_base<_Tp, _Alloc, _Alloc_traits<_Tp, _Alloc>::_S_instanceless> _Base;//基类的别名
  typedef typename _Base::allocator_type allocator_type;//继承分配器类型
//构造函数
  _Vector_base(const allocator_type& __a) : _Base(__a) {}

  _Vector_base(size_t __n, const allocator_type& __a) : _Base(__a) 
  {
    _M_start = _M_allocate(__n);//调用基类的内存分配
    _M_finish = _M_start;
    _M_end_of_storage = _M_start + __n;
  }
//析构函数
  ~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }//调用基类的内存释放
};    

#else /* __STL_USE_STD_ALLOCATORS */
template <class _Tp, class _Alloc> 
class _Vector_base {
public:
//返回用户提供的分配器类型
  typedef _Alloc allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }
//构造函数
  _Vector_base(const _Alloc&)
    : _M_start(0), _M_finish(0), _M_end_of_storage(0) {}
  _Vector_base(size_t __n, const _Alloc&)
    : _M_start(0), _M_finish(0), _M_end_of_storage(0) 
  {
    _M_start = _M_allocate(__n);
    _M_finish = _M_start;
    _M_end_of_storage = _M_start + __n;
  }
//析构函数
  ~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }

protected:
  _Tp* _M_start;
  _Tp* _M_finish;
  _Tp* _M_end_of_storage;

  typedef simple_alloc<_Tp, _Alloc> _M_data_allocator;
  _Tp* _M_allocate(size_t __n)
    { return _M_data_allocator::allocate(__n); }
  void _M_deallocate(_Tp* __p, size_t __n) 
    { _M_data_allocator::deallocate(__p, __n); }
};
#endif /* __STL_USE_STD_ALLOCATORS */



template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) /* 分配器类型*/>
class vector : protected _Vector_base<_Tp, _Alloc> 
{

  __STL_CLASS_REQUIRES(_Tp, _Assignable);
//类型定义
private:
  typedef _Vector_base<_Tp, _Alloc> _Base;
public:
  typedef _Tp value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef typename _Base::allocator_type allocator_type;
//返回分配器对象，用于内存管理
  allocator_type get_allocator() const { return _Base::get_allocator(); }
//反向迭代器定义
#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION//支持模板特化
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION 显示指定 reverse_iterator 的模板参数（适配老编译器）*/
  typedef reverse_iterator<const_iterator, value_type, const_reference, difference_type>  const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type> reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:
#ifdef __STL_HAS_NAMESPACES//将基类的保护成员引入当前作用域，方便派生类使用
  using _Base::_M_allocate;
  using _Base::_M_deallocate;
  using _Base::_M_start;
  using _Base::_M_finish;
  using _Base::_M_end_of_storage;
#endif /* __STL_HAS_NAMESPACES */

//辅助函数：当push_back和insert导致内存不足，负责扩充和元素移动
protected:
  void _M_insert_aux(iterator __position, const _Tp& __x);//插入元素（带值）
  void _M_insert_aux(iterator __position);//插入默认构造函数

//迭代器相关函数
public:
//首尾迭代器
  //返回指向首元素的迭代器
  iterator begin() { return _M_start; }
  const_iterator begin() const { return _M_start; }
  //返回指向尾元素的迭代器
  iterator end() { return _M_finish; }
  const_iterator end() const { return _M_finish; }
  //反向迭代器首元素(相当于正向迭代器的"end")
  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  //反向迭代器尾元素（相当于正向迭代器的"begin"）
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
//容量大小
  size_type size() const { return size_type(end() - begin()); }
  size_type max_size() const { return size_type(-1) / sizeof(_Tp); }
  size_type capacity() const { return size_type(_M_end_of_storage - begin()); }
  bool empty() const { return begin() == end(); }
//元素访问操作
  //直接通过指针偏移移动访问元素
  reference operator[](size_type __n) { return *(begin() + __n); }
  const_reference operator[](size_type __n) const { return *(begin() + __n); }

#ifdef __STL_THROW_RANGE_ERRORS
  void _M_range_check(size_type __n) const {
    if (__n >= this->size())
      __stl_throw_range_error("vector");
  }
  //带边界检查的访问
  reference at(size_type __n)
    { _M_range_check(__n); return (*this)[__n]; }
  const_reference at(size_type __n) const
    { _M_range_check(__n); return (*this)[__n]; }
#endif /* __STL_THROW_RANGE_ERRORS */
//构造函数
  //默认构造函数
  explicit vector(const allocator_type& __a = allocator_type()) : _Base(__a) {}
  //初始化大小构造函数：分配n个元素的内存
    //使用__value初始化所有元素
  vector(size_type __n, const _Tp& __value,const allocator_type& __a = allocator_type()) : _Base(__n, __a)
  { _M_finish = uninitialized_fill_n(_M_start, __n, __value); }
    //使用默认构造的_TP()初始化元素
  explicit vector(size_type __n): _Base(__n, allocator_type())
  { _M_finish = uninitialized_fill_n(_M_start, __n, _Tp()); }
//拷贝构造函数：按原容器大小分配内存，用 uninitialized_copy 复制元素（深拷贝）
  vector(const vector<_Tp, _Alloc>& __x) : _Base(__x.size(), __x.get_allocator())
    { _M_finish = uninitialized_copy(__x.begin(), __x.end(), _M_start); }

//迭代器范围构造函数
#ifdef __STL_MEMBER_TEMPLATES
  //通过 _Is_integer 判断 _InputIterator 是否为整数类型（区分迭代器和普通数值）
  template <class _InputIterator>
  vector(_InputIterator __first, _InputIterator __last,const allocator_type& __a = allocator_type()) : _Base(__a) 
  {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_initialize_aux(__first, __last, _Integral());//_M_initialize_aux：根据类型萃取结果，选择不同的初始化方式
  }

  template <class _Integer>
  void _M_initialize_aux(_Integer __n, _Integer __value, __true_type) 
  {
    _M_start = _M_allocate(__n);
    _M_end_of_storage = _M_start + __n; 
    _M_finish = uninitialized_fill_n(_M_start, __n, __value);
  }

  template <class _InputIterator>
  void _M_initialize_aux(_InputIterator __first, _InputIterator __last,__false_type) 
  {
    _M_range_initialize(__first, __last, __ITERATOR_CATEGORY(__first));
  }

#else
  vector(const _Tp* __first, const _Tp* __last, const allocator_type& __a = allocator_type()): _Base(__last - __first, __a) 
  { _M_finish = uninitialized_copy(__first, __last, _M_start); }
#endif /* __STL_MEMBER_TEMPLATES */

//析构函数
  ~vector(){ destroy(_M_start, _M_finish); }
//赋值运算符重载
  vector<_Tp, _Alloc>& operator=(const vector<_Tp, _Alloc>& __x);
//内存管理：
  void reserve(size_type __n) {
    if (capacity() < __n) {
      const size_type __old_size = size();
      iterator __tmp = _M_allocate_and_copy(__n, _M_start, _M_finish);
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = __tmp;
      _M_finish = __tmp + __old_size;
      _M_end_of_storage = _M_start + __n;
    }
  }

  void assign(size_type __n, const _Tp& __val) { _M_fill_assign(__n, __val); }
  void _M_fill_assign(size_type __n, const _Tp& __val);

#ifdef __STL_MEMBER_TEMPLATES
  
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
    { _M_fill_assign((size_type) __n, (_Tp) __val); }

  template <class _InputIter>
  void _M_assign_dispatch(_InputIter __first, _InputIter __last, __false_type)
    { _M_assign_aux(__first, __last, __ITERATOR_CATEGORY(__first)); }

  template <class _InputIterator>
  void _M_assign_aux(_InputIterator __first, _InputIterator __last,
                     input_iterator_tag);

  template <class _ForwardIterator>
  void _M_assign_aux(_ForwardIterator __first, _ForwardIterator __last,
                     forward_iterator_tag); 

#endif /* __STL_MEMBER_TEMPLATES */


//元素操作
    //
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }
  //尾插
  void push_back(const _Tp& __x) {
    if (_M_finish != _M_end_of_storage) {
      construct(_M_finish, __x);
      ++_M_finish;
    }
    else
      _M_insert_aux(end(), __x);
  }
  void push_back() {
    if (_M_finish != _M_end_of_storage) {
      construct(_M_finish);
      ++_M_finish;
    }
    else
      _M_insert_aux(end());
  }
  //交换元素
  void swap(vector<_Tp, _Alloc>& __x) {
    __STD::swap(_M_start, __x._M_start);
    __STD::swap(_M_finish, __x._M_finish);
    __STD::swap(_M_end_of_storage, __x._M_end_of_storage);
  }
  //任意位置插入
  iterator insert(iterator __position, const _Tp& __x) {
    size_type __n = __position - begin();
    if (_M_finish != _M_end_of_storage && __position == end()) {
      construct(_M_finish, __x);
      ++_M_finish;
    }
    else
      _M_insert_aux(__position, __x);
    return begin() + __n;
  }
  iterator insert(iterator __position) {
    size_type __n = __position - begin();
    if (_M_finish != _M_end_of_storage && __position == end()) {
      construct(_M_finish);
      ++_M_finish;
    }
    else
      _M_insert_aux(__position);
    return begin() + __n;
  }
#ifdef __STL_MEMBER_TEMPLATES
  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_insert_dispatch(__pos, __first, __last, _Integral());
  }

  template <class _Integer>
  void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __val,__true_type)
  { _M_fill_insert(__pos, (size_type) __n, (_Tp) __val); }

  template <class _InputIterator>
  void _M_insert_dispatch(iterator __pos,_InputIterator __first, _InputIterator __last,__false_type) 
  {
    _M_range_insert(__pos, __first, __last, __ITERATOR_CATEGORY(__first));
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator __position,const_iterator __first, const_iterator __last);
#endif /* __STL_MEMBER_TEMPLATES */

  void insert (iterator __pos, size_type __n, const _Tp& __x){ _M_fill_insert(__pos, __n, __x); }

  void _M_fill_insert (iterator __pos, size_type __n, const _Tp& __x);

  void pop_back() {
    --_M_finish;
    destroy(_M_finish);
  }
  iterator erase(iterator __position) {
    if (__position + 1 != end())
      copy(__position + 1, _M_finish, __position);
    --_M_finish;
    destroy(_M_finish);
    return __position;
  }
  iterator erase(iterator __first, iterator __last) {
    iterator __i = copy(__last, _M_finish, __first);
    destroy(__i, _M_finish);
    _M_finish = _M_finish - (__last - __first);
    return __first;
  }

  void resize(size_type __new_size, const _Tp& __x) {
    if (__new_size < size()) 
      erase(begin() + __new_size, end());
    else
      insert(end(), __new_size - size(), __x);
  }
  void resize(size_type __new_size) { resize(__new_size, _Tp()); }
  void clear() { erase(begin(), end()); }

protected:

#ifdef __STL_MEMBER_TEMPLATES
  template <class _ForwardIterator>
  iterator _M_allocate_and_copy(size_type __n, _ForwardIterator __first, _ForwardIterator __last)
{
    iterator __result = _M_allocate(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, __result);
      return __result;
    }
    __STL_UNWIND(_M_deallocate(__result, __n));
  }
#else /* __STL_MEMBER_TEMPLATES */
  iterator _M_allocate_and_copy(size_type __n, const_iterator __first, const_iterator __last)
  {
    iterator __result = _M_allocate(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, __result);
      return __result;
    }
    __STL_UNWIND(_M_deallocate(__result, __n));
  }
#endif /* __STL_MEMBER_TEMPLATES */


#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  void _M_range_initialize(_InputIterator __first,  _InputIterator __last, input_iterator_tag)
  {
    for ( ; __first != __last; ++__first)
      push_back(*__first);
  }

  // This function is only called by the constructor. 
  template <class _ForwardIterator>
  void _M_range_initialize(_ForwardIterator __first,_ForwardIterator __last, forward_iterator_tag)
  {
    size_type __n = 0;
    distance(__first, __last, __n);
    _M_start = _M_allocate(__n);
    _M_end_of_storage = _M_start + __n;
    _M_finish = uninitialized_copy(__first, __last, _M_start);
  }

  template <class _InputIterator>
  void _M_range_insert(iterator __pos,_InputIterator __first, _InputIterator __last,
                       input_iterator_tag);

  template <class _ForwardIterator>
  void _M_range_insert(iterator __pos,_ForwardIterator __first, _ForwardIterator __last,forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */
};




template <class _Tp, class _Alloc>
inline bool operator==(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
{
  return __x.size() == __y.size() &&  equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Tp, class _Alloc>
inline bool operator<(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
{
  return lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Tp, class _Alloc>
inline void swap(vector<_Tp, _Alloc>& __x, vector<_Tp, _Alloc>& __y)
{
  __x.swap(__y);
}

template <class _Tp, class _Alloc>
inline bool
operator!=(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y) {
  return !(__x == __y);
}

template <class _Tp, class _Alloc>
inline bool
operator>(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y) {
  return __y < __x;
}

template <class _Tp, class _Alloc>
inline bool
operator<=(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y) {
  return !(__y < __x);
}

template <class _Tp, class _Alloc>
inline bool
operator>=(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y) {
  return !(__x < __y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */


template <class _Tp, class _Alloc>
vector<_Tp,_Alloc>&  vector<_Tp,_Alloc>::operator=(const vector<_Tp, _Alloc>& __x)
{
  if (&__x != this) {
    const size_type __xlen = __x.size();
    if (__xlen > capacity()) {
      iterator __tmp = _M_allocate_and_copy(__xlen, __x.begin(), __x.end());
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = __tmp;
      _M_end_of_storage = _M_start + __xlen;
    }
    else if (size() >= __xlen) {
      iterator __i = copy(__x.begin(), __x.end(), begin());
      destroy(__i, _M_finish);
    }
    else {
      copy(__x.begin(), __x.begin() + size(), _M_start);
      uninitialized_copy(__x.begin() + size(), __x.end(), _M_finish);
    }
    _M_finish = _M_start + __xlen;
  }
  return *this;
}

template <class _Tp, class _Alloc>
void vector<_Tp, _Alloc>::_M_fill_assign(size_t __n, const value_type& __val) 
{
  if (__n > capacity()) {
    vector<_Tp, _Alloc> __tmp(__n, __val, get_allocator());
    __tmp.swap(*this);
  }
  else if (__n > size()) {
    fill(begin(), end(), __val);
    _M_finish = uninitialized_fill_n(_M_finish, __n - size(), __val);
  }
  else
    erase(fill_n(begin(), __n, __val), end());
}

#ifdef __STL_MEMBER_TEMPLATES
//输入迭代器类型
template <class _Tp, class _Alloc> template <class _InputIter>
void vector<_Tp, _Alloc>::_M_assign_aux(_InputIter __first, _InputIter __last,input_iterator_tag) 
{
  iterator __cur = begin();
  //同步遍历输入迭代器和 vector，逐个赋值
  for ( ; __first != __last && __cur != end(); ++__cur, ++__first)
    *__cur = *__first;
  if (__first == __last)  erase(__cur, end());
  else
    insert(end(), __first, __last);
}
//输出迭代器类型
template <class _Tp, class _Alloc> template <class _ForwardIter>
void vector<_Tp, _Alloc>::_M_assign_aux(_ForwardIter __first, _ForwardIter __last,forward_iterator_tag) 
{
  size_type __len = 0;
  distance(__first, __last, __len);

  if (__len > capacity()) 
  {//1.输入元素数量超过当前容量
    iterator __tmp = _M_allocate_and_copy(__len, __first, __last);
    destroy(_M_start, _M_finish);
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = __tmp;
    _M_end_of_storage = _M_finish = _M_start + __len;
  }else if (size() >= __len) 
  {// 情况2：当前元素数量 ≥ 输入数量，可直接覆盖
    iterator __new_finish = copy(__first, __last, _M_start);
    destroy(__new_finish, _M_finish);
    _M_finish = __new_finish;
  }else 
  {// 情况3：当前元素数量 < 输入数量，需填充剩余元素
    _ForwardIter __mid = __first;
    advance(__mid, size());
    copy(__first, __mid, _M_start);
    _M_finish = uninitialized_copy(__mid, __last, _M_finish);
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

//在指定位置插入单个元素
template <class _Tp, class _Alloc>
void  vector<_Tp, _Alloc>::_M_insert_aux(iterator __position, const _Tp& __x)
{
  if (_M_finish != _M_end_of_storage) 
  {//1.在尾部构造一个原最后一个元素的副本
    construct(_M_finish, *(_M_finish - 1));
    ++_M_finish;
    //2.复制新元素的值
    _Tp __x_copy = __x;
    //3.将插入点的元素后移一位
    copy_backward(__position, _M_finish - 2, _M_finish - 1);
    *__position = __x_copy;
  }else 
  {//容量不足
    //1.计算新容量
    const size_type __old_size = size();
    const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
    //2.分配内存
    iterator __new_start = _M_allocate(__len);
    iterator __new_finish = __new_start;
    __STL_TRY {
      //3.复制插入点前的元素
      __new_finish = uninitialized_copy(_M_start, __position, __new_start);
      //4.在新内存插入点构造新元素
      construct(__new_finish, __x);
      ++__new_finish;
      //5.复制插入点后元素
      __new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
    }
    __STL_UNWIND((destroy(__new_start,__new_finish), 
                  _M_deallocate(__new_start,__len)));
    //6.销毁原内存元素并释放
    destroy(begin(), end());
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = __new_start;
    _M_finish = __new_finish;
    _M_end_of_storage = __new_start + __len;
  }
}

//在指定位置默认构造一个新元素（不接收值参数，而是默认构造元素）
template <class _Tp, class _Alloc>
void  vector<_Tp, _Alloc>::_M_insert_aux(iterator __position)
{
  if (_M_finish != _M_end_of_storage) {
    construct(_M_finish, *(_M_finish - 1));
    ++_M_finish;
    copy_backward(__position, _M_finish - 2, _M_finish - 1);
    *__position = _Tp();
  }else
  {
    const size_type __old_size = size();
    const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
    iterator __new_start = _M_allocate(__len);
    iterator __new_finish = __new_start;
    __STL_TRY {
      __new_finish = uninitialized_copy(_M_start, __position, __new_start);
      construct(__new_finish);
      ++__new_finish;
      __new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
    }
    __STL_UNWIND((destroy(__new_start,__new_finish), 
                  _M_deallocate(__new_start,__len)));
    destroy(begin(), end());
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = __new_start;
    _M_finish = __new_finish;
    _M_end_of_storage = __new_start + __len;
  }
}


//在__position位置插入_n个值为_x的元素
template <class _Tp, class _Alloc> 
void vector<_Tp, _Alloc>::_M_fill_insert(iterator __position, size_type __n, const _Tp& __x)
{
  if (__n != 0) 
  {
    if (size_type(_M_end_of_storage - _M_finish) >= __n/*容量足够*/) 
    {
      _Tp __x_copy = __x;
      const size_type __elems_after = _M_finish - __position;//计算插入点后的元素数量
      iterator __old_finish = _M_finish;//原始尾部位置
      if (__elems_after > __n) //插入点后元素多
      {
        uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);//复制尾部前n个元素到新位置
        _M_finish += __n;//更新尾部指针
        copy_backward(__position, __old_finish - __n, __old_finish);//将插入点到原尾部前_n个元素后移_n个位置
        fill(__position, __position + __n, __x_copy);//在插入点填充_n 个数据 x
      }else {//插入点后元素少
        uninitialized_fill_n(_M_finish, __n - __elems_after, __x_copy);//在尾部构造  __n - __elems_after 个 __x_copy
        _M_finish += __n - __elems_after;//更性尾指针
        uninitialized_copy(__position, __old_finish, _M_finish);//将插入点到原尾部的元素复制到新尾部后
        _M_finish += __elems_after;
        fill(__position, __old_finish, __x_copy);//将插入点到原尾部的元素替换成x
      }
    }else {//容量不足
      const size_type __old_size = size();        
      const size_type __len = __old_size + max(__old_size, __n);
      iterator __new_start = _M_allocate(__len);//分配新内存
      iterator __new_finish = __new_start;//初始化新尾部指针
      __STL_TRY {//异常安全体
        //1. 复制插入点前的元素
        __new_finish = uninitialized_copy(_M_start, __position, __new_start);
        //2.在新内存中构造n个x
        __new_finish = uninitialized_fill_n(__new_finish, __n, __x);
        //3.复制插入点后的元素
        __new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
      }
      __STL_UNWIND((destroy(__new_start,__new_finish), _M_deallocate(__new_start,__len)));
    //4.销毁原内存中元素并释放内存
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = __new_start;
      _M_finish = __new_finish;
      _M_end_of_storage = __new_start + __len;
    }
  }
}


//_M_range_insert内部函数
#ifdef __STL_MEMBER_TEMPLATES
//输入迭代器版本：逐个插入
template <class _Tp, class _Alloc> template <class _InputIterator>
void  vector<_Tp, _Alloc>::_M_range_insert(iterator __pos, _InputIterator __first, _InputIterator __last,input_iterator_tag)
{
  for ( ; __first != __last; ++__first) {
    __pos = insert(__pos, *__first);
    ++__pos;
  }
}
//前向迭代器（批量优化）
template <class _Tp, class _Alloc> template <class _ForwardIterator>
void vector<_Tp, _Alloc>::_M_range_insert(iterator __position,_ForwardIterator __first,_ForwardIterator __last,forward_iterator_tag)
{
  if (__first != __last) {
    size_type __n = 0;
    distance(__first, __last, __n);//计算插入元素数量
    if (size_type(_M_end_of_storage - _M_finish) >= __n) 
    {//容量足够原地插入
      const size_type __elems_after = _M_finish - __position;
      iterator __old_finish = _M_finish;
      if (__elems_after > __n) 
      {//插入点后元素多
        uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);
        _M_finish += __n;
        copy_backward(__position, __old_finish - __n, __old_finish);
        copy(__first, __last, __position);
      }else {//插入点后元素少
        _ForwardIterator __mid = __first;
        advance(__mid, __elems_after);
        uninitialized_copy(__mid, __last, _M_finish);
        _M_finish += __n - __elems_after;
        uninitialized_copy(__position, __old_finish, _M_finish);
        _M_finish += __elems_after;
        copy(__first, __mid, __position);
      }
    }else {//容量不够
      const size_type __old_size = size();
      const size_type __len = __old_size + max(__old_size, __n);
      iterator __new_start = _M_allocate(__len);
      iterator __new_finish = __new_start;
      __STL_TRY {
        __new_finish = uninitialized_copy(_M_start, __position, __new_start);
        __new_finish = uninitialized_copy(__first, __last, __new_finish);
        __new_finish
          = uninitialized_copy(__position, _M_finish, __new_finish);
      }
      __STL_UNWIND((destroy(__new_start,__new_finish), _M_deallocate(__new_start,__len)));
      //释放原内存
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = __new_start;
      _M_finish = __new_finish;
      _M_end_of_storage = __new_start + __len;
    }
  }
}

#else /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
void vector<_Tp, _Alloc>::insert(iterator __position, const_iterator __first, const_iterator __last)
{
  if (__first != __last) {
    size_type __n = 0;
    distance(__first, __last, __n);
    if (size_type(_M_end_of_storage - _M_finish) >= __n) {
      const size_type __elems_after = _M_finish - __position;
      iterator __old_finish = _M_finish;
      if (__elems_after > __n) {
        uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);
        _M_finish += __n;
        copy_backward(__position, __old_finish - __n, __old_finish);
        copy(__first, __last, __position);
      }else {
        uninitialized_copy(__first + __elems_after, __last, _M_finish);
        _M_finish += __n - __elems_after;
        uninitialized_copy(__position, __old_finish, _M_finish);
        _M_finish += __elems_after;
        copy(__first, __first + __elems_after, __position);
      }
    }
    else {
      const size_type __old_size = size();
      const size_type __len = __old_size + max(__old_size, __n);
      iterator __new_start = _M_allocate(__len);
      iterator __new_finish = __new_start;
      __STL_TRY {
        __new_finish = uninitialized_copy(_M_start, __position, __new_start);
        __new_finish = uninitialized_copy(__first, __last, __new_finish);
        __new_finish
          = uninitialized_copy(__position, _M_finish, __new_finish);
      }
      __STL_UNWIND((destroy(__new_start,__new_finish),
                    _M_deallocate(__new_start,__len)));
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = __new_start;
      _M_finish = __new_finish;
      _M_end_of_storage = __new_start + __len;
    }
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE 

#endif /* __SGI_STL_INTERNAL_VECTOR_H */

// Local Variables:
// mode:C++
// End:
