// (C) 2008 Wesley Steiner

#pragma once

#include <stdlib.h>
#include <string>

template <typename T> class BoxBase
{
public:
    explicit BoxBase(T value) : m_value(value) { }
protected:
    T m_value;
};

template <typename T> class Box { };

template < > class Box <int> : public BoxBase<int>
{
public:
    explicit Box(int value) : BoxBase(value) { }
    std::string ToString() const 
    { 
        TCHAR buf[50];
        ::_itot_s(m_value, buf, sizeof(buf), 10); 
        return buf;
    }
};

template < > class Box <size_t> : public BoxBase<size_t>
{
public:
    explicit Box(size_t value) : BoxBase(value) { }
    std::string ToString() const 
    { 
        TCHAR buf[50];
        ::_ultot_s(static_cast <unsigned long> (m_value), buf, sizeof(buf), 10); 
        return buf;
    }
};
