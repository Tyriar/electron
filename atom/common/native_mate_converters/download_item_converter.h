// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_COMMON_NATIVE_MATE_CONVERTERS_DOWNLOAD_ITEM_CONVERTER_H_
#define ATOM_COMMON_NATIVE_MATE_CONVERTERS_DOWNLOAD_ITEM_CONVERTER_H_

#include <string>

#include "content/public/browser/download_item.h"
#include "native_mate/converter.h"

namespace mate {

template<>
struct Converter<content::DownloadItem::DownloadState> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                   content::DownloadItem::DownloadState state) {
    std::string download_state;
    switch (state) {
      case content::DownloadItem::COMPLETE:
        download_state = "completed";
        break;
      case content::DownloadItem::CANCELLED:
        download_state = "cancelled";
        break;
      case content::DownloadItem::INTERRUPTED:
        download_state = "interrupted";
        break;
      default:
        break;
    }
    return ConvertToV8(isolate, download_state);
  }
};

}  // namespace mate

#endif  // ATOM_COMMON_NATIVE_MATE_CONVERTERS_DOWNLOAD_ITEM_CONVERTER_H_
