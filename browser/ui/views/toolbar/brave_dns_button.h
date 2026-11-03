// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_VIEWS_TOOLBAR_BRAVE_DNS_BUTTON_H_
#define BRAVE_BROWSER_UI_VIEWS_TOOLBAR_BRAVE_DNS_BUTTON_H_

#include <memory>

#include "base/memory/raw_ref.h"
#include "chrome/browser/ui/views/toolbar/toolbar_button.h"
#include "components/prefs/pref_change_registrar.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/menus/simple_menu_model.h"

class BrowserWindowInterface;

namespace views {
class MenuRunner;
}  // namespace views

class BraveDnsButton : public ToolbarButton,
                       public ui::SimpleMenuModel::Delegate {
  METADATA_HEADER(BraveDnsButton, ToolbarButton)
 public:
  explicit BraveDnsButton(BrowserWindowInterface* browser);
  BraveDnsButton(const BraveDnsButton&) = delete;
  BraveDnsButton& operator=(const BraveDnsButton&) = delete;
  ~BraveDnsButton() override;

  // ToolbarButton:
  void UpdateIcon() override;

 private:
  enum DnsCommand {
    kOsDefault = 1,
    kCloudflare,
    kGoogle,
    kCleanBrowsing,
    kOpenDns,
    kNextDns,
    kOpenDnsSettings,
  };

  void ButtonPressed();
  void UpdateState();
  void OnDnsPrefChanged();
  std::unique_ptr<ui::SimpleMenuModel> CreateMenuModel();

  // ui::SimpleMenuModel::Delegate:
  void ExecuteCommand(int command_id, int event_flags) override;
  bool IsCommandIdChecked(int command_id) const override;

  const raw_ref<BrowserWindowInterface> browser_;
  PrefChangeRegistrar pref_change_registrar_;
  std::unique_ptr<ui::SimpleMenuModel> menu_model_;
  std::unique_ptr<views::MenuRunner> menu_runner_;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_TOOLBAR_BRAVE_DNS_BUTTON_H_
