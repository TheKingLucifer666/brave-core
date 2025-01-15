// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/ai_chat/tab_informer.h"

#include <memory>
#include <vector>

#include "base/functional/callback_forward.h"
#include "base/run_loop.h"
#include "base/test/bind.h"
#include "brave/components/ai_chat/core/common/mojom/tab_informer.mojom-forward.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_tabstrip.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/test/browser_test.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace ai_chat {

namespace {

using Predicate =
    base::RepeatingCallback<bool(const std::vector<mojom::WindowPtr>&)>;

class UpdateTracker : public mojom::TabListener {
 public:
  UpdateTracker() = default;
  ~UpdateTracker() override = default;

  const std::vector<mojom::WindowPtr>& GetLastWindows() const {
    CHECK(last_windows_.has_value());
    return last_windows_.value();
  }

  void WaitFor(Predicate predicate) {
    if (last_windows_ && predicate.Run(last_windows_.value())) {
      return;
    }

    base::RunLoop waiter;
    on_change_ = base::BindLambdaForTesting([&]() {
      if (!last_windows_) {
        return;
      }
      if (predicate.Run(last_windows_.value())) {
        waiter.Quit();
        on_change_.Reset();
      }
    });

    waiter.Run();
  }

 private:
  void TabsChanged(std::vector<mojom::WindowPtr> windows) override {
    last_windows_ = std::move(windows);

    if (!on_change_.is_null()) {
      on_change_.Run();
    }
  }

  base::RepeatingClosure on_change_;
  std::optional<std::vector<mojom::WindowPtr>> last_windows_;
  std::unique_ptr<base::RunLoop> run_loop_;
};

}  // namespace

class TabInformerBrowserTest : public InProcessBrowserTest {
 public:
  TabInformerBrowserTest() = default;
  ~TabInformerBrowserTest() override = default;

  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();

    mojo::PendingReceiver<mojom::TabInformer> receiver;
    tab_informer_ = std::make_unique<TabInformer>(
        std::move(receiver),
        browser()->tab_strip_model()->GetActiveWebContents());

    auto listener = std::make_unique<UpdateTracker>();
    tracker_ = listener.get();

    mojo::PendingRemote<mojom::TabListener> pending_remote;
    mojo::MakeSelfOwnedReceiver(
        std::move(listener), pending_remote.InitWithNewPipeAndPassReceiver());
    tab_informer_->AddListener(std::move(pending_remote));
  }

  void TearDownOnMainThread() override {
    tracker_ = nullptr;
    tab_informer_.reset();

    InProcessBrowserTest::SetUpOnMainThread();
  }

  const std::vector<mojom::WindowPtr>& GetLastWindows() const {
    return tracker_->GetLastWindows();
  }

  void WaitFor(Predicate predicate) { tracker_->WaitFor(std::move(predicate)); }

 protected:
  void AppendTab(std::string url) {
    chrome::AddTabAt(browser(), GURL(url), -1, false);
  }

  std::unique_ptr<TabInformer> tab_informer_;
  raw_ptr<UpdateTracker> tracker_;
};

IN_PROC_BROWSER_TEST_F(TabInformerBrowserTest, GetsInitialTabs) {
  WaitFor(base::BindLambdaForTesting(
      [](const std::vector<mojom::WindowPtr>& windows) {
        return windows.size() == 1;
      }));

  const auto& windows = GetLastWindows();

  // Should be have no tabs because we exclude the owner tab
  EXPECT_EQ(windows[0]->tabs.size(), 0u);
}

IN_PROC_BROWSER_TEST_F(TabInformerBrowserTest, TabsChange) {
  AppendTab("https://google.com");
  AppendTab("https://brave.com");
  WaitFor(base::BindLambdaForTesting(
      [](const std::vector<mojom::WindowPtr>& windows) {
        return windows.size() == 1 && windows.at(0)->tabs.size() == 2;
      }));

  {
    const auto& windows = GetLastWindows();
    EXPECT_EQ(windows[0]->tabs[0]->url, GURL("https://google.com"));
    EXPECT_EQ(windows[0]->tabs[1]->url, GURL("https://brave.com"));
  }

  // Close all tabs but the one we're bound to
  chrome::CloseOtherTabs(browser());

  WaitFor(base::BindLambdaForTesting(
      [](const std::vector<mojom::WindowPtr>& windows) {
        return windows.size() == 1 && windows.at(0)->tabs.size() == 0;
      }));

  {
    const auto& updated_windows = GetLastWindows();
    EXPECT_EQ(updated_windows[0]->tabs.size(), 0u);
  }
}

IN_PROC_BROWSER_TEST_F(TabInformerBrowserTest, MultipleWindows) {
  AppendTab("https://topos.nz");
  WaitFor(base::BindLambdaForTesting(
      [](const std::vector<mojom::WindowPtr>& windows) {
        return windows.size() == 1 && windows.at(0)->tabs.size() == 1;
      }));

  {
    const auto& windows = GetLastWindows();
    EXPECT_EQ(windows[0]->tabs[0]->url, GURL("https://topos.nz"));
  }

  chrome::NewWindow(browser());
  WaitFor(base::BindLambdaForTesting(
      [](const std::vector<mojom::WindowPtr>& windows) {
        return windows.size() == 2 && windows.at(0)->tabs.size() == 1 &&
               windows.at(1)->tabs.size() == 0;
      }));

  AppendTab("https://brave.com");
  AppendTab("https://readr.nz");
  WaitFor(base::BindLambdaForTesting(
      [](const std::vector<mojom::WindowPtr>& windows) {
        return windows.size() == 2 && windows.at(0)->tabs.size() == 3 &&
               windows.at(1)->tabs.size() == 0;
      }));

  chrome::MoveTabsToNewWindow(browser(), {2, 3});
  WaitFor(base::BindLambdaForTesting(
      [](const std::vector<mojom::WindowPtr>& windows) {
        LOG(ERROR) << "Window 0: " << windows.at(0)->tabs.size()
                   << ", Window 1: " << windows.at(1)->tabs.size();
        return windows.size() == 3 && windows.at(0)->tabs.size() == 1 &&
               windows.at(1)->tabs.size() == 0 &&
               windows.at(2)->tabs.size() == 2;
      }));

  {
    const auto& windows = GetLastWindows();

    // Just topos.nz - first tab is bound to the TabInformer
    EXPECT_EQ(windows[0]->tabs[0]->url, GURL("https://topos.nz"));

    // readr.nz, brave.com
    EXPECT_EQ(windows[2]->tabs[1]->url, GURL("https://readr.nz"));
    EXPECT_EQ(windows[2]->tabs[0]->url, GURL("https://brave.com"));
  }
}

}  // namespace ai_chat
