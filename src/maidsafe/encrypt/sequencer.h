﻿
/*******************************************************************************
 *  Copyright 2009-2011 maidsafe.net limited                                   *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the license   *
 *  file LICENSE.TXT found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ***************************************************************************//**
 * @file  sequencer.h
 * @brief random access buffer.
 * @date  2011-08-14
 */

#ifndef MAIDSAFE_ENCRYPT_SEQUENCER_H_
#define MAIDSAFE_ENCRYPT_SEQUENCER_H_

#include <map>
#include "maidsafe/encrypt/version.h"

#if MAIDSAFE_ENCRYPT_VERSION != 906
#  error This API is not compatible with the installed library.\
    Please update the library.
#endif


namespace maidsafe {
namespace encrypt {

typedef std::pair<char*, size_t> SequenceData;

class Sequencer {
 public:
  bool Add(size_t position, char *data, size_t length);
  SequenceData Peek(size_t position) {
    return PositionFromSequencer(position, false);
  }
  SequenceData Get(size_t position) {
    return PositionFromSequencer(position, true);
  }
  size_t PeekFirst(char *data, size_t *length) {
    return NextFromSequencer(data, length, false);
  }
  size_t GetFirst(char *data, size_t *length) {
    return NextFromSequencer(data, length, true);
  }
  bool empty() const { return sequencer_.empty(); }
 private:
  SequenceData PositionFromSequencer(size_t position, bool remove);
  size_t NextFromSequencer(char *data, size_t *length, bool remove);
  std::map<size_t, SequenceData> sequencer_;
};

}  // namespace encrypt
}  // namespace maidsafe

#endif  // MAIDSAFE_ENCRYPT_SEQUENCER_H_