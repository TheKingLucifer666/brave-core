/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_HTML5_NTT_SOURCE_H_
#define BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_HTML5_NTT_SOURCE_H_

#include <optional>
#include <string>

#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/url_data_source.h"

namespace base {
class FilePath;
}  // namespace base

namespace ntp_background_images {

class NTPBackgroundImagesService;

// This serves HTML5 NTT data.
class HTML5NTTSource : public content::URLDataSource {
 public:
  explicit HTML5NTTSource(NTPBackgroundImagesService* service);

  ~HTML5NTTSource() override;

  HTML5NTTSource(const HTML5NTTSource&) = delete;
  HTML5NTTSource& operator=(const HTML5NTTSource&) = delete;

 private:
  FRIEND_TEST_ALL_PREFIXES(HTML5NTTSourceTest, HTML5NTTTest);

  // content::URLDataSource overrides:
  std::string GetSource() override;
  void StartDataRequest(const GURL& url,
                        const content::WebContents::Getter& wc_getter,
                        GotDataCallback callback) override;
  std::string GetMimeType(const GURL& url) override;
  bool AllowCaching() override;

  base::FilePath GetLocalFilePathFor(const std::string& path);
  void GetFile(const base::FilePath& file_path, GotDataCallback callback);
  void OnGotFile(GotDataCallback callback, std::optional<std::string> input);
  bool IsValidPath(const std::string& path) const;

  raw_ptr<NTPBackgroundImagesService> service_ = nullptr;  // not owned
  base::WeakPtrFactory<HTML5NTTSource> weak_factory_;
};

}  // namespace ntp_background_images

#endif  // BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_HTML5_NTT_SOURCE_H_
