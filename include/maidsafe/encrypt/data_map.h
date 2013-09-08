/*  Copyright 2011 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.novinet.com/license

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_ENCRYPT_DATA_MAP_H_
#define MAIDSAFE_ENCRYPT_DATA_MAP_H_

#include <cstdint>
#include <string>
#include <vector>

#include "maidsafe/common/crypto.h"
#include "boost/shared_array.hpp"


namespace maidsafe {

namespace encrypt {

struct ChunkDetails {
  enum PreHashState { kEmpty, kOutdated, kOk };
  enum StorageState { kStored, kPending, kUnstored };
  ChunkDetails() : hash(),
                   pre_hash(),
                   old_n1_pre_hash(),
                   old_n2_pre_hash(),
                   pre_hash_state(kEmpty),
                   storage_state(kUnstored),
                   size(0) {}
  std::string hash;  // SHA512 of processed chunk
  byte pre_hash[crypto::SHA512::DIGESTSIZE];  // SHA512 of unprocessed src data
  // pre hashes of chunks n-1 and n-2, only valid if chunk n-1 or n-2 has
  // modified content
  std::vector<byte> old_n1_pre_hash, old_n2_pre_hash;
  // If the pre_hash hasn't been calculated, or if data has been written to the
  // chunk since the pre_hash was last calculated, pre_hash_ok should be false.
  PreHashState pre_hash_state;
  StorageState storage_state;
  uint32_t size;  // Size of unprocessed source data in bytes
};

struct DataMap {
  DataMap() : chunks(), content() {}

  bool operator==(const DataMap &other) const {
    if (!this)
      return false;

    if (content != other.content || chunks.size() != other.chunks.size())
      return false;

    for (uint32_t i = 0; i < chunks.size(); ++i)
      if (chunks[i].hash != other.chunks[i].hash)
        return false;

    return true;
  }

  bool operator!=(const DataMap &other) const { return !(*this == other); }

  std::vector<ChunkDetails> chunks;
  std::string content;  // Whole data item, if small enough
};

typedef std::shared_ptr<DataMap> DataMapPtr;

void SerialiseDataMap(const DataMap& data_map, std::string& serialised_data_map);
void ParseDataMap(const std::string& serialised_data_map, DataMap& data_map);

}  // namespace encrypt

}  // namespace maidsafe

#endif  // MAIDSAFE_ENCRYPT_DATA_MAP_H_
