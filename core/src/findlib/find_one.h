/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2018-2018 Bareos GmbH & Co. KG

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
#ifndef FINDLIB_FIND_ONE_H_
#define FINDLIB_FIND_ONE_H_

DLL_IMP_EXP int find_one_file(JobControlRecord *jcr, FindFilesPacket *ff,
                  int handle_file(JobControlRecord *jcr, FindFilesPacket *ff_pkt, bool top_level),
                  char *p, dev_t parent_device, bool top_level);
DLL_IMP_EXP int term_find_one(FindFilesPacket *ff);
DLL_IMP_EXP bool has_file_changed(JobControlRecord *jcr, FindFilesPacket *ff_pkt);
DLL_IMP_EXP bool check_changes(JobControlRecord *jcr, FindFilesPacket *ff_pkt);

#endif // FINDLIB_FIND_ONE_H_
