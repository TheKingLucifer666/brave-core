/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_NEW_TAB_PAGE_HTML5_NTT_UI_H_
#define BRAVE_BROWSER_UI_WEBUI_NEW_TAB_PAGE_HTML5_NTT_UI_H_

#include <memory>

#include "content/public/browser/web_ui.h"
#include "content/public/browser/webui_config.h"
#include "ui/webui/untrusted_web_ui_controller.h"

class UntrustedHTML5NTTUI : public ui::UntrustedWebUIController {
 public:
  explicit UntrustedHTML5NTTUI(content::WebUI* web_ui);
  UntrustedHTML5NTTUI(const UntrustedHTML5NTTUI&) = delete;
  UntrustedHTML5NTTUI& operator=(const UntrustedHTML5NTTUI&) = delete;
  ~UntrustedHTML5NTTUI() override;
};

class UntrustedHTML5NTTUIConfig : public content::WebUIConfig {
 public:
  UntrustedHTML5NTTUIConfig();
  ~UntrustedHTML5NTTUIConfig() override = default;

  std::unique_ptr<content::WebUIController> CreateWebUIController(
      content::WebUI* web_ui,
      const GURL& url) override;
};

#endif  // BRAVE_BROWSER_UI_WEBUI_NEW_TAB_PAGE_HTML5_NTT_UI_H_
