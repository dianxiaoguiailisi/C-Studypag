/**
 * @file iterator.h
 * @author zx
 * @brief 迭代器适配器的实现
 * @version 0.1
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

#include"iterator_base.h"
#include <cstddef>

namespace MYSTL
{
    //一个输出迭代器适配器，允许将元素插入到容器的尾部而不是覆盖现有元素
    template<class Container>
    class back_insert_iterator
    {
        protected:
            Container* container;
        public:
            typedef Container container_type;
            typedef output_iterator_tag iterator_category;
            typedef void  value_type;
            typedef void difference_type;
            typedef void pointer;
            typedef void reference;
        //构造函数
            explicit back_insert_iterator(Container& x):container(& x){}
        //赋值运算符：当迭代器被赋值时（如*it = value），实际执行容器的插入操作
            back_insert_iterator<Container>& operator=(const typename Container::value_type& value)
            {
                container->push_back(value);
                return *this;
            }
        //满足迭代器的接口规范而设计（无用）
            back_insert_iterator<Container>& operator* () { return *this;}//解引用运算符
            back_insert_iterator<Container>& operator++() { return *this;}//前置递增运算符
            back_insert_iterator<Container>& operator++(int) { return *this;}//后置递增运算符
    };

    template <class Container>
    class front_insert_iterator 
    {
        protected:
            Container* container;
        public:
            typedef Container          container_type;
            typedef output_iterator_tag iterator_category;
            typedef void                value_type;
            typedef void                difference_type;
            typedef void                pointer;
            typedef void                reference;

            explicit front_insert_iterator(_Container& x) : container(& x) {}
            front_insert_iterator<Container>& operator=(const typename Container::value_type& value) { 
                container->push_front(value);
                return *this;
            }
            front_insert_iterator<Container>& operator*() { return *this; }
            front_insert_iterator<Container>& operator++() { return *this; }
            front_insert_iterator<Container>& operator++(int) { return *this; }
    };

    //用于将容器的插入操作适配为迭代器的赋值操作
    template <class Container>
    class insert_iterator 
    {
        protected:
            Container* container;
            typename Container::iterator iter;
        public:
            typedef Container          container_type;
            typedef output_iterator_tag iterator_category;
            typedef void                value_type;
            typedef void                difference_type;
            typedef void                pointer;
            typedef void                reference;

            insert_iterator(Container& x, typename Container::iterator i) : container(&x), iter(i) {}
            insert_iterator<Container>& operator=(const typename Container::value_type& value) 
            { 
                iter = container->insert(iter, value);
                ++iter;
                return *this;
            }
            insert_iterator<Container>& operator*() { return *this; }
            insert_iterator<Container>& operator++() { return *this; }
            insert_iterator<Container>& operator++(int) { return *this; }
    };

    //反向迭代器适配器，用于将双向迭代器的遍历方向反转（从后向前遍历）
    template <class BidirectionalIterator, class Tp, class Reference = Tp&, class Distance = ptrdiff_t> 
    class reverse_bidirectional_iterator 
    {
        typedef reverse_bidirectional_iterator<BidirectionalIterator, Tp,  Reference, Distance>  Self;
        protected:
            BidirectionalIterator current;
        public:
            typedef bidirectional_iterator_tag iterator_category;
            typedef Tp                        value_type;
            typedef Distance                  difference_type;
            typedef Tp*                       pointer;
            typedef Reference                 reference;

            reverse_bidirectional_iterator() {}
            explicit reverse_bidirectional_iterator(_BidirectionalIterator __x): current(__x) {}
            BidirectionalIterator base() const { return current; }
            Reference operator*() const 
            {
                BidirectionalIterator tmp = current;
                return *--tmp;
            }
            pointer operator->() const { return &(operator*()); }

            Self& operator++() {
                --current;
                return *this;
            }
            Self operator++(int) {
                Self tmp = *this;
                --current;
                return tmp;
            }
            Self& operator--() {
                ++current;
                return *this;
            }
            Self operator--(int) {
                Self tmp = *this;
                ++current;
                return tmp;
            }
};
    
    //一个反向迭代器适配器，允许以相反的顺序遍历容器
    template <class Iterator>
    class reverse_iterator 
    {
        protected:
            Iterator current;
        public:
            typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
            typedef typename iterator_traits<Iterator>::value_type value_type;
            typedef typename iterator_traits<Iterator>::difference_type difference_type;
            typedef typename iterator_traits<Iterator>::pointer pointer;
            typedef typename iterator_traits<Iterator>::reference reference;
            typedef Iterator iterator_type;
            typedef reverse_iterator<Iterator> Self;

        public:
            reverse_iterator() {}
            explicit reverse_iterator(iterator_type x) : current(x) {}
            reverse_iterator(const _Self& x) : current( x.current) {}

            template <class Iter>
            reverse_iterator(const reverse_iterator<Iter>& x) :current(x.base()) {}
            
            iterator_type base() const { return current; }
            reference operator*() const {
                Iterator tmp = current;
                return *--tmp;
            }
            pointer operator->() const { return &(operator*()); }

            Self& operator++() { --current; return *this; }
            Self operator++(int) { Self tmp = *this; --current; return tmp; }
            Self& operator--() { ++current; return *this; }
            Self operator--(int) { Self tmp = *this; ++current; return tmp; }
            Self operator+(difference_type n) const { return Self(current - n);}
            Self& operator+=(difference_type n) { current -= n; return *this; }
            Self operator-(difference_type n) const { return Self(current + n); }
            Self& operator-=(difference_type n) { current += n; return *this; }
            reference operator[](difference_type n) const { return *(*this + n); }  
    }; 
    
    //用于将随机访问迭代器（如 vector 的迭代器）转换为反向遍历的迭代器
    template <class RandomAccessIterator, class Tp, class Reference = Tp& ,class Distance = ptrdiff_t> 
    class reverse_iterator 
    {
        typedef reverse_iterator<RandomAccessIterator, Tp, Reference, Distance> Self;
        protected:
            RandomAccessIterator current;
        public:
            typedef random_access_iterator_tag iterator_category;
            typedef Tp                        value_type;
            typedef Distance                  difference_type;
            typedef Tp*                       pointer;
            typedef Reference                 reference;
      
            reverse_iterator() {}
            explicit reverse_iterator( RandomAccessIterator x) : current( x) {}
            RandomAccessIterator base() const { return current; }
            Reference operator*() const { return *(current - 1); }
            pointer operator->() const { return &(operator*()); }
            Self& operator++() { --current; return *this; }
            Self operator++(int) { Self tmp = *this; --current; return tmp; }
            Self& operator--() { ++current; return *this; }
            Self operator--(int) { Self tmp = *this; ++current; return tmp; }
            Self operator+(Distance n) const { return Self(current - n); }
            Self& operator+=(Distance n) { current -= n; return *this; }
            Self operator-(Distance n) const { return Self(current + n);}
            Self& operator-=(Distance n) { current += n; return *this; }
            Reference operator[](Distance n) const { return *(*this + n); }
      };
       
    //流迭代器（Stream Iterators） 的实现:从输入流（如文件、标准输入）读取数据，支持按元素类型（如 int、string）解析
    //ostream_iterator（输出流迭代器）
    //istreambuf_iterator（输入缓冲区迭代器）
    //ostreambuf_iterator（输出缓冲区迭代器）
}
#endif
