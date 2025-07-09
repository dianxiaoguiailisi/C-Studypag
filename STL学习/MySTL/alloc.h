/**
 * @file alloc.h
 * @author zx
 * @brief 定义了一、二级配置器,负责内存空间的配置和释放
 * @version 0.1
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MYSTL_ALLOC_H_
#define MYSTL_ALLOC_H_

#include<cstddef>
#include<new>
#include<cstdlib>

namespace MYSTL
{
//分配器定义

    /*malloc_alloc_template:第一级分配器*/
    template<int inst>
    class malloc_alloc_template
    {
        private:
            static void* S_oom_malloc(size_t);//内存失败处理函数.143
            static void* S_oom_realloc(void*, size_t);//内存重新分配失败时处理函数,159
            static void (* malloc_alloc_oom_hander)();//用户通过__set_malloc_handler注册的处理函数
        public:
            static void* allocate(size_t n);//内存分配
            static void deallocate(void* p, size_t);//内存释放
            static void* reallocate(void* p, size_t, size_t new_sz);//内存重新分配

            /**
             * @brief //注册和管理内存分配失败时的回调函数
             *  参数：void (*__f)() - 指向无参数、无返回值函数的指针
             *  返回值：void (*)() - 同样是一个指向无参数、无返回值函数的指针
             */
            static void (* set_malloc_hander(void* (* f) ()))
            {
                //1. 记录原来的处理函数
                void (* old)() = malloc_alloc_oom_hander;
                //2：登记新的处理函数
                malloc_alloc_oom_hander = f;
                //3：返回
                return old ;
            }
    };
    
    /*第二级内存分配器（__default_alloc_template） 的实现：
        用于高效管理小块内存（≤128 字节），通过 自由链表（Free List） 和 批量分配 机制减少内存碎片和系统调用开销*/
    template<bool threads/*是否支持多线程*/, int inst/*实例编号（通常用于控制内存池的行为*/>
    class default_alloc_template
    {
            private:
            //变量
                enum{ALIGN = 8};//内存对齐字节
                enum{MAX_BYTES = 128};//管理的最大块大小
                enum{NFREELISTS = 16};//自由链表的数量
            //内存快结构体
                union obj
                {
                    union obj* M_free_list_link;//指向下一个空闲快
                    char M_client_date[1];//用户数据的起始位置
                };
                static obj*  S_free_list[NFREELISTS];//自由链表数组
                static char* S_start_free;//堆中剩余内存起始地址
                static char* S_end_free;//堆中剩余内存结束地址
                static size_t S_heap_size;//已经分配堆的总大小
            //函数
                static size_t S_round_up(size_t bytes)//将任意字节数向上取整到指定对齐边界
                {
                    return (((bytes) + (size_t) ALIGN-1) & ~((size_t) ALIGN - 1));
                }
                static size_t S_freelist_index(size_t bytes)//计算需求块对应的空闲链表索引
                {
                    return (((bytes) + (size_t)ALIGN-1)/(size_t)ALIGN - 1);
                }
                static void* S_refill(size_t n);//填充自由链表
                static char* S_chunk_alloc(size_t size, int& nobjs);//S_chunk_alloc的核心职责是管理连续空间，并在必要时将碎片整理到链表。从内存池分配nobjs个大小为size的对象，返回首地址
            
            public:
                static void* allocate(size_t n);//分配 n个字节内存
                static void* deallocate(void* p, size_t n);//回收用户不用的块
                static void* reallocate(void* P,size_t ole_size, size_t new_size);//内存重新分配
    };
    
//适配器定义：底层封装了不同分配器   
    /*1：类型安全适配器：simple_alloc*/
    template<class TP, class Alloc>
    class simple_alloc
    {
        public:
            static TP* allocate(size_t n)
            {//分配n个TP类型对象需要内存
                 return n == 0 ? 0 :(TP*) Alloc::allocate(n * sizeof(TP)); 
            }
            static TP* allocate(void ) 
            {//分配单个对象 
                return (TP*) Alloc::allocate(sizeof(TP));
            }
            static void deallocate(TP* p, size_t n) 
            {//释放n个对象数组
                if(n != 0) 
                    Alloc::deallocate(p, n*sizeof(TP));
            }
            static void deallocate(TP* p)
            { //释放对象数组
                Alloc::deallocate(p, sizeof(TP));
            }
    };

    /*2：调试功能的适配器：其核心功能是在分配的内存块前额外存储大小信息，以便在释放或重新分配时验证内存操作的正确性。*/
    
    /*3：标准适配器：std::allocator*/
    template<class TP>
    class allocator
    {
        typedef alloc Alloc;
        public:
        //变量
            typedef size_t     size_type;
            typedef ptrdiff_t  difference_type;
            typedef Tp*       pointer;
            typedef const Tp* const_pointer;
            typedef Tp&       reference;
            typedef const Tp& const_reference;
            typedef Tp        value_type;
        //rebind机制
            template<class TP1>
            struct rebind
            {
                typedef allocator<TP1> other;
            };
        //构造与析构函数
            allocator() {}
            allocator(const allocator&)  {}
            template <class _Tp1> allocator(const allocator<_Tp1>&) {}
            ~allocator() {}
        //内存分配和释放
            pointer address(reference x) const;//返回对象地址
            const_pointer address(const_reference _x) const;//
            _TP* allocate(size_type _n, const void* =0);
            void deallocate(pointer _p, size_type _n);
        //对象构造与析构
            void construct(pointer _p, const _TP& val);
            void destory(pointer _P);
    };

        /*标准适配器 的特化版本：其目的是为 void 类型提供专门的分配器定义*/
        template<>
        class allocator<void> 
        {
            public:
                typedef size_t      size_type;
                typedef ptrdiff_t   difference_type;
                typedef void*       pointer;
                typedef const void* const_pointer;
                typedef void        value_type;
                template <class _Tp1> struct rebind { typedef allocator<_Tp1> other; };
        };


    /**
     * @brief  通用分配器适配器,SGI:allocator：将任意符合 STL 规范的底层分配器Alloc封装成一个标准接口，
        使得各种内存管理策略都能无缝集成到 STL 容器中
     * @tparam TP 类型
     * @tparam Alloc 底层的分配器
     */
    template<class Tp, class Alloc>
    struct __allocator
    {
        Alloc underlying_alloc;

        typedef size_t    size_type;
        typedef ptrdiff_t difference_type;
        typedef Tp*       pointer;
        typedef const Tp* const_pointer;
        typedef Tp&       reference;
        typedef const Tp& const_reference;
        typedef Tp        value_type;

        template<class TP1>struct rebind//从当前分配器创建一个管理其他类型的分配器
        {
            typedef __allocator<TP1, Alloc> other;
        };
        //构造函数
        __allocator(){}
        __allocator(const __allocator& a): underlying_alloc(a.underlying_alloc) {}
        template <class Tp1> 
        __allocator(const __allocator<Tp1, Alloc>& a) : underlying_alloc(a.underlying_alloc) {}
        //析构函数
        ~__allocator(){}

        pointer address(reference x) const{ return & x;}
        const_pointer address(const_reference x) const{ return & x;}
        //内存分配和释放
        TP* allocate(size_type n, const void* =0)
        {
            return n != 0 ? static_cast<Tp*>(__underlying_alloc.allocate(n * sizeof(Tp))) : 0;
        }
        void deallocate(pointer p, size_type n)
        {
            underlying_alloc.deallocate(p, n * sizeof(Tp));
        }
        //对象构造和析构
        void construct(pointer p, const TP&__p->~_Tp();  val)
        {
            new(p) Tp(val);
        }
        void destroy(pointer p){ p->~Tp(); }
    };

    //void特化版本
    template <class Alloc>
    class __allocator<void, Alloc> {
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef void*       pointer;
    typedef const void* const_pointer;
    typedef void        value_type;

    template <class Tp1> struct rebind {
        typedef __allocator<Tp1, Alloc> other;
        };
    };

    typedef malloc_alloc_template<0> malloc_alloc;
//不启用多线程

typedef default_alloc_template<false, 0> alloc;
typedef default_alloc_template<false, 0> single_client_alloc;
//分配器特征萃取：
        /*1.通用模板定义*/
        template<class TP, class Allocator>
        struct Alloc_traits
        {
            static const bool S_instanceless = false;//分配器是否有状态
            typedef typename Allocator::rebind<TP>::other allocator_type;
        };
        /*2. STD：：allocator标准分配器特化*/
        template <class Tp, class Tp1>
        struct Alloc_traits<Tp, allocator<Tp1> >
        {
            static const bool S_instanceless = true;
            typedef simple_alloc<Tp, alloc> Alloc_type;
            typedef allocator<Tp> allocator_type;
        };
        /*3. 针对SGI原始分配器的特化*/
            /*3.1:包装了第一级分配器的适配器分配器*/
            template <class Tp, int inst>
            struct Alloc_traits<Tp, malloc_alloc_template<inst> >
            {
                static const bool S_instanceless = true;
                //定义适配器类型
                typedef simple_alloc<Tp, malloc_alloc_template<inst> > Alloc_type;//将底层分配器接口转换为STL兼容的allocate/deallocate
                typedef __allocator<Tp, malloc_alloc_template<inst> > __allocator_type;//进一步封装为完整的标准分配器接口（含construct/deatroy）
            };
            /*3.2:包装了第二级分配器的适配器分配器*/
            template <class Tp, bool threads, int inst>
            struct Alloc_traits<Tp, default_alloc_template<threads, inst> >
            {
              static const bool S_instanceless = true;
              typedef simple_alloc<Tp, default_alloc_template<threads, inst> >  Alloc_type;
              typedef __allocator<Tp, default_alloc_template<threads, inst> >  allocator_type;
            };
            /*3.3:带调试功能的分配器*/

        /*4:针对__allocator适配器的特化*/
            /*4.1：第一级分配器*/
            template <class Tp, class Tp1, int inst>
            struct Alloc_traits<Tp,  __allocator<Tp1, malloc_alloc_template<inst> > >
            {
                static const bool S_instanceless = true;
                typedef simple_alloc<Tp, malloc_alloc_template<inst> > _Alloc_type;
                typedef __allocator<Tp, malloc_alloc_template<inst> > allocator_type;
            };
            /*4.2：第二级分配器*/
            template <class Tp, class Tp1, bool thr, int inst>
            struct Alloc_traits<Tp, __allocator<Tp1, default_alloc_template<thr, inst> > >
            {
                static const bool _S_instanceless = true;
                typedef simple_alloc<Tp, default_alloc_template<thr,inst> > _Alloc_type;
                typedef __allocator<Tp, default_alloc_template<thr,inst> > allocator_type;
            };

            /*4.3：适配 debug_alloc*/

            
//分配器实现：
    //malloc_alloc_template类
    template<int inst>
    void (*malloc_alloc_template<inst> ::malloc_alloc_oom_hander)() =0;//初始化处理函数为0

    template<int inst>
    void* malloc_alloc_template<inst>::S_oom_malloc(size_t n)
    {
        void (*my_malloc_handler)();//函数指针变量：指向无参数、无返回值的函数
        void* result;

        for(;;)
        {
            my_malloc_handler = malloc_alloc_oom_hander;//获得当前注册的内存不足的处理函数
            if(0 == my_malloc_handler){std::bad_alloc(); }//抛出异常}
            //调用处理函数
            (*my_malloc_handler)();
            //再次尝试分配内存
            result = malloc(n);
            if(result)  return result;
        }
    }

    template<int inst>
    void* malloc_alloc_template<inst>::S_oom_realloc(void* p, size_t n)
    {
        void(* my_malloc_hander)();
        void * result;
        for(;;)
        {
            my_malloc_hander = malloc_alloc_oom_hander;
            if(my_malloc_hander == 0) { std::bad_alloc();}
            (*my_malloc_handler)();
            result = realloc(p, n);
            if(result) return result;
        }

    }

    void* malloc_alloc_template<inst>::allocate(size_t n)
    {
        //1：分配内存
        void* result = malloc(n);
        //2：失败调用
        if(result ==0 ) result = S_oom_malloc(n);
        //3：返回
        return result;
    }

    void malloc_alloc_template<inst>::deallocate(void* p, size_t n){ free(p);}

    void* malloc_alloc_template<inst>::reallocate(void* p, size_t old_size, size_t new_size)
    {
        void* result = realloc(p, new_size);
        if(result == 0) result = S_oom_realloc(p, new_size);
        return result;
    }

    //default_alloc_template类
    typedef malloc_alloc_template<0> malloc_alloc;

    template <bool threads, int inst>
    char* default_alloc_template<threads, inst>::S_start_free = 0;

    template <bool threads, int inst>
    char* default_alloc_template<threads, inst>::S_end_free = 0;

    template <bool threads, int inst>
    size_t default_alloc_template<threads, inst>::S_heap_size = 0;

    template<bool threads, int inst>//自由链表初始化为0
    typename default_alloc_template< threads, inst>::obj*default_alloc_template<threads, inst>::S_free_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

    /**
    * @brief S_chunk_alloc的核心职责是管理连续空间，并在必要时将碎片整理到链表。
    *  从内存池分配nobjs个大小为size的对象，返回首地址
    * @param size 单个内存块大小
    * @param nobjs 请求的块数
    * @return char* 
    */
    template<bool threads, int inst>
    char* default_alloc_template<threads, inst>::S_chunk_alloc(size_t size, int& nobjs)
    {
        char* result;//分配内存的起始地址指针
        size_t total_bytes = size* nobjs;//请求的总字节数
        size_t bytes_left = S_end_free -S_start_free;//内存池连续、未分配的字节数
        //1.内存池满足需求：优先使用连续的空间（效率高）
        if(bytes_left >= total_bytes)//内存池剩余连续、未分配的空间 >需求的总字节数
        {
            result = S_start_free;
            S_start_free+=total_bytes;//更新起始地址指针
            return result;
        }else if(bytes_left >= size){//内存池剩余连续、未分配的空间 >需求的一个块字节。只满足部分需求

            nobjs = (int)(bytes_left/size);//以size大小为块，切割剩余连续内存池后的块数
            total_bytes =size * nobjs;
            //分配内存更新指针
            result = S_start_free;
            S_start_free += total_bytes;
            return result;
        }else{//内存池不满足需求
            size_t bytes_to_get = 2* total_bytes + S_round_up(S_heap_size >>4);//计算从系统申请的新内存大小
            //处理内存池碎片
            if(bytes_left > 0)//将当前内存池的剩余碎片加入对应的空闲链表
            {
                obj* my_free_list = S_free_list + S_freelist_index(bytes_left);//获取剩余块对应的索引
                ((obj*) S_start_free) ->M_free_list_link = * my_free_list;//更新下一个空闲链表为my_free_list
                *my_free_list = (obj*) S_free_list; //更新my_free_list为空闲链表头指针
            }
            S_start_free = (char*) malloc(bytes_to_get);//使用malloc申请__bytes_to_get大小的内存块
            if(S_start_free ==0)//失败，备用方案：扫描其他空闲链表，从更大的空闲链表中 “借” 一个块
            {
                size_t i;
                obj* my_free_list;
                obj* p;
                for(i = size; i <= (size_t) MAX_BYTES; i+= (size_t) ALIGN)
                {
                    my_free_list = S_free_list + S_freelist_index(i);
                    p = *my_free_list;
                    if(p != 0)
                    {
                        * my_free_list = p->M_free_list_link;//从链表中移除当前块
                        S_start_free = (char*) p;//重置内存池起始地址
                        S_end_free = S_start_free + i;//设置内存池结束地址
                        return S_chunk_alloc(size, nobjs);//递归重新分配
                    }
                }
                S_end_free = 0;//标记内存池不可用
                S_start_free = (char*) malloc_alloc::allocate(bytes_to_get);
            }
            S_heap_size += bytes_to_get;
            S_end_free = S_start_free + bytes_to_get;//设置新的内存池边界
            return S_chunk_alloc(size, nobjs);
        }
    }

    /**
     * @brief 构造空闲链表：为指定大小 n的空闲链表重新填充内存块
     * 
     * @tparam threads 
     * @tparam inst 
     * @param n 
     * @return void* 
     */
    template<bool threads, int inst>
    void* default_alloc_template<threads, inst>::S_refill(size_t n)
    {
        int nobjs = 20;
        char* chunk = S_chunk_alloc(n, nobjs);//从内存池分配20个大小为n的块
        obj* my_free_list;
        obj* result;
        obj* current_obj;
        obj* next_obj;
        int i;

        if(nobjs == 1) return chunk;
        //初始化空闲链表
        my_free_list = S_free_list + S_freelist_index(n);//获得对应大小的空闲链表头
        result = (obj*) chunk;//第一个块返回给调用者
        *my_free_list = next_obj = (obj*)(chunk + n);//第二个块作为链表头
        //构造空闲链表
        for(i=0; ; i++)
        {
            current_obj =next_obj;
            next_obj = (obj*) ((char*) next_obj +n);
            if(nobjs -1 ==i)//最后一个块
            {
                current_obj ->M_free_list_link = 0;
                break;
            }else{
                current_obj->M_free_list_link = next_obj;//连接到下一个块
            }
        }
        return result;
    }

    /**
     * @brief 首先判断区块大小，大于128bytes调用第一级配置器，小于128bytes检查对应的free-list。如果free-list内有可用的区块，直接使用；
     * 若没有，就将大小调至8倍数边界，然后调用refill()，准备为free-list重新填充空间
     * @tparam threads 
     * @tparam inst 
     * @param n 
     * @return void* 
     */
    template<bool threads, int inst>
    void* default_alloc_template<threads, inst>::allocate(size_t n)
    {
        void* ret =0;//返回给用户的块
        if(n > (size_t) MAX_BYTES)//超过128字节，使用第一级分配器
        {
            ret = malloc_alloc::allocate(n);
        }else{//没有超过128字节,使用第二级分配器
            obj* my_free_list = S_free_list + S_freelist_index(n);
            
            obj* result  = *my_free_list;
            if(result == 0)//链表为空，调用S_fill重新填充链表
            {
                ret = S_refill(S_round_up(n));
            }else{//链表非空：冲链表头部取出第一个块
                * my_free_list = result ->M_free_list_link;
                ret = result;
            }
        }
        return ret;
    }

    /**
     * @brief 内存释放，并插入对应的空闲链表（头插法）
     * 
     * @tparam threads 
     * @tparam inst 
     * @param p 内存指针 
     * @param n 大小n
     * @return void* 
     */
    template<bool threads, int inst>
    void* default_alloc_template<threads, inst>::deallocate(void* p, size_t n)
    {
        if( n > (size_t) MAX_BYTES)
        {//大于128
            malloc_alloc::deallocate(p, n);
        }else{//处理小块内存
            obj* my_free_list = S_free_list + S_freelist_index(n);//对于大小为n的，对应的空闲大小链表头指针
            //将p内存插入对应的空闲链表
            obj* q = (obj*) p;
            q->M_free_list_link = *my_free_list;
            *my_free_list = q;

        }
    }

    /**
     * @brief 重新分配内存
     * 
     * @tparam threads 
     * @tparam inst 
     * @param p 原内存指针p
     * @param old_size 旧内存大小
     * @param new_size 新内存大小
     * @return void* 
     */
    template<bool threads, int inst>
    void* default_alloc_template<threads, inst>::reallocate(void* p, size_t old_size, size_t new_size)
    {
        void* result;//新分配的内存指针
        size_t copy_size;
        if(old_size > (size_t) MAX_BYTES && new_size > (size_t) MAX_BYTES)
        {//内存大于128直接调用realloc
            return realloc(p, new_size);
        }else{//内存小于128
            result = allocate(new_size);
            copy_size = new_size > old_size? old_size:new_size;//计算需要复制的数据大小
            memcpy(result, p, copy_size);//复制到新地址
            deallocate(p, old_size);//释放原内存
            return result;
        }
    }


}




#endif