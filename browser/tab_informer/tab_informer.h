// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_TAB_INFORMER_TAB_INFORMER_H_
#define BRAVE_BROWSER_TAB_INFORMER_TAB_INFORMER_H_

#include "content/public/browser/web_contents_observer.h"

namespace content {
class NavigationEntry;
}

namespace tab_informer {

class TabInformer : public content::WebContentsObserver {
 public:
  explicit TabInformer(content::WebContents* contents);
  ~TabInformer() override;

  TabInformer(const TabInformer&) = delete;
  TabInformer& operator=(const TabInformer&) = delete;

  void PrimaryPageChanged(content::Page& page) override;
  void TitleWasSet(content::NavigationEntry* entry) override;

 private:
  int32_t tab_id_ = 0;

  void UpdateTab();
};

}  // namespace tab_informer

#endif  // BRAVE_BROWSER_TAB_INFORMER_TAB_INFORMER_H_
