#pragma once

#include "gtk.utils.hpp"

#include <utility>
#include <functional>

#include <boost/callable_traits.hpp>

namespace saucer::utils
{
    template <auto Deleter>
    template <typename T>
    constexpr void custom_deleter<Deleter>::operator()(T *ptr) const
    {
        using args_t = boost::callable_traits::args_t<decltype(Deleter)>;
        using arg_t  = std::tuple_element_t<0, args_t>;

        Deleter(reinterpret_cast<arg_t>(ptr));
    }

    template <typename T, auto Ref, auto Unref>
    template <auto Action>
    T *ref_ptr<T, Ref, Unref>::perform(T *data)
    {
        if (data)
        {
            std::invoke(Action, data);
        }

        return data;
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref>::ref_ptr() : m_data(nullptr)
    {
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref>::ref_ptr(T *data) : m_data(data)
    {
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref>::ref_ptr(const ref_ptr &other) : m_data(perform<Ref>(other.m_data))
    {
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref>::ref_ptr(ref_ptr &&other) noexcept : m_data(std::exchange(other.m_data, nullptr))
    {
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref>::~ref_ptr()
    {
        reset(nullptr);
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref> &ref_ptr<T, Ref, Unref>::operator=(const ref_ptr &other)
    {
        if (this != &other)
        {
            reset(perform<Ref>(other.m_data));
        }

        return *this;
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref> &ref_ptr<T, Ref, Unref>::operator=(ref_ptr &&other) noexcept
    {
        if (this != &other)
        {
            reset(std::exchange(other.m_data, nullptr));
        }

        return *this;
    }

    template <typename T, auto Ref, auto Unref>
    T *ref_ptr<T, Ref, Unref>::get() const
    {
        return m_data;
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref>::operator bool() const
    {
        return m_data != nullptr;
    }

    template <typename T, auto Ref, auto Unref>
    void ref_ptr<T, Ref, Unref>::reset(T *other)
    {
        perform<Unref>(m_data);
        m_data = other;
    }

    template <typename T, auto Ref, auto Unref>
    ref_ptr<T, Ref, Unref> ref_ptr<T, Ref, Unref>::ref(T *data)
    {
        return perform<Ref>(data);
    }
} // namespace saucer::utils
