/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/new_tab_page/html5_ntt_ui.h"

#include <string>

#include "base/strings/strcat.h"
#include "brave/components/constants/webui_url_constants.h"
#include "brave/components/html5_ntt/resources/grit/html5_ntt_generated_map.h"
#include "components/grit/brave_components_resources.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/common/url_constants.h"
#include "ui/resources/grit/webui_resources.h"

UntrustedHTML5NTTUI::UntrustedHTML5NTTUI(content::WebUI* web_ui)
    : ui::UntrustedWebUIController(web_ui) {
  auto* untrusted_source = content::WebUIDataSource::CreateAndAdd(
      web_ui->GetWebContents()->GetBrowserContext(), kUntrustedHTML5NTTURL);
  untrusted_source->AddFrameAncestor(GURL(kBraveNewTabPageURL));

  untrusted_source->SetDefaultResource(IDR_HTML5_NTT_HTML);
  untrusted_source->AddResourcePath("style.css", IDR_HTML5_NTT_CSS);
  untrusted_source->AddResourcePath("image1.png", IDR_HTML5_NTT_IMAGE1);
  untrusted_source->AddResourcePaths(base::span(kHtml5NttGenerated));

  untrusted_source->OverrideContentSecurityPolicy(
      network::mojom::CSPDirectiveName::StyleSrc,
      std::string("style-src 'self';"));
  untrusted_source->OverrideContentSecurityPolicy(
      network::mojom::CSPDirectiveName::ImgSrc,
      std::string("img-src 'self';"));

  untrusted_source->UseStringsJs();
  untrusted_source->AddString("braveHtml5NttUrl", kUntrustedHTML5NTTURL);

  // In case if we need to embed a frame.
  // untrusted_source->AddResourcePath("parent_frame.html", IDR_HTML5_NTT_PARENT_FRAME_HTML);
  // untrusted_source->AddFrameAncestor(GURL(kUntrustedHTML5NTTURL));
  // untrusted_source->OverrideContentSecurityPolicy(
  //     network::mojom::CSPDirectiveName::FrameSrc,
  //     base::StrCat({"frame-src ", kUntrustedHTML5NTTURL, ";"}));
}

UntrustedHTML5NTTUI::~UntrustedHTML5NTTUI() = default;

std::unique_ptr<content::WebUIController>
UntrustedHTML5NTTUIConfig::CreateWebUIController(content::WebUI* web_ui,
                                                 const GURL& url) {
  return std::make_unique<UntrustedHTML5NTTUI>(web_ui);
}

UntrustedHTML5NTTUIConfig::UntrustedHTML5NTTUIConfig()
    : WebUIConfig(content::kChromeUIUntrustedScheme, kUntrustedHTML5NTTHost) {}
