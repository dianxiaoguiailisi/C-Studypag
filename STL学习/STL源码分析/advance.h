#include<iostream>
using namespace std;

template <class ForwardIterator , class Distance>
inline void __advance (ForwardIterator& i, Distance n, input_iterator_tag)
{
    while(n--)  ++i;
}

template <class ForwardIterator , class Distance>
inline void __advance (ForwardIterator& i, Distance n, forward_iterator_tag)
{
    advance(i, n, input_iterator_tag);
}

template <class ForwardIterator , class Distance>
inline void __advance (ForwardIterator& i, Distance n, bidirectional_iterator_tag)
{
    if(n >= 0)
        while(n--) ++i;
    else 
        while(n++) --i;
}

template <class ForwardIterator , class Distance>
inline void __advance (ForwardIterator& i, Distance n, random_access_iterator_tag)
{
    i+=n;
}


//对外接口
template <class InputIterator , class Distance>
inline void advance (InputIterator& i, Distance n)
{
    __advance(i, n, 
        iterator_traits<InputIterator>::iterator_category()/*产生一个临时对象，根据类型调用不同的函数 */);
}
