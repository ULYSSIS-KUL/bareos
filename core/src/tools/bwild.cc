/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2006-2006 Free Software Foundation Europe e.V.
   Copyright (C) 2016-2022 Bareos GmbH & Co. KG

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
// Kern Sibbald, MMVI
/**
 * @file
 * Test program for testing wild card expressions
 */

#include "include/bareos.h"
#include "lib/cli.h"
#include "lib/fnmatch.h"

int main(int argc, char* const* argv)
{
  setlocale(LC_ALL, "");
  tzset();
  bindtextdomain("bareos", LOCALEDIR);
  textdomain("bareos");

  CLI::App bwild_app;
  InitCLIApp(bwild_app, "The Bareos Wildcard tool.");

  AddDebugOptions(bwild_app);

  std::string fname{};
  bwild_app
      .add_option("-f,--filename", fname, "Specify file or data to be matched.")
      ->required();

  int ic = 0;
  bwild_app.add_flag(
      "-i,--ignore-case", [&ic](bool) { ic = FNM_CASEFOLD; },
      "Use case insensitive match.");

  bool no_linenos = false;
  bwild_app.add_flag("-l,--suppress-linenumbers", no_linenos,
                     "Suppress line numbers.");

  bool match_only = true;
  bwild_app.add_flag(
      "-n,--not-match", [&match_only](bool) { match_only = false; },
      "Print line that do not match.");

  CLI11_PARSE(bwild_app, argc, argv);

  OSDependentInit();

  char data[1000];
  char pat[500];
  FILE* fd;
  int lineno;
  int rc;

  for (;;) {
    printf("Enter a wild-card: ");
    if (fgets(pat, sizeof(pat) - 1, stdin) == NULL) { break; }
    StripTrailingNewline(pat);
    if (pat[0] == 0) { exit(0); }
    fd = fopen(fname.c_str(), "r");
    if (!fd) {
      printf(_("Could not open data file: %s\n"), fname.c_str());
      exit(1);
    }
    lineno = 0;
    while (fgets(data, sizeof(data) - 1, fd)) {
      StripTrailingNewline(data);
      lineno++;
      rc = fnmatch(pat, data, ic);
      if ((match_only && rc == 0) || (!match_only && rc != 0)) {
        if (no_linenos) {
          printf("%s\n", data);
        } else {
          printf("%5d: %s\n", lineno, data);
        }
      }
    }
    fclose(fd);
  }
  exit(0);
}
