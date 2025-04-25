/*
 * MONA
 * Copyright (C) 1997-2013 Aarhus University.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the  Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335,
 * USA.
 */

#ifndef __OFFSETS_H
#define __OFFSETS_H

#include <assert.h>
#include "deque.h"



/**
 * This class keeps a Deque where each index holds itself as a value.
 * I.e., the buffer of offsetMap looks like: [0, 1, 2, 3, ...].
 * It seems like there was some attempt to allow reordering of the offsetMap
 * to improve construction of automata,
 * but this was abandoned.
 * Thus, we can optimize this entire class into no-ops.
 *
 * To disable this optimization, comment the following define
 */
#define OPTIMIZE_OFFSETS


class Offsets {
public:
#ifndef OPTIMIZE_OFFSETS
  void insert();
  void reorder();
  int off(unsigned int id) {
    assert(id<=max_offset);
    return offsetMap.get(id);
  }
  void clear();
#else
  void insert() {
    max_offset++;
  }
  void reorder() {}
  int off(unsigned int id) {
    return id;
  }
  void clear() {
    max_offset = 0;
  }
#endif

  int maxOffset() const {return max_offset;}


protected:
#ifndef OPTIMIZE_OFFSETS
  Deque<int> offsetMap;
#endif

  unsigned max_offset = 0;
};

#endif


