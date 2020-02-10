/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2020-2020 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#include "include/bareos.h"
#include "cats/cats.h"
#include "dird/dbcopy/progress.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

struct Amount {
  bool is_valid{};
  std::size_t amount{};
};

static int AmountHandler(void* ctx, int fields, char** row)
{
  Amount* a = static_cast<Amount*>(ctx);
  std::istringstream iss(row[0]);
  iss >> a->amount;
  a->is_valid = !iss.fail();
  return 1;
}

Progress::Progress(BareosDb* db,
                   const std::string& table_name,
                   std::size_t limit_amount_of_rows_)
{
  Amount a;
  std::string query{"SELECT count(*) from " + table_name};

  if (!db->SqlQuery(query.c_str(), AmountHandler, &a)) {
    std::cout << "Could not select number of rows for: " << table_name
              << std::endl;
    return;
  }

  if (a.is_valid) {
    if (a.amount > limit_amount_of_rows_) {
      // commandline parameter
      a.amount = limit_amount_of_rows_;
    }
    full_amount_ = a.amount;
    state_.amount = a.amount;
    state_.start = system_clock::now();
    state_old_ = state_;
    is_valid_ = true;
  }
}

bool Progress::Increment()
{
  if (!is_valid_) {
    // if anything failed before do not abort, just omit the progress counter
    return false;
  }

  state_.amount = state_.amount != 0 ? state_.amount - 1 : 0;
  if ((state_.amount % 10000) != 0) { return false; }

  state_.ratio =
      Ratio::num - (state_.amount * Ratio::num) / (full_amount_ * Ratio::den);

  state_.start = system_clock::now();

  milliseconds duration =
      std::chrono::duration_cast<milliseconds>(state_.start - state_old_.start);

  state_.duration = (state_old_.duration + duration) / 2;

  auto remaining_time =
      (state_.duration) * (state_.amount / (state_old_.amount - state_.amount));

  state_.eta = system_clock::now() + remaining_time;

  bool changed =
      (state_.ratio != state_old_.ratio) || (state_.eta != state_old_.eta);

  state_old_ = state_;

  return changed;
}

std::string Progress::Eta() const
{
  std::ostringstream oss;
  std::time_t time = system_clock::to_time_t(state_.eta);
  oss << std::put_time(std::localtime(&time), "%F %T");
  return oss.str();
}
