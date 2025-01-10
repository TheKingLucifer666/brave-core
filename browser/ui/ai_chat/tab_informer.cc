// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/ai_chat/tab_informer.h"

#include <algorithm>
#include <iterator>
#include <utility>

#include "base/strings/utf_string_conversions.h"
#include "brave/components/ai_chat/core/common/constants.h"
#include "brave/components/ai_chat/core/common/mojom/tab_informer.mojom.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/tabs/tab_model.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/web_contents.h"
#include "mojo/public/cpp/bindings/receiver.h"

namespace ai_chat {

TabInformer::TabInformer(mojo::PendingReceiver<mojom::TabInformer> receiver,
                         content::WebContents* owner_contents)
    : owner_contents_(owner_contents), receiver_(this, std::move(receiver)) {
  tracker_.Init();
}

TabInformer::~TabInformer() = default;

content::WebContents* TabInformer::GetFromTab(const mojom::TabPtr& mojom_tab) {
  const tabs::TabHandle handle = tabs::TabHandle(mojom_tab->id);
  tabs::TabInterface* const tab = handle.Get();
  if (!tab) {
    return nullptr;
  }
  return tab->GetContents();
}

void TabInformer::AddListener(
    mojo::PendingRemote<mojom::TabListener> listener) {
  auto id = listeners_.Add(std::move(listener));

  NotifyListener(listeners_.Get(id), GetState());
}

void TabInformer::OnTabStripModelChanged(
    TabStripModel* tab_strip_model,
    const TabStripModelChange& change,
    const TabStripSelectionChange& selection) {
  NotifyListeners();
}

void TabInformer::TabChangedAt(content::WebContents* contents,
                               int index,
                               TabChangeType change_type) {
  NotifyListeners();
}

bool TabInformer::ShouldTrackBrowser(Browser* browser) {
  return browser->is_type_normal() &&
         browser->profile() ==
             Profile::FromBrowserContext(owner_contents_->GetBrowserContext());
}

TabInformer::State TabInformer::GetState() {
  State state;

  // Used to determine whether a given window is active.
  auto* active = chrome::FindLastActive();

  for (Browser* browser : *BrowserList::GetInstance()) {
    if (!ShouldTrackBrowser(browser)) {
      continue;
    }
    auto window = mojom::Window::New();
    window->is_active = browser == active;

    auto* tab_strip_model = browser->tab_strip_model();
    for (int i = 0; i < tab_strip_model->count(); ++i) {
      auto* contents = tab_strip_model->GetWebContentsAt(i);
      if (!contents) {
        continue;
      }

      if (!ai_chat::kAllowedContentSchemes.contains(
              contents->GetLastCommittedURL().scheme())) {
        continue;
      }

      auto tab_state = mojom::Tab::New();
      tab_state->id = tab_strip_model->GetTabHandleAt(i).raw_value();
      tab_state->title = base::UTF16ToUTF8(contents->GetTitle());
      tab_state->url = contents->GetVisibleURL();
      tab_state->content_id =
          contents->GetController().GetVisibleEntry()->GetUniqueID();
      window->tabs.push_back(std::move(tab_state));
    }

    state.push_back(std::move(window));
  }
  return state;
}

void TabInformer::NotifyListeners() {
  if (listeners_.empty()) {
    return;
  }
  auto state = GetState();
  for (auto& listener : listeners_) {
    State clone;
    std::ranges::transform(state, std::back_inserter(clone),
                           [](auto& window) { return window.Clone(); });
    NotifyListener(listener.get(), std::move(clone));
  }
}

void TabInformer::NotifyListener(mojom::TabListener* listener, State state) {
  listener->TabsChanged(std::move(state));
}

}  // namespace ai_chat
