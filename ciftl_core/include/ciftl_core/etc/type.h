#pragma once
#include <cstdint>
#include <vector>
#include <array>

namespace ciftl
{
    typedef uint8_t byte;
    typedef std::vector<byte> ByteVector;

    template <size_t ARRAY_SIZE>
    using ByteArray = std::array<byte, ARRAY_SIZE>;

    typedef uint32_t Crc32Value;
    typedef ByteArray<sizeof(Crc32Value)> Crc32ByteArray;

    template <class T1, class T2>
    bool compare(const T1 &t1, const T2 &t2)
    {
        if (t1.size() != t2.size())
        {
            return false;
        }
        for (size_t i = 0; i < t1.size(); i++)
        {
            if (t1[i] != t2[i])
            {
                return false;
            }
        }
        return true;
    }

    template <class T>
    T auto_cast(const byte *data)
    {
        T t;
        memcpy(reinterpret_cast<byte *>(&t), data, sizeof(T));
        return t;
    }

    template <class T1, class T2>
    T1 auto_cast(const T2 &t2)
    {
    }

    template <>
    inline ByteVector auto_cast(const std::string &str)
    {
        ByteVector res(str.size());
        memcpy(res.data(), str.c_str(), str.size());
        return res;
    }

    template <>
    inline std::string auto_cast(const ByteVector &vec)
    {
        std::string res(vec.size(), '\0');
        memcpy(res.data(), vec.data(), vec.size());
        return res;
    }

    template <>
    inline ByteArray<sizeof(uint32_t)> auto_cast(const uint32_t &n)
    {
        ByteArray<sizeof(uint32_t)> res;
        memcpy(res.data(), reinterpret_cast<const void *>(&n), sizeof(uint32_t));
        return res;
    }

    template <>
    inline ByteVector auto_cast(const uint32_t &n)
    {
        ByteVector res(sizeof(uint32_t));
        memcpy(res.data(), reinterpret_cast<const void *>(&n), sizeof(uint32_t));
        return res;
    }

    template <>
    inline uint32_t auto_cast(const ByteArray<sizeof(uint32_t)> &vec)
    {
        uint32_t res;
        memcpy(reinterpret_cast<void *>(&res), vec.data(), sizeof(uint32_t));
        return res;
    }

    template <class T>
    ByteVector combine(T t)
    {
        ByteVector res;
        size_t t_size = t.size();
        res.resize(t_size);
        memcpy(res.data(), t.data(), t_size);
        return res;
    }

    template <class T, class... ARGS>
    ByteVector combine(T t, ARGS... args)
    {
        ByteVector res;
        size_t t_size = t.size();
        res.resize(t_size);
        memcpy(res.data(), t.data(), t_size);
        ByteVector next_res = combine(args...);
        res.resize(res.size() + next_res.size());
        memcpy(res.data() + t_size, next_res.data(), next_res.size());
        return res;
    }

    template <class SRC, class T>
    void divide(SRC &src, size_t start_idx, T &t)
    {
        memcpy(t.data(), src.data() + start_idx, t.size());
        return;
    }

    template <class SRC, class T, class... ARGS>
    void divide(SRC &src, size_t start_idx, T &t, ARGS &...args)
    {
        memcpy(t.data(), src.data() + start_idx, t.size());
        divide(src, start_idx + t.size(), args...);
        return;
    }

}
