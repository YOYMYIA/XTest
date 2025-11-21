#ifndef XPLAT_GEN_CORE_H
#define XPLAT_GEN_CORE_H

namespace xplat{
namespace gen{


template <class Value, class Self>
class GenImpl;

template <class Self>
class Operator;

namespace detail 
{

template <class Self>
struct XBounded;

template <class First, class Second>
class Composed;

template <class Value, class First, class Second>
class Chain;


} //namespace detail
} //namespace gen
} //namespace xplat

#include "Core-inl.h"

#endif