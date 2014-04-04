// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_EXPECTED_OPTIONAL_MONAD_HPP
#define BOOST_EXPECTED_OPTIONAL_MONAD_HPP

#include <boost/functional/monads/categories/pointer_like.hpp>
#include <boost/functional/monads/categories/valued_and_errored.hpp>
#include <boost/functional/monads/monad_error.hpp>
#include <boost/optional.hpp>
#include <boost/mpl/identity.hpp>
#include <type_traits>

namespace boost
{
  namespace monads
  {

    template <class T>
    struct is_monad<optional<T> > : std::true_type {};

    template <class T, class U>
    struct bind<optional<T>, U> : mpl::identity<optional<U> > {};

    template <class T>
    struct value_category<optional<T> > : mpl::identity<category::pointer_like> { };
    template <class T>
    struct unexpected_traits< optional<T> > {
      template< class M >
      using type = none_t;

      template <class M, class E>
      static M make_error(E&&)
      {
        return none;
      }
      static constexpr none_t get_unexpected(optional<T> const& ) { return none; }
      template< class M >
      static constexpr none_t error(M && m) { return none; }

    };
    template <class T>
    struct functor_category<optional<T> > : mpl::identity<category::valued_and_errored> { };
    template <class T>
    struct monad_category<optional<T> > : mpl::identity<category::valued_and_errored> { };

    template <class T>
    struct monad_error_traits<optional<T> >
    {
      template <class M>
      static constexpr auto value(M&& m) -> decltype(m.value()) { return m.value(); };

      template <class M, class E>
      static M make_error(E&&)
      {
        return none;
      }

      // f : E -> T
      // todo complete with the other prototypes
      // f : E -> void
      // f : E -> M
      template <class M, class F, class FR = decltype( std::declval<F>()( none ) ) >
      static BOOST_CONSTEXPR M
      catch_error(M&& m, F&& f)
      {
        typedef typename bind<decay_t<M>, FR>::type result_type;
#if ! defined BOOST_NO_CXX14_RELAXED_CONSTEXPR
        if(! has_value(m))
        {
          result_type(f(none));
        }
        return deref(m);
#else
        return (! has_value(m)
        ? result_type(f(none))
        : deref(m)
        );
#endif
      }
    };

  }
}


#endif // BOOST_EXPECTED_OPTIONAL_MONAD_HPP
