#include<iostream>
//链表类定义
template<typename T>
class List
{
public:
    void insert_front(T value);//链表头部插入元素
    void insert_end(T value);//链表尾部插入元素
    void display(std::ostream & os = std::cout) const;//输出链表元素
    //
private:
    ListItem<T>*  _end;//指向链表尾节点指针
    LsitItem<t>* _front;//指向链表头节点
    long _size;
};

//链表节点类定义
template <typename T>
class ListItem
{
    public:
        T value() const{ return _value;}//返回节点存储的值
        ListItem* next(){ return _next;}//返回指向下一个节点的指针
    private:
        T _value;//节点存储的值
        ListItem* _next;//指向下一个节点的指针
};