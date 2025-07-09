#include<iostream>
using namespace std;

template<class InputIterator>
inline iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last,input_iterator_tag)
{
    iterator_traits<InputIterator>::difference_type n =0;
    while(first != last){
        ++first;++n;
    }
    return n;
}

template<class RandomAccessIterator>
inline iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last,random_access_iterator_tag)
{
    return lasg-first;
}
//“传递调用”的行为模式：当客户端调用distance(), 
//并使用output iterator 或forward iterator 或bidirctional iterator 会传递调用
//调用input iterator 版的那个__distance 
template<class InputIterator>
inline iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category())y
}

