/**
 * @file stl_alloc.h
 * @author zx
 * @brief 
 * @version 0.1
 * @date 2025-05-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __SGI_STL_INTERNAL_ALLOC_H
#define __SGI_STL_INTERNAL_ALLOC_H

#ifdef __SUNPRO_CC
#  define __PRIVATE public
   // Extra access restrictions prevent us from really making some things
   // private.
#else
#  define __PRIVATE private
#endif

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
#  define __USE_MALLOC
#endif


#ifndef __THROW_BAD_ALLOC
#  if defined(__STL_NO_BAD_ALLOC) || !defined(__STL_USE_EXCEPTIONS)
#    include <stdio.h>
#    include <stdlib.h>
#    define __THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
#  else /* Standard conforming out-of-memory handling */
#    include <new>
#    define __THROW_BAD_ALLOC throw std::bad_alloc()
#  endif
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef __RESTRICT
#  define __RESTRICT
#endif

#ifdef __STL_THREADS
# include <stl_threads.h>
# define __NODE_ALLOCATOR_THREADS true
# ifdef __STL_SGI_THREADS
    extern "C" {
      extern int __us_rsthread_malloc;
    }
#   define __NODE_ALLOCATOR_LOCK if (threads && __us_rsthread_malloc) \
                { _S_node_allocator_lock._M_acquire_lock(); }
#   define __NODE_ALLOCATOR_UNLOCK if (threads && __us_rsthread_malloc) \
                { _S_node_allocator_lock._M_release_lock(); }
# else /* !__STL_SGI_THREADS */
#   define __NODE_ALLOCATOR_LOCK \
        { if (threads) _S_node_allocator_lock._M_acquire_lock(); }
#   define __NODE_ALLOCATOR_UNLOCK \
        { if (threads) _S_node_allocator_lock._M_release_lock(); }
# endif
#else
//  Thread-unsafe
#   define __NODE_ALLOCATOR_LOCK
#   define __NODE_ALLOCATOR_UNLOCK
#   define __NODE_ALLOCATOR_THREADS 
#endif

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
# ifdef __DECLARE_GLOBALS_HERE
    void (* __malloc_alloc_oom_handler)() = 0;
    // g++ 2.7.2 does not handle static template data members.
# else
    extern void (* __malloc_alloc_oom_handler)();
# endif
#endif

//第一级分配器
template <int __inst>
class __malloc_alloc_template 
{
  private:
    static void* _S_oom_malloc(size_t);
    static void* _S_oom_realloc(void*, size_t);
  #ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
    static void (* __malloc_alloc_oom_handler)();
  #endif
  public:
    static void* allocate(size_t __n){
      void* __result = malloc(__n);
      if (0 == __result) __result = _S_oom_malloc(__n);
      return __result;
    }
    static void deallocate(void* __p, size_t /* __n */){ free(__p);}
    static void* reallocate(void* __p, size_t /* old_sz */, size_t __new_sz){
      void* __result = realloc(__p, __new_sz);
      if (0 == __result) __result = _S_oom_realloc(__p, __new_sz);
      return __result;
    }

    static void (* __set_malloc_handler(void (*__f)()))(){
      void (* __old)() = __malloc_alloc_oom_handler;
      __malloc_alloc_oom_handler = __f;
      return(__old);
    }

};

//第一级分配器内部函数实现
template <int __inst>
void* __malloc_alloc_template<__inst>::_S_oom_malloc(size_t __n)
{
    void (* __my_malloc_handler)();
    void* __result;
    for (;;) {
        __my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == __my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*__my_malloc_handler)();
        __result = malloc(__n);
        if (__result) return(__result);
    }
}

template <int __inst>
void* __malloc_alloc_template<__inst>::_S_oom_realloc(void* __p, size_t __n)
{
    void (* __my_malloc_handler)();
    void* __result;
    for (;;) {
        __my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == __my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*__my_malloc_handler)();
        __result = realloc(__p, __n);
        if (__result) return(__result);
    }
}

//将模板类__malloc_alloc_template实例化
typedef __malloc_alloc_template<0> malloc_alloc;

//适配器：类型安全的适配器
template<class _Tp, class _Alloc>
class simple_alloc 
{
  public:
      static _Tp* allocate(size_t __n){ return 0 == __n ? 0 : (_Tp*) _Alloc::allocate(__n * sizeof (_Tp)); }
      static _Tp* allocate(void){ return (_Tp*) _Alloc::allocate(sizeof (_Tp)); }
      static void deallocate(_Tp* __p, size_t __n) { if (0 != __n) _Alloc::deallocate(__p, __n * sizeof (_Tp)); }
      static void deallocate(_Tp* __p){ _Alloc::deallocate(__p, sizeof (_Tp)); }
};

//适配器：调试功能的适配器
template <class _Alloc>
class debug_alloc 
{
  private:
    enum {_S_extra = 8};//额外空间，存储额外信息

  public:
    static void* allocate(size_t __n){
      //1:分配比请求更多的内存（多出_S_extra字节
      char* result = (char*) Alloc::allocate(n+(int) S_extra);
      //2：在内存块起始处写入实际分配的大小（覆盖前_S_extra字节）
      *(size_t*) result = n;
      //3：返回真正可用的内存起始位置（跳过前_S_extra字节）
      return result+ (int) S_extra;
    }
    static void deallocate(void* __p, size_t __n)
    {
      char* __real_p = (char*)__p - (int) _S_extra;
      assert(*(size_t*)__real_p == __n);
      _Alloc::deallocate(__real_p, __n + (int) _S_extra);
    }
    static void* reallocate(void* __p, size_t __old_sz, size_t __new_sz)
  {
    char* __real_p = (char*)__p - (int) _S_extra;
    assert(*(size_t*)__real_p == __old_sz);
    char* __result = (char*)
      _Alloc::reallocate(__real_p, __old_sz + (int) _S_extra,
                                   __new_sz + (int) _S_extra);
    *(size_t*)__result = __new_sz;
    return __result + (int) _S_extra;
  }
};


# ifdef __USE_MALLOC

typedef malloc_alloc alloc;
typedef malloc_alloc single_client_alloc;

# else
#if defined(__SUNPRO_CC) || defined(__GNUC__)
// breaks if we make these template class members:
  enum {_ALIGN = 8};
  enum {_MAX_BYTES = 128};
  enum {_NFREELISTS = 16}; // _MAX_BYTES/_ALIGN
#endif

//第二级分配器
template <bool threads, int inst>
class __default_alloc_template 
{

  private:
    #if ! (defined(__SUNPRO_CC) || defined(__GNUC__))
        enum {_ALIGN = 8};
        enum {_MAX_BYTES = 128};
        enum {_NFREELISTS = 16}; 
    # endif
      static size_t _S_round_up(size_t __bytes) 
      { return (((__bytes) + (size_t) _ALIGN-1) & ~((size_t) _ALIGN - 1)); }
    __PRIVATE:
        union _Obj {
          union _Obj* _M_free_list_link;
          char _M_client_data[1];    /* The client sees this.        */
    };
  private:
    #if defined(__SUNPRO_CC) || defined(__GNUC__) || defined(__HP_aCC)
        static _Obj* __STL_VOLATILE _S_free_list[]; 
    #else
        static _Obj* __STL_VOLATILE _S_free_list[_NFREELISTS]; 
    #endif
    static  size_t _S_freelist_index(size_t __bytes) {
      return (((__bytes) + (size_t)_ALIGN-1)/(size_t)_ALIGN - 1);
    }
    static void* _S_refill(size_t __n);
    static char* _S_chunk_alloc(size_t __size, int& __nobjs);
    static char* _S_start_free;
    static char* _S_end_free;
    static size_t _S_heap_size;
  # ifdef __STL_THREADS
      static _STL_mutex_lock _S_node_allocator_lock;
  # endif
      class _Lock;
      friend class _Lock;
      class _Lock {
          public:
              _Lock() { __NODE_ALLOCATOR_LOCK; }
              ~_Lock() { __NODE_ALLOCATOR_UNLOCK; }
            };

  public:
    static void* allocate(size_t __n){
      void* __ret = 0;
      if (__n > (size_t) _MAX_BYTES) {
        __ret = malloc_alloc::allocate(__n);
      }else {
        _Obj* __STL_VOLATILE* __my_free_list = _S_free_list + _S_freelist_index(__n);
        #ifndef _NOTHREADS
        _Lock __lock_instance;
        #endif

        _Obj* __RESTRICT __result = *__my_free_list;
        if (__result == 0) __ret = _S_refill(_S_round_up(__n));
        else {
          *__my_free_list = __result -> _M_free_list_link;
          __ret = __result;
        }
      }

      return __ret;
  };
  static void deallocate(void* __p, size_t __n)
  {
    if (__n > (size_t) _MAX_BYTES) malloc_alloc::deallocate(__p, __n);
    else {
      _Obj* __STL_VOLATILE*  __my_free_list= _S_free_list + _S_freelist_index(__n);
      _Obj* __q = (_Obj*)__p;

      // acquire lock
      #ifndef _NOTHREADS
      _Lock __lock_instance;
      #endif 
      __q -> _M_free_list_link = *__my_free_list;
      *__my_free_list = __q;
    }
  }
  static void* reallocate(void* __p, size_t __old_sz, size_t __new_sz);
} ;

//__default_alloc_template实例化
typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
typedef __default_alloc_template<false, 0> single_client_alloc;

template <bool __threads, int __inst>
inline bool operator==(const __default_alloc_template<__threads, __inst>&,const __default_alloc_template<__threads, __inst>&)
{
  return true;
}

# ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER
template <bool __threads, int __inst>
inline bool operator!=(const __default_alloc_template<__threads, __inst>&,const __default_alloc_template<__threads, __inst>&)
{
  return false;
}
# endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */


//__default_alloc_template内部函数实现
template <bool __threads, int __inst>
char*__default_alloc_template<__threads, __inst>::_S_chunk_alloc(size_t __size, int& __nobjs)
{
    char* __result;
    size_t __total_bytes = __size * __nobjs;
    size_t __bytes_left = _S_end_free - _S_start_free;

    if (__bytes_left >= __total_bytes) {
        __result = _S_start_free;
        _S_start_free += __total_bytes;
        return(__result);
    } else if (__bytes_left >= __size) {
        __nobjs = (int)(__bytes_left/__size);
        __total_bytes = __size * __nobjs;
        __result = _S_start_free;
        _S_start_free += __total_bytes;
        return(__result);
    } else {
        size_t __bytes_to_get = 2 * __total_bytes + _S_round_up(_S_heap_size >> 4);
        if (__bytes_left > 0) {
            _Obj* __STL_VOLATILE* __my_free_list =_S_free_list + _S_freelist_index(__bytes_left);
            ((_Obj*)_S_start_free) -> _M_free_list_link = *__my_free_list;
            *__my_free_list = (_Obj*)_S_start_free;
        }
        _S_start_free = (char*)malloc(__bytes_to_get);
        if (0 == _S_start_free) {
            size_t __i;
            _Obj* __STL_VOLATILE* __my_free_list;
	          _Obj* __p;
            for (__i = __size;__i <= (size_t) _MAX_BYTES;__i += (size_t) _ALIGN) {
                __my_free_list = _S_free_list + _S_freelist_index(__i);
                __p = *__my_free_list;
                if (0 != __p) {
                    *__my_free_list = __p -> _M_free_list_link;
                    _S_start_free = (char*)__p;
                    _S_end_free = _S_start_free + __i;
                    return(_S_chunk_alloc(__size, __nobjs));
                }
            }
	    _S_end_free = 0;	// In case of exception.
            _S_start_free = (char*)malloc_alloc::allocate(__bytes_to_get);
        }
        _S_heap_size += __bytes_to_get;
        _S_end_free = _S_start_free + __bytes_to_get;
        return(_S_chunk_alloc(__size, __nobjs));
    }
}


template <bool __threads, int __inst>
void* __default_alloc_template<__threads, __inst>::_S_refill(size_t __n)
{
    int __nobjs = 20;
    char* __chunk = _S_chunk_alloc(__n, __nobjs);
    _Obj* __STL_VOLATILE* __my_free_list;
    _Obj* __result;
    _Obj* __current_obj;
    _Obj* __next_obj;
    int __i;

    if (1 == __nobjs) return(__chunk);
    __my_free_list = _S_free_list + _S_freelist_index(__n);

      __result = (_Obj*)__chunk;
      *__my_free_list = __next_obj = (_Obj*)(__chunk + __n);
      for (__i = 1; ; __i++) {
        __current_obj = __next_obj;
        __next_obj = (_Obj*)((char*)__next_obj + __n);
        if (__nobjs - 1 == __i) {
            __current_obj -> _M_free_list_link = 0;
            break;
        } else {
            __current_obj -> _M_free_list_link = __next_obj;
        }
      }
    return(__result);
}

template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::reallocate(void* __p, size_t __old_sz,size_t __new_sz)
{
    void* __result;
    size_t __copy_sz;

    if (__old_sz > (size_t) _MAX_BYTES && __new_sz > (size_t) _MAX_BYTES) {
        return(realloc(__p, __new_sz));
    }
    if (_S_round_up(__old_sz) == _S_round_up(__new_sz)) return(__p);
    __result = allocate(__new_sz);
    __copy_sz = __new_sz > __old_sz? __old_sz : __new_sz;
    memcpy(__result, __p, __copy_sz);
    deallocate(__p, __old_sz);
    return(__result);
}


#ifdef __STL_THREADS
    template <bool __threads, int __inst>
    _STL_mutex_lock
    __default_alloc_template<__threads, __inst>::_S_node_allocator_lock
        __STL_MUTEX_INITIALIZER;
#endif
//__default_alloc_template内部变量初始化
template <bool __threads, int __inst>
char* __default_alloc_template<__threads, __inst>::_S_start_free = 0;

template <bool __threads, int __inst>
char* __default_alloc_template<__threads, __inst>::_S_end_free = 0;

template <bool __threads, int __inst>
size_t __default_alloc_template<__threads, __inst>::_S_heap_size = 0;

template <bool __threads, int __inst>
typename __default_alloc_template<__threads, __inst>::_Obj* __STL_VOLATILE __default_alloc_template<__threads, __inst> 
::_S_free_list[
# if defined(__SUNPRO_CC) || defined(__GNUC__) || defined(__HP_aCC)
    _NFREELISTS
# else
    __default_alloc_template<__threads, __inst>::_NFREELISTS
# endif
] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
#endif /* ! __USE_MALLOC */




//#ifdef __STL_USE_STD_ALLOCATORS：
//若定义使用 C++ 标准库提供的默认分配器 std::allocator，而非SGI STL 使用的内存技术
template <class _Tp>
class allocator 
{
    typedef alloc _Alloc;         
  public:
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;
    typedef _Tp*       pointer;
    typedef const _Tp* const_pointer;
    typedef _Tp&       reference;
    typedef const _Tp& const_reference;
    typedef _Tp        value_type;

    template <class _Tp1> struct rebind {
      typedef allocator<_Tp1> other;
    };

    allocator() __STL_NOTHROW {}
    allocator(const allocator&) __STL_NOTHROW {}
    template <class _Tp1> allocator(const allocator<_Tp1>&) __STL_NOTHROW {}
    ~allocator() __STL_NOTHROW {}

    pointer address(reference __x) const { return &__x; }
    const_pointer address(const_reference __x) const { return &__x; }

    _Tp* allocate(size_type __n, const void* = 0) 
    {
      return __n != 0 ? static_cast<_Tp*>(_Alloc::allocate(__n * sizeof(_Tp))) : 0;
    }

    void deallocate(pointer __p, size_type __n)
    { _Alloc::deallocate(__p, __n * sizeof(_Tp)); }

    size_type max_size() const __STL_NOTHROW 
    { return size_t(-1) / sizeof(_Tp); }

    void construct(pointer __p, const _Tp& __val) { new(__p) _Tp(__val); }
    void destroy(pointer __p) { __p->~_Tp(); }
};
//std::allocator适配器的void特化版本
template<>
class allocator<void> {
public:
  typedef size_t      size_type;
  typedef ptrdiff_t   difference_type;
  typedef void*       pointer;
  typedef const void* const_pointer;
  typedef void        value_type;

  template <class _Tp1> struct rebind {
    typedef allocator<_Tp1> other;
  };
};


template <class _T1, class _T2>
inline bool operator==(const allocator<_T1>&, const allocator<_T2>&) 
{
  return true;
}

template <class _T1, class _T2>
inline bool operator!=(const allocator<_T1>&, const allocator<_T2>&)
{
  return false;
}

//SGI::allocator适配器的实现
template <class _Tp, class _Alloc>
struct __allocator 
{
    _Alloc __underlying_alloc;
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef _Tp*       pointer;
    typedef const _Tp* const_pointer;
    typedef _Tp&       reference;
    typedef const _Tp& const_reference;
    typedef _Tp        value_type;

    template <class _Tp1> struct rebind {
      typedef __allocator<_Tp1, _Alloc> other;
    };

    __allocator() __STL_NOTHROW {}
    __allocator(const __allocator& __a) __STL_NOTHROW: __underlying_alloc(__a.__underlying_alloc) {}
    template <class _Tp1> 
    __allocator(const __allocator<_Tp1, _Alloc>& __a) __STL_NOTHROW: __underlying_alloc(__a.__underlying_alloc) {}
    ~__allocator() __STL_NOTHROW {}

    pointer address(reference __x) const { return &__x; }
    const_pointer address(const_reference __x) const { return &__x; }

    _Tp* allocate(size_type __n, const void* = 0) {
      return __n != 0 ? static_cast<_Tp*>(__underlying_alloc.allocate(__n * sizeof(_Tp))) : 0;
    }

    void deallocate(pointer __p, size_type __n)
      { __underlying_alloc.deallocate(__p, __n * sizeof(_Tp)); }

    size_type max_size() const __STL_NOTHROW 
      { return size_t(-1) / sizeof(_Tp); }

    void construct(pointer __p, const _Tp& __val) { new(__p) _Tp(__val); }
    void destroy(pointer __p) { __p->~_Tp(); }
};


//SGI：：alloctor特化版本
template <class _Alloc>
class __allocator<void, _Alloc> {
  typedef size_t      size_type;
  typedef ptrdiff_t   difference_type;
  typedef void*       pointer;
  typedef const void* const_pointer;
  typedef void        value_type;

  template <class _Tp1> struct rebind {
    typedef __allocator<_Tp1, _Alloc> other;
  };
};

template <class _Tp, class _Alloc>
inline bool operator==(const __allocator<_Tp, _Alloc>& __a1,const __allocator<_Tp, _Alloc>& __a2)
{
  return __a1.__underlying_alloc == __a2.__underlying_alloc;
}


template <class _Tp, class _Alloc>
inline bool operator!=(const __allocator<_Tp, _Alloc>& __a1,const __allocator<_Tp, _Alloc>& __a2)
{
  return __a1.__underlying_alloc != __a2.__underlying_alloc;
}

template <int inst>
inline bool operator==(const __malloc_alloc_template<inst>&, const __malloc_alloc_template<inst>&)
{
  return true;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER
template <int __inst>
inline bool operator!=(const __malloc_alloc_template<__inst>&,const __malloc_alloc_template<__inst>&)
{
  return false;
}
#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */


template <class _Alloc>
inline bool operator==(const debug_alloc<_Alloc>&, const debug_alloc<_Alloc>&) {
  return true;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER
template <class _Alloc>
inline bool operator!=(const debug_alloc<_Alloc>&, const debug_alloc<_Alloc>&) {
  return false;
}
#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

//分配器特征萃取器

template <class _Tp, class _Allocator>
struct _Alloc_traits
{
  static const bool _S_instanceless = false;
  typedef typename _Allocator::__STL_TEMPLATE rebind<_Tp>::other allocator_type;
};

template <class _Tp, class _Allocator>
const bool _Alloc_traits<_Tp, _Allocator>::_S_instanceless;

//分配器特性：default allocator特化版本的

template <class _Tp, class _Tp1>
struct _Alloc_traits<_Tp, allocator<_Tp1> >
{
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, alloc> _Alloc_type;
  typedef allocator<_Tp> allocator_type;
};

// 针对SGI原始分配器的特化
//直接使用malloc的分配器
template <class _Tp, int __inst>
struct _Alloc_traits<_Tp, __malloc_alloc_template<__inst> >
{
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, __malloc_alloc_template<__inst> > _Alloc_type;
  typedef __allocator<_Tp, __malloc_alloc_template<__inst> > allocator_type;
};
//带内存池的分配器
template <class _Tp, bool __threads, int __inst>
struct _Alloc_traits<_Tp, __default_alloc_template<__threads, __inst> >
{
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, __default_alloc_template<__threads, __inst> > 
          _Alloc_type;
  typedef __allocator<_Tp, __default_alloc_template<__threads, __inst> > 
          allocator_type;
};
//带调试的分配器
template <class _Tp, class _Alloc>
struct _Alloc_traits<_Tp, debug_alloc<_Alloc> >
{
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, debug_alloc<_Alloc> > _Alloc_type;
  typedef __allocator<_Tp, debug_alloc<_Alloc> > allocator_type;
};

//针对__allocator适配器的特化
//适配 __malloc_alloc_template
template <class _Tp, class _Tp1, int __inst>
struct _Alloc_traits<_Tp,  __allocator<_Tp1, __malloc_alloc_template<__inst> > >
{
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, __malloc_alloc_template<__inst> > _Alloc_type;
  typedef __allocator<_Tp, __malloc_alloc_template<__inst> > allocator_type;
};
//适配 __default_alloc_template
template <class _Tp, class _Tp1, bool __thr, int __inst>
struct _Alloc_traits<_Tp, __allocator<_Tp1, __default_alloc_template<__thr, __inst> > >
{
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, __default_alloc_template<__thr,__inst> > _Alloc_type;
  typedef __allocator<_Tp, __default_alloc_template<__thr,__inst> > allocator_type;
};
//适配 debug_alloc
template <class _Tp, class _Tp1, class _Alloc>
struct _Alloc_traits<_Tp, __allocator<_Tp1, debug_alloc<_Alloc> > >
{
  static const bool _S_instanceless = true;
  typedef simple_alloc<_Tp, debug_alloc<_Alloc> > _Alloc_type;
  typedef __allocator<_Tp, debug_alloc<_Alloc> > allocator_type;
};


//#endif /* __STL_USE_STD_ALLOCATORS */
__STL_END_NAMESPACE

#undef __PRIVATE

#endif /* __SGI_STL_INTERNAL_ALLOC_H */

// Local Variables:
// mode:C++
// End:
