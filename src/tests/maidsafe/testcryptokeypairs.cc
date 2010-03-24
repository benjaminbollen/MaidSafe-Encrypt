/*
* ============================================================================
*
* Copyright [2010] maidsafe.net limited
*
* Description:  Test for CryptoKeyPairs class
* Version:      1.0
* Created:      2010-03-15-17.21.51
* Revision:     none
* Compiler:     gcc
* Author:       Fraser Hutchison (fh), fraser.hutchison@maidsafe.net
* Company:      maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file LICENSE.TXT found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/

#include <gtest/gtest.h>
#include "maidsafe/client/cryptokeypairs.h"
#include "maidsafe/client/packetfactory.h"

namespace maidsafe {

TEST(CryptoKeyPairsTest, BEH_MAID_GetCryptoKey) {
  CryptoKeyPairs ckp;
  crypto::RsaKeyPair kp;
  ASSERT_FALSE(ckp.GetKeyPair(&kp));
  ASSERT_TRUE(ckp.StartToCreateKeyPairs(1));
  ASSERT_TRUE(ckp.GetKeyPair(&kp));
  ASSERT_FALSE(kp.public_key().empty());
  ASSERT_FALSE(kp.private_key().empty());
}

TEST(CryptoKeyPairsTest, FUNC_MAID_GetMultipleCryptoKeys) {
  CryptoKeyPairs ckp;
  boost::int16_t no_of_keys = 20;
  std::vector<crypto::RsaKeyPair> kps;
  ASSERT_TRUE(ckp.StartToCreateKeyPairs(no_of_keys));
  ASSERT_FALSE(ckp.StartToCreateKeyPairs(no_of_keys));

  boost::this_thread::sleep(boost::posix_time::seconds(1));
  crypto::RsaKeyPair kp;
  while (ckp.GetKeyPair(&kp)) {
    kps.push_back(kp);
    ASSERT_FALSE(kp.public_key().empty());
    ASSERT_FALSE(kp.private_key().empty());
    kp.ClearKeys();
    boost::this_thread::sleep(boost::posix_time::seconds(1));
  }

  ASSERT_EQ(static_cast<size_t>(no_of_keys), kps.size());
}

TEST(CryptoKeyPairsTest, FUNC_MAID_ReuseObject) {
  CryptoKeyPairs ckp;
  boost::int16_t no_of_keys(5);
  std::vector<crypto::RsaKeyPair> kps;
  ASSERT_TRUE(ckp.StartToCreateKeyPairs(no_of_keys));
  ASSERT_FALSE(ckp.StartToCreateKeyPairs(no_of_keys));

  boost::this_thread::sleep(boost::posix_time::seconds(1));
  crypto::RsaKeyPair kp;
  boost::int16_t i(0), keys_rec(3);
  while (ckp.GetKeyPair(&kp)) {
    if (i == keys_rec)
      break;
    kps.push_back(kp);
    ASSERT_FALSE(kp.public_key().empty());
    ASSERT_FALSE(kp.private_key().empty());
    kp.ClearKeys();
    ++i;
    boost::this_thread::sleep(boost::posix_time::seconds(1));
  }

  while (!ckp.StartToCreateKeyPairs(no_of_keys))
    boost::this_thread::sleep(boost::posix_time::seconds(1));

  while (ckp.GetKeyPair(&kp)) {
    kps.push_back(kp);
    ASSERT_FALSE(kp.public_key().empty());
    ASSERT_FALSE(kp.private_key().empty());
    kp.ClearKeys();
    boost::this_thread::sleep(boost::posix_time::seconds(1));
  }

  ASSERT_EQ(static_cast<size_t>(no_of_keys + keys_rec), kps.size());
}

}  // namespace maidsafe
