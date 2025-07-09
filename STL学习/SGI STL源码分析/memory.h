/**
 * @file memory
 * @author your name (you@domain.com)
 * @brief SGI STL库中的memory头文件内容,主要定义auto_ptr智能指针类
 * auto_ptr一个用来包装原生指针的对象
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __SGI_STL_MEMORY
#define __SGI_STL_MEMORY

#include <stl_algobase.h>
#include <stl_alloc.h>
#include <stl_construct.h>
#include <stl_tempbuf.h>
#include <stl_uninitialized.h>
#include <stl_raw_storage_iter.h>


__STL_BEGIN_NAMESPACE
//STL内部命名空间开始标记

#if defined(__SGI_STL_USE_AUTO_PTR_CONVERSIONS) && \
    defined(__STL_MEMBER_TEMPLATES)

template<class _Tp1> struct auto_ptr_ref {
  _Tp1* _M_ptr;
  auto_ptr_ref(_Tp1* __p) : _M_ptr(__p) {}
};

#endif

//定义了一个模板类auto_ptr管理动态分配的对象
template <class _Tp> class auto_ptr {
private:
  _Tp* _M_ptr;//被管理的原始指针

public:
  typedef _Tp element_type;//被管理指针对象的类型
    
  explicit auto_ptr(_Tp* __p = 0) __STL_NOTHROW : _M_ptr(__p) {
    /*构造函数：auto_ptr<int> p1(new int(42));
      1. 使用explicit防止隐式转换，只能使用显示初始化。
      2. __STL_NOTHROW:宏定义表示不会抛出异常 
      3._M_ptr(__p) :this->_M_ptr = __p,此时两个指针都管理一个地址并且为同一个对象的别名
    */
  }

  auto_ptr(auto_ptr& __a) __STL_NOTHROW : _M_ptr(__a.release()) {
    /* 拷贝构造函数：auto_ptr<int> p2(p1);  
      1. __a.release():实现了所有权的转移即原指针不再有数据的权利,这也是为什么auto_ptr会被抛弃
                        auto_ptr<int> p1(new int(42));
                        auto_ptr<int> p2(p1);  // 调用拷贝构造函数，&左值引用：__a是p1的别名
                        ---->此时： p2._M_ptr 指向new int(42) ，而 p1._M_ptr 变为nullptr（所有权已转移）
  */
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class _Tp1> auto_ptr(auto_ptr<_Tp1>& __a) __STL_NOTHROW
    : _M_ptr(__a.release()) {
      /* 
        模板拷贝构造函数：同样通过 release() 方法转移资源所有权

      */
    }
#endif /* __STL_MEMBER_TEMPLATES */


//赋值运算符重载：先释放当前对象的资源，再接管参数对象的资源
  auto_ptr& operator=(auto_ptr& __a) __STL_NOTHROW {
    if (&__a != this) {
      delete _M_ptr;
      _M_ptr = __a.release();
    }
    return *this;
  }
//模板赋值运算符重载
#ifdef __STL_MEMBER_TEMPLATES
  template <class _Tp1>
  auto_ptr& operator=(auto_ptr<_Tp1>& __a) __STL_NOTHROW {
    if (__a.get() != this->get()) {
      delete _M_ptr;
      _M_ptr = __a.release();
    }
    return *this;
  }
#endif /* __STL_MEMBER_TEMPLATES */

 //析构函数
  ~auto_ptr() { delete _M_ptr; }

//解引用重载
  _Tp& operator*() const __STL_NOTHROW {
    return *_M_ptr;
  }
//"->"运算符重载
  _Tp* operator->() const __STL_NOTHROW {
    return _M_ptr;
  }
//获得原始指针
  _Tp* get() const __STL_NOTHROW {
    return _M_ptr;
  }

//核心代码：放弃对资源的所有权并返回原始指针
  _Tp* release() __STL_NOTHROW {
    _Tp* __tmp = _M_ptr;//内部指针保存当前指针
    _M_ptr = NULL;//原始指针放弃所有权，这是origin_ptr指针放弃对地址的管理，并不是将地址直接删除
    return __tmp;
  }
//重置指针
  void reset(_Tp* __p = 0) __STL_NOTHROW {
    if (__p != _M_ptr) {
      delete _M_ptr;
      _M_ptr = __p;
    }
  }

//转换代码
  
#if defined(__SGI_STL_USE_AUTO_PTR_CONVERSIONS) && \
    defined(__STL_MEMBER_TEMPLATES)

public:
  auto_ptr(auto_ptr_ref<_Tp> __ref) __STL_NOTHROW
    : _M_ptr(__ref._M_ptr) {}

  auto_ptr& operator=(auto_ptr_ref<_Tp> __ref) __STL_NOTHROW {
    if (__ref._M_ptr != this->get()) {
      delete _M_ptr;
      _M_ptr = __ref._M_ptr;
    }
    return *this;
  }

  template <class _Tp1> operator auto_ptr_ref<_Tp1>() __STL_NOTHROW 
    { return auto_ptr_ref<_Tp1>(this->release()); }
  template <class _Tp1> operator auto_ptr<_Tp1>() __STL_NOTHROW
    { return auto_ptr<_Tp1>(this->release()); }

#endif /* auto ptr conversions && member templates */
};

__STL_END_NAMESPACE

#endif /* __SGI_STL_MEMORY */


// Local Variables:
// mode:C++
// End:
