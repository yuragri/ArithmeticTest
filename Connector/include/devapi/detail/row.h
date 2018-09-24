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

#ifndef MYSQLX_DETAIL_ROW_H
#define MYSQLX_DETAIL_ROW_H

/**
  @file
  Classes used to access query and command execution results.
*/


#include "../common.h"
#include "../document.h"

#include <memory>


namespace mysqlx {
namespace internal {

class Row_result_detail;

class PUBLIC_API Row_detail
{
protected:

  class INTERNAL Impl;
  DLL_WARNINGS_PUSH
  std::shared_ptr<Impl>  m_impl;
  DLL_WARNINGS_POP

  Row_detail() = default;

  Row_detail(std::shared_ptr<Impl> &&impl)
  {
    m_impl = std::move(impl);
  }

  Impl& get_impl();

  const Impl& get_impl() const
  {
    return const_cast<Row_detail*>(this)->get_impl();
  }

  void ensure_impl();

  using Args_prc = Args_processor<Row_detail, std::pair<Impl*, col_count_t>*>;

  template<typename... Types>
  void set_values(col_count_t pos, Types... args)
  {
    ensure_impl();
    assert(m_impl);
    std::pair<Impl*, col_count_t> data{ m_impl.get(), pos };
    Args_prc::process_args(&data, args...);
  }

  static void process_one(std::pair<Impl*,col_count_t>*, const Value &val);

  friend Row_result_detail;
  friend Args_prc;
};


}  // internal namespace
}  // mysqlx

#endif
