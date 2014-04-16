// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_MONADS_CATEGORIES_VALUED_AND_ERRORED2_HPP
#define BOOST_EXPECTED_MONADS_CATEGORIES_VALUED_AND_ERRORED2_HPP

#include <boost/config.hpp>
#include <boost/functional/monads/valued2.hpp>
#include <boost/functional/monads/errored2.hpp>
#include <boost/functional/monads/functor2.hpp>
#include <boost/functional/monads/monad2.hpp>
#include <boost/functional/monads/algorithms/have_value2.hpp>
#include <boost/functional/monads/algorithms/first_unexpected2.hpp>
#include <boost/functional/meta.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>

#if ! defined BOOST_NO_CXX14_RELAXED_CONSTEXPR
#if defined __clang__
#if ! __has_feature(cxx_relaxed_constexpr)
#define BOOST_NO_CXX14_RELAXED_CONSTEXPR
#endif
#else
#define BOOST_NO_CXX14_RELAXED_CONSTEXPR
#endif
#endif

#define REQUIRES(...) typename ::boost::enable_if_c<__VA_ARGS__, void*>::type = 0
#define T_REQUIRES(...) typename = typename ::boost::enable_if_c<(__VA_ARGS__)>::type

namespace boost
{
  namespace functional
  {
    namespace category
    {
      struct valued_and_errored {};
    }
    namespace functor
    {

      using namespace ::boost::functional::valued;
      using namespace ::boost::functional::errored;
      template <>
      struct functor_traits<category::valued_and_errored>
      {

        template <class F, class M0, class ...M,
        class FR = decltype( std::declval<F>()(deref(std::declval<M0>()), deref(std::declval<M>())...) )>
        static BOOST_CONSTEXPR auto fmap(F&& f, M0&& m0, M&& ...m) -> typename functional::rebind<decay_t<M0>, FR>::type
        {
          typedef typename functional::rebind<decay_t<M0>, FR>::type result_type;
          return have_value( std::forward<M0>(m0), std::forward<M>(m)... )
          ? result_type( std::forward<F>(f)( deref(std::forward<M0>(m0)), deref(std::forward<M>(m))... ) )
          : first_unexpected( std::forward<M0>(m0), std::forward<M>(m)... )
          ;
        }
      };
    }
    namespace monad
    {

      template <>
      struct monad_traits<category::valued_and_errored>
      {

        template <class M, class T>
        static BOOST_CONSTEXPR apply<M, T> make(T&& v)
        {
          return apply<M, T>(std::forward<T>(v));
        }

        template <class M, class F, class FR = decltype( std::declval<F>()( deref(std::declval<M>()) ) )>
        static BOOST_CONSTEXPR auto
        mbind(M&& m, F&& f,
            REQUIRES(boost::is_same<FR, void>::value)
        ) -> typename functional::rebind<decay_t<M>, FR>::type
        {
          typedef typename functional::rebind<decay_t<M>, FR>::type result_type;
#if ! defined BOOST_NO_CXX14_RELAXED_CONSTEXPR
          if(has_value(m))
          {
            f(deref(m));
            return result_type();
          }
          return get_unexpected(m);
#else
          return (has_value(m)
              ? (f(deref(m)), result_type() )
              : result_type( get_unexpected(m) )
          );
#endif
        }

        template <class M, class F, class FR = decltype( std::declval<F>()( deref(std::declval<M>()) ) )>
        static BOOST_CONSTEXPR auto
        mbind(M&& m, F&& f,
            REQUIRES((! boost::is_same<FR, void>::value
                    && ! boost::functional::monad::is_monad<FR>::value)
            )) -> typename functional::rebind<decay_t<M>, FR>::type
        {
          typedef typename functional::rebind<decay_t<M>, FR>::type result_type;
#if ! defined BOOST_NO_CXX14_RELAXED_CONSTEXPR
          if(has_value(m))
          {
            return result_type(f(deref(m)));
          }
          return get_unexpected(m);
#else
          return (has_value(m)
              ? result_type(f(deref(m)))
              : result_type( get_unexpected(m) )
          );
#endif
        }

        template <class M, class F, class FR = decltype( std::declval<F>()( deref(std::declval<M>()) ) )>
        static BOOST_CONSTEXPR auto
        mbind(M&& m, F&& f,
            REQUIRES( boost::functional::monad::is_monad<FR>::value )
        ) -> FR
        {
#if ! defined BOOST_NO_CXX14_RELAXED_CONSTEXPR
          if(has_value(m))
          {
            return f(deref(m));
          }
          return get_unexpected(m);
#else
          return (has_value(m)
              ? f(deref(m))
              : FR( get_unexpected(m) )
          );
#endif
        }
      };
    }
  }
}

#undef REQUIRES
#undef T_REQUIRES
#endif // BOOST_EXPECTED_MONADS_CATEGORIES_VALUED_AND_ERRORED_HPP