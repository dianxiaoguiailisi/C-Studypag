#include "3mylist.h"

template <class Item>
struct ListIter
{
    Item* ptr;

    ListIter(Ltem* p = 0):ptr(p){}
    Item& operator* () const {return *ptr; }
    Item* operator-> () const { return ptr; }
    ListIter& operator++()
    {
        ptr = ptr->next();
        return  *this;
    }

    ListIter& operator++( int )
    {   
        ListIter tmp = *this;
        ++*this;
        return  temp;
    }

    bool operator== (const ListLter& i){ return ptr == i.ptr; }
    bool operator!= (const ListLter& i){  return ptr != i.ptr; }

};