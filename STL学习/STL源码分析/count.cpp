template<class I, class T>
typename iterator_traits<I>::difference_type
count(I first, I last, const T& value){
    typename iterator_traits<I>::difference_type n =0;
    for(;first != last; ++first)
        if(*first == value)
        ++n;
    return n;
}