/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_K_ANONYMITY_SERVICE_K_ANONYMITY_TRUST_TOKEN_GETTER_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_K_ANONYMITY_SERVICE_K_ANONYMITY_TRUST_TOKEN_GETTER_H_

#define TryGetTrustTokenAndKey(...)    \
  TryGetTrustTokenAndKey(__VA_ARGS__); \
  void TryGetTrustTokenAndKey_ChromiumImpl(__VA_ARGS__)

#include "src/chrome/browser/k_anonymity_service/k_anonymity_trust_token_getter.h"  // IWYU pragma: export

#undef TryGetTrustTokenAndKey
#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_K_ANONYMITY_SERVICE_K_ANONYMITY_TRUST_TOKEN_GETTER_H_
