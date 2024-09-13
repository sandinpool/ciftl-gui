#pragma once
#include <sstream>
#include <fmt/core.h>

template <class T>
std::string auto_format(const T &vec)
{
    std::string res;
    char tmp[5];
    for (size_t i = 0; i < vec.size(); i++)
    {
        sprintf(tmp, "%02X", (int)vec[i]);
        res += tmp;
        res += ' ';
        if (i % 16 == 15)
        {
            res += '\n';
        }
    }
    return res;
}

template <class T>
bool is_equal(std::vector<T> const &a, std::vector<T> const &b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool is_equal(const T *a, const T *b, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}
