// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/api/save_page_handler.h"

#include <string>

#include "atom/browser/atom_browser_context.h"
#include "atom/browser/atom_download_manager_delegate.h"
#include "atom/common/native_mate_converters/download_item_converter.h"
#include "base/callback.h"
#include "base/files/file_path.h"
#include "content/public/browser/web_contents.h"

namespace atom {

namespace api {

SavePageHandler::SavePageHandler(content::WebContents* web_contents,
                                 const SavePageCallback& callback)
    : id_(0),
      web_contents_(web_contents),
      callback_(callback) {
}

SavePageHandler::~SavePageHandler() {
  LOG(ERROR) << "~SavePageHandler";
}

void SavePageHandler::OnDownloadCreated(content::DownloadManager* manager,
                                        content::DownloadItem* item) {
  if (id_ == item->GetId()) {
    GetDownloadManager()->RemoveObserver(this);
    item->AddObserver(this);
  }
}

bool SavePageHandler::Handle(const base::FilePath& full_path,
                             const content::SavePageType& save_type) {
  auto delegate = static_cast<AtomDownloadManagerDelegate*>(
      GetDownloadManager()->GetDelegate());
  GetDownloadManager()->AddObserver(this);

  id_ = delegate->NextId();
  bool result = web_contents_->SavePage(full_path,
                                        full_path.DirName(),
                                        save_type);
  DCHECK_EQ(id_+1, delegate->NextId());
  // If initialization fails which means fail to create |DownloadItem|, we need
  // to delete the |SavePageHandler| instance to avoid memory-leak.
  if (!result)
    delete this;
  return result;
}

void SavePageHandler::OnDownloadUpdated(content::DownloadItem* item) {
  if (item->IsDone()) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Locker locker(isolate);
    v8::HandleScope handle_scope(isolate);
    callback_.Run(
        mate::Converter<content::DownloadItem::DownloadState>::ToV8(
            isolate, item->GetState()));
    Destroy();
  }
}

void SavePageHandler::OnDownloadDestroyed(content::DownloadItem* item) {
}

void SavePageHandler::Destroy() {
  auto download_item = GetDownloadManager()->GetDownload(id_);
  if (download_item) {
    // Self destroyed when finishing a request.
    download_item->RemoveObserver(this);
    delete this;
  }
}

content::DownloadManager* SavePageHandler::GetDownloadManager() {
  return content::BrowserContext::GetDownloadManager(
      web_contents_->GetBrowserContext());
}

// static
bool SavePageHandler::IsSavePageTypes(const std::string& type) {
  return type == "multipart/related" || type == "text/html";
}

}  // namespace api

}  // namespace atom
