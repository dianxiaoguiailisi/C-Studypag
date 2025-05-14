/**
 * @file stl_uninitialized.h
 * @author zx
 * @brief 
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __SGI_STL_INTERNAL_UNINITIALIZED_H
#define __SGI_STL_INTERNAL_UNINITIALIZED_H

__STL_BEGIN_NAMESPACE

/**
 * @brief uninitialized_copy
 * uninitialized_copy 函数用于把一个迭代器范围 [__first, __last) 内的元素复制到
 * 以 __result 开始的未初始化内存区域
 * 
 */
//处理平凡可复制函数
template <class _InputIter, class _ForwardIter>
inline _ForwardIter 
__uninitialized_copy_aux(_InputIter __first, _InputIter __last,/*输入迭代器[_first,_last]*/
                                            _ForwardIter __result,/*为初始内存__result*/
                                            __true_type/*判断是否是平凡可复制*/){
  return copy(__first, __last, __result);
}

//处理非平凡函数
template <class _InputIter, class _ForwardIter>
_ForwardIter  
__uninitialized_copy_aux(_InputIter __first, _InputIter __last,
                         _ForwardIter __result,
                         __false_type/*需要调用的构造函数*/){
  _ForwardIter __cur = __result;//迭代器__cur指向内存初始位置
  __STL_TRY /*异常处理模块*/{
    for ( ; __first != __last; ++__first, ++__cur)
      _Construct(&*__cur, *__first);//对每一个元素调用construct在&*__cur位置构造一个新对象
    return __cur;
  }
  __STL_UNWIND(_Destroy(__result, __cur));//如果抛出异常，将构造好的对象，进行析构保证资源释放
}

//类型特征分派器：根据是否是平凡可复制，调用不同的_uninitalized_acpy_aux
template <class _InputIter, class _ForwardIter, class _Tp>
inline _ForwardIter 
__uninitialized_copy(_InputIter __first, _InputIter __last,_ForwardIter __result, _Tp* /*模板参数推导*/){

  typedef typename __type_traits<_Tp>::is_POD_type _Is_POD;
  //查询_Tp是不是POD类型并将结果重命名为_IS_PDD?????
  return __uninitialized_copy_aux(__first, __last, __result, 
                                  _Is_POD()/*_Is_POD()：创建一个__true_type或__false_type的临时对象*/);
}

//公共接口：
template <class _InputIter, class _ForwardIter>
inline _ForwardIter 
uninitialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __result/*未初始化内存地址 */)
{
  //调用分配器
  return __uninitialized_copy(__first, __last, __result,__VALUE_TYPE(__result)/*复制的类型*/);
}

//针对char 和wchar_t的优化（高效复制）
inline char* 
uninitialized_copy(const char* __first, const char* __last, char* __result) 
{
  memmove(__result, __first, __last - __first);//memmove 是 C 标准库中的内存复制函数，它是专门为高效内存操作而设计的
  return __result + (__last - __first);
}

inline wchar_t* 
uninitialized_copy(const wchar_t* __first, const wchar_t* __last, wchar_t* __result)
{
  memmove(__result, __first, sizeof(wchar_t) * (__last - __first));
  return __result + (__last - __first);
}





/**
 * @brief  uninitialized_copy_n 
 *  uninitialized_copy_n 函数用于把从迭代器 __first 开始的
 *       __count 个元素复制到以 __result 开始的未初始化内存区域
 */


template <class _InputIter, class _Size, class _ForwardIter>
pair<_InputIter, _ForwardIter> 
__uninitialized_copy_n(_InputIter __first/*输入迭代器起始位置 */, _Size __count/*元素数量*/,
                                                      _ForwardIter __result,/*输出迭代器的起始位置*/
                                                      input_iterator_tag)/*迭代器类别标签，用于函数重载*/
{
  _ForwardIter __cur = __result;
  __STL_TRY {
    for ( ; __count > 0 ; --__count, ++__first, ++__cur) 
      _Construct(&*__cur/*初始化对象的地址 */, *__first/*初始化对象的值*/);
      /* &*__cur:这里先解引用获得其迭代器指向内存内容，在取该内存位置的地址（这里是指针），
          __Construct要求第一个参数是指针*/
    return pair<_InputIter, _ForwardIter>(__first, __cur);
  }
  __STL_UNWIND(_Destroy(__result, __cur));
}

//随机访问迭代器
template <class _RandomAccessIter, class _Size, class _ForwardIter>
inline pair<_RandomAccessIter, _ForwardIter> 
__uninitialized_copy_n(_RandomAccessIter __first, _Size __count, _ForwardIter __result,
                        random_access_iterator_tag) {
  _RandomAccessIter __last = __first + __count;
  return pair<_RandomAccessIter, _ForwardIter>(__last,uninitialized_copy(__first, __last, __result));
}

//迭代器类别自动分配版本
template <class _InputIter, class _Size, class _ForwardIter>
inline pair<_InputIter, _ForwardIter> 
__uninitialized_copy_n(_InputIter __first, _Size __count,_ForwardIter __result) 
{
  return __uninitialized_copy_n(__first, __count, __result,
                                                          __ITERATOR_CATEGORY(__first)/*迭代器类别*/);
}

//对外接口
template <class _InputIter, class _Size, class _ForwardIter>
inline pair<_InputIter, _ForwardIter> 
uninitialized_copy_n(_InputIter __first, _Size __count,_ForwardIter __result/*复制元素的目标位置*/) 
{
  return __uninitialized_copy_n(__first, __count, __result,
                                __ITERATOR_CATEGORY(__first)/*获得迭代器类别标签*/);
}

/**
 * @brief 在 [__first, __last) 这个未初始化的内存区域中，使用值 __x 来构造对象
 * 
 */
template <class _ForwardIter, class _Tp>
inline void 
__uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last, const _Tp& __x, __true_type){
  fill(__first, __last, __x);
}

template <class _ForwardIter, class _Tp>
void 
__uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last, const _Tp& __x, __false_type)
{
  _ForwardIter __cur = __first;
  __STL_TRY {
    for ( ; __cur != __last; ++__cur)
      _Construct(&*__cur, __x);
  }
  __STL_UNWIND(_Destroy(__first, __cur));
}

template <class _ForwardIter, class _Tp, class _Tp1>
inline void 
__uninitialized_fill(_ForwardIter __first,  _ForwardIter __last, const _Tp& __x, _Tp1*)
{
  typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
  __uninitialized_fill_aux(__first, __last, __x, _Is_POD());
                   
}

template <class _ForwardIter, class _Tp>
inline void 
uninitialized_fill(_ForwardIter __first,_ForwardIter __last, const _Tp& __x)
{

  __uninitialized_fill(__first, __last, __x, __VALUE_TYPE(__first));
}



/**
 * @brief 从 __first 开始的未初始化内存区域中，填充 __n 个值为 __x 的元素
 * 
 */
template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter
__uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,const _Tp& __x, __true_type)
{
  return fill_n(__first, __n, __x);
}

template <class _ForwardIter, class _Size, class _Tp>
_ForwardIter
__uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,const _Tp& __x, __false_type)
{
  _ForwardIter __cur = __first;
  __STL_TRY {
    for ( ; __n > 0; --__n, ++__cur)
      _Construct(&*__cur, __x);
    return __cur;
  }
  __STL_UNWIND(_Destroy(__first, __cur));
}

template <class _ForwardIter, class _Size, class _Tp, class _Tp1>
inline _ForwardIter 
__uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x, _Tp1*)
{
  typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
  return __uninitialized_fill_n_aux(__first, __n, __x, _Is_POD());
}

template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter 
uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x)
{
  return __uninitialized_fill_n(__first, __n, __x, __VALUE_TYPE(__first));
}

/**
 * @brief 在未初始化的内存区域中执行两次连续的复制操作。具体来说，它先将 [__first1, __last1) 区间内的元素复制到以 __result 开始的未初始化内存区域，
 * 然后再将 [__first2, __last2) 区间内的元素复制到紧接着前面复制区域的未初始化内存区域
 * 
 */
template <class _InputIter1, class _InputIter2, class _ForwardIter>
inline _ForwardIter
__uninitialized_copy_copy(_InputIter1 __first1, _InputIter1 __last1,
                          _InputIter2 __first2, _InputIter2 __last2,
                          _ForwardIter __result)
{
  _ForwardIter __mid = uninitialized_copy(__first1, __last1, __result);
  __STL_TRY {
    return uninitialized_copy(__first2, __last2, __mid);
  }
  __STL_UNWIND(_Destroy(__result, __mid));
}


/**
 * @brief 它会先将 [__result, __mid) 范围内的未初始化内存区域用值 __x 填充，
 * 然后将 [__first, __last) 范围内的元素复制到紧接着填充区域的未初始化内存区域
 */
template <class _ForwardIter, class _Tp, class _InputIter>
inline _ForwardIter 
__uninitialized_fill_copy(_ForwardIter __result, _ForwardIter __mid,
                          const _Tp& __x,
                          _InputIter __first, _InputIter __last)
{
  uninitialized_fill(__result, __mid, __x);
  __STL_TRY {
    return uninitialized_copy(__first, __last, __mid);
  }
  __STL_UNWIND(_Destroy(__result, __mid));
}

/**
 * @brief  它会将 [__first1, __last1) 范围内的元素复制到以 __first2 开始的未初始化内存区域，
 * 然后将 [__first2 + (__last1 - __first1), __last2) 范围内的未初始化内存区域用值 __x 进行填充。
 */
template <class _InputIter, class _ForwardIter, class _Tp>
inline void
__uninitialized_copy_fill(_InputIter __first1, _InputIter __last1,
                          _ForwardIter __first2, _ForwardIter __last2,
                          const _Tp& __x)
{
  _ForwardIter __mid2 = uninitialized_copy(__first1, __last1, __first2);
  __STL_TRY {
    uninitialized_fill(__mid2, __last2, __x);
  }
  __STL_UNWIND(_Destroy(__first2, __mid2));
}

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_UNINITIALIZED_H */
