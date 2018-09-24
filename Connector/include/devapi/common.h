/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQLX_COMMON_H
#define MYSQLX_COMMON_H


#include "../mysql_common.h"
#include <string>
#include <stdexcept>
#include <ostream>
#include <memory>
#include <forward_list>
#include <string.h>  // for memcpy
#include <utility>   // std::move etc

namespace cdk {
namespace foundation {

class bytes;
class string;

}}  // cdk::foundation


namespace mysqlx {

typedef unsigned char byte;


namespace internal {

/*
  List_initializer class is used to initialize user std::vector, std::list or
  own list implementations, as long as initialized by iterators of defined type
*/

template <typename T>
struct List_init
{
   std::forward_list<T> m_data;

   template <typename L>
   List_init(L&& list)
     : m_data(std::move(list.begin()), std::move(list.end()))
   {}

   //Special case for forward_list using move constructor
   List_init(std::forward_list<T>&& list)
     : m_data(std::move(list))
   {}

   template<typename U>
   operator U()
   {
     return U(m_data.begin(), m_data.end());
   }

   operator std::forward_list<T>()
   {
     return std::move(m_data);
   }
};

}  // internal


/**
  A wrapper around std::wstring that can perform
  conversions from/to different character encodings
  used by MySQL.

  Currently only utf-8 encoding is supported.

  @ingroup devapi_aux
*/

class string : public std::wstring
{

  struct Impl
  {
    PUBLIC_API static std::string to_utf8(const string&);
    PUBLIC_API static void from_utf8(string&, const std::string&);
  };

public:

  string() {}

  string(const wchar_t *other)
  {
    if (other)
      assign(other);
  }
  string(const std::wstring &other) : std::wstring(other) {}
  string(std::wstring &&other) : std::wstring(std::move(other)) {}

  string& operator=(const std::wstring &other)
  {
    assign(other);
    return *this;
  }

  // TODO: make utf8 conversions explicit

  string(const char *other)
  {
    if (!other)
      return;
    std::string utf8(other);
    Impl::from_utf8(*this, utf8);
  }

  string(const std::string &other)
  {
    Impl::from_utf8(*this, other);
  }

  // conversion to utf-8

  operator std::string() const
  {
    return Impl::to_utf8(*this);
  }

};


inline
std::ostream& operator<<(std::ostream &out, const string &str)
{
  const std::string utf8(str);
  out << utf8;
  return out;
}


typedef unsigned long col_count_t;
typedef unsigned long row_count_t;


/**
  Class representing a region of memory holding raw bytes.

  Method `begin()` returns pointer to the first byte in the
  region, `end()` to one past the last byte in the region.

  @note An instance of `bytes` does not store the bytes -
  it merely describes a region of memory and is equivalent
  to a pair of pointers. It is very cheap to copy `bytes` and
  pass them by value.

  @note This class extends std::pair<byte *, size_t> to make
  it consistent with how memory regions are described by
  std::get_temporary_buffer(). It is also possible to initialize
  a `bytes` instance by buffer returned from
  std::get_temporary_buffer(), as follows:

    bytes buf = std::get_temporary_buffer<byte>(size);

  @ingroup devapi_aux
*/

class bytes : public std::pair<byte*, size_t>
{

public:

  bytes(byte *beg_, byte *end_)
    : pair(beg_, end_ - beg_)
  {}

  bytes(byte *beg, size_t len) : pair(beg, len)
  {}

  bytes(const char *str) : pair((byte*)str,0)
  {
    if (NULL != str)
      second = strlen(str);
  }

  bytes(std::pair<byte*, size_t> buf) : pair(buf)
  {}

  bytes() : pair(NULL, 0)
  {}

  bytes(const bytes &) = default;

  virtual byte* begin() const { return first; }
  virtual byte* end() const { return first + second; }

  size_t length() const { return second; }
  size_t size() const { return length(); }

  class Access;
  friend Access;
};


namespace internal {

  class PUBLIC_API nocopy
  {
  public:
    nocopy(const nocopy&) = delete;
    nocopy& operator=(const nocopy&) = delete;
  protected:
    nocopy() {}
  };


  class PUBLIC_API Printable
  {
    virtual void print(std::ostream&) const = 0;
    friend std::ostream& operator<<(std::ostream&, const Printable&);
  };

  inline
  std::ostream& operator<<(std::ostream &out, const Printable &obj)
  {
    obj.print(out);
    return out;
  }


  /*
    Defined here because std::enable_if_t is not defined on all platforms on
    which we build (clang is missing one). Note: it is C++14 feature.
  */

  template<bool Cond, typename T = void>
  using enable_if_t = typename std::enable_if<Cond, T>::type;

}  // internal


/*
  Global unique identifiers for documents.

  TODO: Windows GUID type
*/

class PUBLIC_API GUID : public internal::Printable
{
  char m_data[32];

  void set(const char *data)
  {
    // Note: Windows gives compile warnings when using strncpy
    for(unsigned i=0; data[i] && i < sizeof(m_data); ++i)
      m_data[i]= data[i];
  }

  INTERNAL void set(const std::string &data) { set(data.c_str()); }

public:

  GUID()
  {
    memset(m_data, 0, sizeof(m_data));
  }

  template <typename T> GUID(T data) { set(data); }
  template<typename T>  GUID& operator=(T data) { set(data); return *this; }

  operator std::string() const
  {
    return std::string(m_data, m_data + sizeof(m_data));
  }

  void generate();

  void print(std::ostream &out) const
  {
    out << std::string(*this);
  }
};


using std::out_of_range;

/**
  Base class for connector errors.

  @internal
  TODO: Derive from std::system_error and introduce proper
  error codes.
  @endinternal

  @ingroup devapi
*/

DLL_WARNINGS_PUSH

class PUBLIC_API Error : public std::runtime_error
{

DLL_WARNINGS_POP

public:

  Error(const char *msg)
    : std::runtime_error(msg)
  {}
};


inline
std::ostream& operator<<(std::ostream &out, const Error &e)
{
  out << e.what();
  return out;
}

#define CATCH_AND_WRAP \
  catch (const ::mysqlx::Error&) { throw; }       \
  catch (const std::exception &e)                 \
  { throw ::mysqlx::Error(e.what()); }            \
  catch (const char *e)                           \
  { throw ::mysqlx::Error(e); }                   \
  catch (...)                                     \
  { throw ::mysqlx::Error("Unknown exception"); } \


inline
void throw_error(const char *msg)
{
  throw ::mysqlx::Error(msg);
}


namespace internal {

  /*
    Type trait which checks if std::begin()/end() work on objects of given
    class C, so that it can be used as a range to iterate over.

    TODO: Make it work also with user-defined begin()/end() functions.
    TODO: Make it work with plain C arrays. For example:

       int vals[] = { 1, 2, 3 }
       process_args(data, vals)
  */

  template <class C>
  class is_range
  {
    /*
      Note: This overload is taken into account only if std::begin(X) and
      std::end(X) expressions are valid.
    */
    template <class X>
    static std::true_type test(decltype(std::begin(*((X*)nullptr)))*,
                               decltype(std::end(*((X*)nullptr)))*);

    template <class X>
    static std::false_type test(...);

  public:

    static const bool value
      = std::is_same<
          std::true_type, decltype(test<C>(nullptr, nullptr))
        >::value;
  };


  /*
    Class template to be used for uniform processing of variable argument lists
    in public API methods. This template handles the cases where arguments
    are specified directly as a list:

      method(arg1, arg2, ..., argN)

    or they are taken from a container such as std::list:

      method(container)

    or they are taken from a range of items described by two iterators:

      method(begin, end)

    A class B that is using this template to define a varargs method 'foo'
    should define it as follows:

      template <typename... T>
      X foo(T... args)
      {
        Args_processor<B>::process_args(m_impl, args...);
        return ...;
      }

    Process_args() is a static method of Args_processor<> and therefore
    additional context data is passed to it as the first argument. By default
    this context is a pointer to internal implementation object, as defined
    by the base class B. The process_args() methods does all the necessary
    processing of the variable argument list, passing the resulting items
    one-by-one to B::process_one() method. Base class B must define this
    static method, which takes the context and one data item as arguments.
    B::process_one() method can have overloads that handle different types
    of data items.

    See devapi/detail/crud.h for usage examples.
  */

  template <class Base, class D = typename Base::Impl*>
  class Args_processor
  {
  public:

    /*
      Check if item of type T can be passed to Base::process_one()
    */

    template <typename T>
    class can_process
    {
      template <typename X>
      static std::true_type
      test(decltype(Base::process_one(*(D*)nullptr,*(X*)nullptr))*);

      template <typename X>
      static std::false_type test(...);

    public:

      static const bool value
        = std::is_same<std::true_type, decltype(test<T>(nullptr))>::value;
    };

  public:

    /*
      Process items from a container.
    */

    template <
      typename C,
      typename = enable_if_t<is_range<C>::value>,
      typename = enable_if_t<!can_process<C>::value>
    >
      static void process_args(D data, C container)
    {
      for (auto el : container)
      {
        Base::process_one(data, el);
      }
    }

    /*
      If process_args(data, a, b) is called and a,b are of the same type It
      which can not be passed to Base::process_one() then we assume that a and
      b are iterators that describe a range of elements to process.
    */

    template <
      typename It,
      typename = enable_if_t<!can_process<It>::value>
    >
      static void process_args(D data, const It &begin, const It &end)
    {
      for (It it = begin; it != end; ++it)
      {
        Base::process_one(data, *it);
      }
    }

    /*
      Process elements given as a varargs list.
    */

    template <
      typename T,
      typename... R,
      typename = enable_if_t<can_process<T>::value>
    >
      static void process_args(D data, T first, R... rest)
    {
      process_args1(data, first, rest...);
    }

  private:

    template <
      typename T,
      typename... R,
      typename = enable_if_t<can_process<T>::value>
    >
      static void process_args1(D data, T first, R... rest)
    {
      Base::process_one(data, first);
      process_args1(data, rest...);
    }

    static void process_args1(D)
    {}

  };

}  // internal namespace



}  // mysqlx


#endif
