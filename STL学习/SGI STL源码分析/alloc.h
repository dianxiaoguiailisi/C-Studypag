#ifndef __SGI_STL_ALLOC_H
#define __SGI_STL_ALLOC_H

#ifndef __STL_CONFIG_H
#include <stl_config.h>
#endif
#ifndef __SGI_STL_INTERNAL_ALLOC_H
#include <stl_alloc.h>
#endif

#ifdef __STL_USE_NAMESPACES

using __STD::__malloc_alloc_template; 
using __STD::malloc_alloc; 
using __STD::simple_alloc; 
using __STD::debug_alloc; 
using __STD::__default_alloc_template; 
using __STD::alloc; 
using __STD::single_client_alloc; 
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
using __STD::__malloc_alloc_oom_handler; 
#endif /* __STL_STATIC_TEMPLATE_MEMBER_BUG */
#ifdef __STL_USE_STD_ALLOCATORS 
using __STD::allocator;
#endif /* __STL_USE_STD_ALLOCATORS */

#endif /* __STL_USE_NAMESPACES */

#endif /* __SGI_STL_ALLOC_H */

// Local Variables:
// mode:C++
// End:
