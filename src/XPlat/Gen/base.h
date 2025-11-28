#ifndef XPLAT_GEN_BASE_H
#define XPLAT_GEN_BASE_H

#include <functional>
namespace xplat
{

namespace gen
{

class Less 
{
public:
    template <class First, class Second>
    auto operator()(const First& first, const Second& second) const
        -> decltype(first < second)
    {
        return first < second;
    }
};

class Greater
{
public:
    template <class First, class Second>
    auto operator()(const First& first, const Second& second) const
        -> decltype(first > second)
    {
        return first > second;
    }
};

template <int n>
class Get
{
public:
    template <class Value>
    auto operator()(Value && value) const
        -> decltype(std::get<n>(std::forward<Value>(value)))
    {
        return std::get<n>(std::forward<Value>(value));
    }
};

template <class Class, class Result>
class MemberFunction
{
public:
    using MemberPtr = Result (Class::*)(); // member function pointer type

private:
    MemberPtr member_;

public:
    explicit MemberFunction(MemberPtr menber) : member_(menber) {}

    Result operator()(Class && object) const{return (object.*member_)();}
    Result operator()(Class & object) const{return (object.*member_)();}
    Result operator()(Class* object) const {return (object->*member_)();}
};

template <class Class, class Result>
class ConstMemberFunction
{
public:
    using MemberPtr = Result (Class::*)() const; // member function pointer type

private:
    MemberPtr member_;

public:
    explicit ConstMemberFunction(MemberPtr menber) : member_(menber) {}
    
    Result operator()(Class const & object) const{return (object.*member_)();}
    Result operator()(Class* object) const {return (object->*member_)();}
};

template <class Class, class FieldType>
class Field
{
public:
    using FieldPtr = FieldType Class::*; // field pointer type

private:
    FieldPtr field_;

public:
    explicit Field(FieldPtr field) : field_(field) {}

    FieldType operator()(Class && object) const{return std::move(object.*field_);}
    FieldType operator()(Class & object) const{return object.*field_;}
    FieldType operator()(Class* object) const {return object->*field_;}
    const FieldType operator()(const Class & object) const{return object.*field_;}
    const FieldType operator()(const Class* object) const {return object->*field_;}
};



} //namespace gen

} //namespace xplat


#endif