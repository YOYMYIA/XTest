//
// Created by 76426 on 2025/11/16.
//

#ifndef XPLAN_PLATFORM_DEFS_H
#define XPLAN_PLATFORM_DEFS_H

// This file is used to define standard macros and constants

namespace xplat
{

/** @cond */
#define XPLAN_JOIN_MACRO_HELPER(a, b) a ## b
#define XPLAN_STRINGIFY_MACRO_HELPER(a) #a
/** @endcond */

#define XPLAN_JOIN_MACRO(item1, item2)  XPLAN_JOIN_MACRO_HELPER (item1, item2)
#define XPLAN_STRINGIFY(item)  XPLAN_STRINGIFY_MACRO_HELPER (item)

// The macro for deleting a class's copy constructor and copy assignment operator.
#define XPLAN_DECLARE_NON_COPYABLE(className) \
    className (const className&) = delete;     \
    className& operator= (const className&) = delete;

// The macro for deleting a class's move constructor and move assignment operator.
#define XPLAN_DECLARE_NON_MOVEABLE(className) \
    className (className&&) = delete;\
    className& operator= (className&&) = delete;

// The macro for disable the use of new/delete tp allocate the object on the heap.
#define XPLAN_PREVENT_HEAP_ALLOCATION \
   private: \
    static void* operator new (size_t) = delete; \
    static void operator delete (void*) = delete;


}// xplat

#endif //XPLAN_PLATFORM_DEFS_H
