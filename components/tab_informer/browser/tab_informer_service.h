// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_COMPONENTS_TAB_INFORMER_BROWSER_TAB_INFORMER_SERVICE_H_
#define BRAVE_COMPONENTS_TAB_INFORMER_BROWSER_TAB_INFORMER_SERVICE_H_

#include <vector>

#include "base/memory/raw_ptr.h"
#include "brave/components/tab_informer/common/tab_informer.mojom.h"
#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/web_contents.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote_set.h"

namespace tab_informer {

class COMPONENT_EXPORT(TAB_INFORMER_BROWSER) TabInformerService
    : public KeyedService,
      public mojom::TabInformer {
 public:
  TabInformerService();
  ~TabInformerService() override;

  // Updates the tab with the given |tab_id|. If |tab| is nullptr the tab will
  // be removed.
  void UpdateTab(int32_t tab_id, mojom::TabPtr tab);

  void Bind(mojo::PendingReceiver<mojom::TabInformer> receiver);

  // mojom::TabInformer
  void AddListener(mojo::PendingRemote<mojom::TabListener> listener) override;

 private:
  friend class TabInformerBrowserTest;

  void NotifyListeners();
  void NotifyListener(mojom::TabListener* listener);

  raw_ptr<content::WebContents> owner_contents_;

  mojo::ReceiverSet<mojom::TabInformer> receivers_;
  mojo::RemoteSet<mojom::TabListener> listeners_;

  std::vector<mojom::TabPtr> tabs_;
};

}  // namespace tab_informer

#endif  // BRAVE_COMPONENTS_TAB_INFORMER_BROWSER_TAB_INFORMER_SERVICE_H_
