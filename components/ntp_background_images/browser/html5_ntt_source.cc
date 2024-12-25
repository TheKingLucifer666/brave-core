/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/ntp_background_images/browser/html5_ntt_source.h"

#include <optional>
#include <utility>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/memory/ref_counted_memory.h"
#include "base/task/thread_pool.h"
#include "brave/components/constants/webui_url_constants.h"
#include "brave/components/ntp_background_images/browser/ntp_background_images_service.h"
#include "brave/components/ntp_background_images/browser/ntp_sponsored_images_data.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"

namespace ntp_background_images {

namespace {

std::optional<std::string> ReadFileToString(const base::FilePath& path) {
  std::string contents;
  if (!base::ReadFileToString(path, &contents)) {
    return std::optional<std::string>();
  }
  return contents;
}

}  // namespace

HTML5NTTSource::HTML5NTTSource(NTPBackgroundImagesService* service)
    : service_(service), weak_factory_(this) {}

HTML5NTTSource::~HTML5NTTSource() = default;

std::string HTML5NTTSource::GetSource() {
  return kUntrustedHTML5NTTDataURL;
}

void HTML5NTTSource::StartDataRequest(
    const GURL& url,
    const content::WebContents::Getter& wc_getter,
    GotDataCallback callback) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  VLOG(6) << "Start data request for HTML5NTT asset at " << url;

  const std::string path = URLDataSource::URLToRequestPath(url);
  if (!IsValidPath(path)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE, base::BindOnce(std::move(callback),
                                  scoped_refptr<base::RefCountedMemory>()));

    return;
  }

  base::FilePath file_path = GetLocalFilePathFor(path);
  CHECK(!file_path.empty());
  GetFile(file_path, std::move(callback));
}

void HTML5NTTSource::GetFile(const base::FilePath& file_path,
                             GotDataCallback callback) {
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock()},
      base::BindOnce(&ReadFileToString, file_path),
      base::BindOnce(&HTML5NTTSource::OnGotFile, weak_factory_.GetWeakPtr(),
                     std::move(callback)));
}

void HTML5NTTSource::OnGotFile(GotDataCallback callback,
                               std::optional<std::string> input) {
  if (!input) {
    return;
  }

  std::move(callback).Run(
      new base::RefCountedBytes(base::as_byte_span(*input)));
}

std::string HTML5NTTSource::GetMimeType(const GURL& url) {
  const std::string path = URLDataSource::URLToRequestPath(url);
  const auto file_path = base::FilePath::FromUTF8Unsafe(path);
  if (file_path.MatchesExtension(FILE_PATH_LITERAL(".html"))) {
    return "text/html";
  } else if (file_path.MatchesExtension(FILE_PATH_LITERAL(".css"))) {
    return "text/css";
  } else if (file_path.MatchesExtension(FILE_PATH_LITERAL(".js"))) {
    return "application/javascript";
  } else if (file_path.MatchesExtension(FILE_PATH_LITERAL(".jpg")) ||
             file_path.MatchesExtension(FILE_PATH_LITERAL(".jpeg"))) {
    return "image/jpeg";
  } else if (file_path.MatchesExtension(FILE_PATH_LITERAL(".png"))) {
    return "image/png";
  } else if (file_path.MatchesExtension(FILE_PATH_LITERAL(".webp"))) {
    return "image/webp";
  } else if (file_path.MatchesExtension(FILE_PATH_LITERAL(".avif"))) {
    return "image/avif";
  } else if (file_path.MatchesExtension(FILE_PATH_LITERAL(".mp4"))) {
    return "video/mp4";
  } else {
    return "";
  }
}

bool HTML5NTTSource::AllowCaching() {
  return false;
}

base::FilePath HTML5NTTSource::GetLocalFilePathFor(const std::string& path) {
  VLOG(6) << "Get local path for HTML5NTT asset at " << path;

  auto* images_data = service_->GetBrandedImagesData(false);
  CHECK(images_data);

  const auto basename_from_path =
      base::FilePath::FromUTF8Unsafe(path).BaseName();

  for (const auto& campaign : images_data->campaigns) {
    for (const auto& background : campaign.backgrounds) {
      for (const auto& asset : background.html.assets) {
        const auto basename_from_data = asset.BaseName();

        if (basename_from_data == basename_from_path) {
          return asset;
        }
      }
    }
  }

  // Should give valid path always here because invalid |path| was
  // already filtered by `IsValidPath()`.
  NOTREACHED();
}

bool HTML5NTTSource::IsValidPath(const std::string& path) const {
  VLOG(6) << "Checking if following HTML5NTT asset is valid " << path;

  NTPSponsoredImagesData* images_data = service_->GetBrandedImagesData(false);
  if (!images_data) {
    return false;
  }

  const auto basename_from_path =
      base::FilePath::FromUTF8Unsafe(path).BaseName();

  for (const auto& campaign : images_data->campaigns) {
    for (const auto& background : campaign.backgrounds) {
      for (const auto& asset : background.html.assets) {
        const auto basename_from_data = asset.BaseName();

        if (basename_from_data == basename_from_path) {
          return true;
        }
      }
    }
  }

  return false;
}

}  // namespace ntp_background_images
