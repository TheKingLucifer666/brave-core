// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/tab_informer/browser/tab_informer_service.h"

#include <algorithm>
#include <iterator>
#include <utility>

#include "brave/components/tab_informer/common/tab_informer.mojom.h"

namespace tab_informer {

TabInformerService::TabInformerService() = default;
TabInformerService::~TabInformerService() = default;

void TabInformerService::Bind(
    mojo::PendingReceiver<mojom::TabInformer> receiver) {
  receivers_.Add(this, std::move(receiver));
}

void TabInformerService::UpdateTab(int32_t tab_id, mojom::TabPtr tab) {
  auto it = base::ranges::find_if(
      tabs_, [tab_id](const auto& tab) { return tab->id == tab_id; });

  // New tab we haven't heard about
  if (it == tabs_.end()) {
    if (tab) {
      tabs_.push_back(std::move(tab));
    }
  } else if (!tab) {
    // Removal of an existing tab
    tabs_.erase(it);
  } else {
    // Update of an existing tab.
    *it = std::move(tab);
  }

  NotifyListeners();
}

void TabInformerService::AddListener(
    mojo::PendingRemote<mojom::TabListener> listener) {
  auto id = listeners_.Add(std::move(listener));

  NotifyListener(listeners_.Get(id));
}

void TabInformerService::NotifyListeners() {
  if (listeners_.empty()) {
    return;
  }

  for (auto& listener : listeners_) {
    NotifyListener(listener.get());
  }
}

void TabInformerService::NotifyListener(mojom::TabListener* listener) {
  std::vector<mojom::TabPtr> state;
  std::ranges::transform(tabs_, std::back_inserter(state),
                         [](auto& tab) { return tab.Clone(); });
  listener->TabsChanged(std::move(state));
}

}  // namespace tab_informer
