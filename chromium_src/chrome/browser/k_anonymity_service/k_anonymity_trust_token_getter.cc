/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/k_anonymity_service/k_anonymity_trust_token_getter.h"

#define TryGetTrustTokenAndKey TryGetTrustTokenAndKey_ChromiumImpl
#include "src/chrome/browser/k_anonymity_service/k_anonymity_trust_token_getter.cc"
#undef TryGetTrustTokenAndKey

// We do not support TrustTokens aka PrivateStateTokens aka FeldgePst
void KAnonymityTrustTokenGetter::TryGetTrustTokenAndKey(
    TryGetTrustTokenAndKeyCallback callback) {
  std::move(callback).Run(std::nullopt);
  return;
}
