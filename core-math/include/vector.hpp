#pragma once

#include <array>
#include <cstddef>
#include <numeric>

namespace bh {

template <typename Val, size_t Len>
class vector {
public:
    using data_t = Val;

    // constructors

    vector() = default;

    template <class... Values>
    explicit vector(Values... values)
        : data_ {
            values...,
        }
    {
        static_assert(sizeof...(values) == Len);
    }

    static vector ones() noexcept
    {
        vector res;
        for (size_t i = 0; i < Len; ++i) {
            res[i] = 1;
        }
        return res;
    }

    // accesors

    const data_t& operator[](size_t i) const noexcept
    {
        return data_[i];
    }

    data_t& operator[](size_t i) noexcept
    {
        return data_[i];
    }

    // compute utils

    data_t len() const noexcept
    {
        data_t res = 0;
        for (size_t i = 0; i < Len; ++i) {
            res += data_[i] * data_[i];
        }
        return std::sqrt(res);
    }

    vector norm() const noexcept
    {
        return div(*this, len());
    }

    data_t sum() const noexcept
    {
        return std::accumulate(data_.begin(), data_.end(), 0);
    }

    data_t mean() const noexcept
    {
        return sum() / Len;
    }

    // vector-to-vector operation

    static data_t dot(vector a, vector b) noexcept
    {
        data_t res = 0;
        for (size_t i = 0; i < Len; ++i) {
            res += a[i] * b[i];
        }
        return res;
    }

    static vector cross(vector a, vector b) noexcept
        requires(Len == 3)
    {
        vector result;
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
        return result;
    }

    static vector add(vector a, vector b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            a[i] += b[i];
        }
        return a;
    }

    static vector sub(vector a, vector b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            a[i] -= b[i];
        }
        return a;
    }

    static vector mul(vector a, vector b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            a[i] *= b[i];
        }
        return a;
    }

    static vector div(vector a, vector b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            a[i] /= b[i];
        }
        return a;
    }

    // vector-to-scalar operations

    static vector add(vector v, data_t b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            v[i] += b;
        }
        return v;
    }

    static vector sub(vector v, data_t b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            v[i] -= b;
        }
        return v;
    }

    static vector mul(vector v, data_t b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            v[i] *= b;
        }
        return v;
    }

    static vector div(vector v, data_t b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            v[i] /= b;
        }
        return v;
    }

    // comparators

    friend bool operator==(vector a, vector b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(vector a, vector b) noexcept
    {
        return !(a == b);
    }

    // operators

    friend vector operator+(const vector& a, const vector& b) noexcept
    {
        return vector::add(a, b);
    }

    friend vector operator-(const vector& a, const vector& b) noexcept
    {
        return vector::sub(a, b);
    }

    friend vector operator+(const vector& a, const data_t b) noexcept
    {
        return vector::add(a, b);
    }

    friend vector operator-(const vector& a, const data_t b) noexcept
    {
        return vector::sub(a, b);
    }

    friend vector operator*(const vector& a, const data_t b) noexcept
    {
        return vector::mul(a, b);
    }

    friend vector operator/(const vector& a, const data_t b) noexcept
    {
        return vector::div(a, b);
    }

    static vector min(vector a, vector b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            a[i] = std::min(a[i], b[i]);
        }
        return a;
    }

    static vector max(vector a, vector b) noexcept
    {
        for (size_t i = 0; i < Len; ++i) {
            a[i] = std::max(a[i], b[i]);
        }
        return a;
    }

private:
    std::array<Val, Len> data_;
};

}
