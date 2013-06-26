/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2013 Agust�n Berg�

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_SPIRIT_ATTR_JUL_23_2008_0956AM
#define BOOST_SPIRIT_ATTR_JUL_23_2008_0956AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/traits/container_traits.hpp>
#include <boost/spirit/home/support/traits/move_to.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>

namespace boost { namespace spirit { namespace x3
{
    template <typename Value>
    struct attr_parser : parser<attr_parser<Value>>
    {
        typedef Value attribute_type;

        static bool const has_attribute =
            !is_same<unused_type, attribute_type>::value;
        static bool const handles_container =
            traits::is_container<attribute_type>::value;
        
        attr_parser(Value const& value)
          : value_(value) {}
        attr_parser(Value&& value)
          : value_(std::move(value)) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr_) const
        {
            //~ BUGBUG: no rvalue-ref support in move_to
            //~ spirit::traits::move_to(value_, attr_);
            Value copy(value_);
            traits::move_to(copy, attr_);
            return true;
        }

        Value value_;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        attr_parser& operator= (attr_parser const&);
    };
    
    template <typename Value, std::size_t N>
    struct attr_parser<Value[N]> : parser<attr_parser<Value[N]>>
    {
        typedef Value attribute_type[N];

        static bool const has_attribute =
            !is_same<unused_type, attribute_type>::value;
        static bool const handles_container = true;
        
        attr_parser(Value const (&value)[N])
        {
            std::copy(value + 0, value + N, value_ + 0);
        }
        attr_parser(Value (&&value)[N])
        {
            std::move(value + 0, value + N, value_ + 0);
        }

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr_) const
        {
            traits::move_to(value_ + 0, value_ + N, attr_);
            return true;
        }

        Value value_[N];

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        attr_parser& operator= (attr_parser const&);
    };
    
    template <typename Value>
    struct get_info<attr_parser<Value>>
    {
        typedef std::string result_type;
        std::string operator()(attr_parser<Value> const& /*p*/) const
        {
            return "attr";
        }
    };

    struct attr_gen
    {
        template <typename Value>
        attr_parser<typename remove_cv<
            typename remove_reference<Value>::type>::type>
        operator()(Value&& value) const
        {
            typedef attr_parser<typename remove_cv<
                typename remove_reference<Value>::type>::type>
            result_type;

            return result_type(std::forward<Value>(value));
        }
        
        template <typename Value, std::size_t N>
        attr_parser<typename remove_cv<Value>::type[N]>
        operator()(Value (&value)[N]) const
        {
            typedef attr_parser<typename remove_cv<Value>::type[N]>
            result_type;

            return result_type(value);
        }
        template <typename Value, std::size_t N>
        attr_parser<typename remove_cv<Value>::type[N]>
        operator()(Value (&&value)[N]) const
        {
            typedef attr_parser<typename remove_cv<Value>::type[N]>
            result_type;

            return result_type(value);
        }
    };

    attr_gen const attr = attr_gen();
}}}

#endif
