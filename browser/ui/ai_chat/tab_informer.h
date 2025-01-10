// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_AI_CHAT_TAB_INFORMER_H_
#define BRAVE_BROWSER_UI_AI_CHAT_TAB_INFORMER_H_

#include <string_view>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "brave/components/ai_chat/core/common/mojom/tab_informer.mojom-forward.h"
#include "brave/components/ai_chat/core/common/mojom/tab_informer.mojom.h"
#include "chrome/browser/ui/browser_tab_strip_tracker.h"
#include "chrome/browser/ui/browser_tab_strip_tracker_delegate.h"
#include "chrome/browser/ui/tabs/tab_strip_model_observer.h"
#include "content/public/browser/web_contents.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote_set.h"

namespace ai_chat {

class TabInformer : public mojom::TabInformer,
                    public BrowserTabStripTrackerDelegate,
                    public TabStripModelObserver {
 public:
  using State = std::vector<mojom::WindowPtr>;
  // Note: |TabInformer| should not outlive |owner_contents_|.
  TabInformer(mojo::PendingReceiver<mojom::TabInformer> receiver,
              content::WebContents* owner_contents);
  ~TabInformer() override;

  static content::WebContents* GetFromTab(const mojom::TabPtr& tab);

  // mojom::TabInformer
  void AddListener(mojo::PendingRemote<mojom::TabListener> listener) override;

  // TabStripModelObserver:
  void OnTabStripModelChanged(
      TabStripModel* tab_strip_model,
      const TabStripModelChange& change,
      const TabStripSelectionChange& selection) override;
  void TabChangedAt(content::WebContents* contents,
                    int index,
                    TabChangeType change_type) override;

  // BrowserTabStripTrackerDelegate:
  bool ShouldTrackBrowser(Browser* browser) override;

 private:
  State GetState();
  void NotifyListeners();
  void NotifyListener(mojom::TabListener* listener, State state);

  raw_ptr<content::WebContents> owner_contents_;

  mojo::Receiver<mojom::TabInformer> receiver_;
  mojo::RemoteSet<mojom::TabListener> listeners_;
  BrowserTabStripTracker tracker_{this, this};
};

}  // namespace ai_chat

#endif  // BRAVE_BROWSER_UI_AI_CHAT_TAB_INFORMER_H_
