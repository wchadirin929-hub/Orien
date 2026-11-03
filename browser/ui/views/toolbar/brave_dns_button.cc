// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/views/toolbar/brave_dns_button.h"

#include <string>
#include <utility>

#include "base/functional/bind.h"
#include "base/types/to_address.h"
#include "brave/components/vector_icons/vector_icons.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/singleton_tabs.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/mojom/menu_source_type.mojom.h"
#include "ui/menus/simple_menu_model.h"
#include "ui/views/controls/button/button_controller.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "url/gurl.h"

namespace {

constexpr char kDohTemplateCloudflare[] =
    "https://chrome.cloudflare-dns.com/dns-query";
constexpr char kDohTemplateGoogle[] =
    "https://dns.google/dns-query{?dns}";
constexpr char kDohTemplateCleanBrowsing[] =
    "https://doh.cleanbrowsing.org/doh/family-filter{?dns}";
constexpr char kDohTemplateOpenDns[] =
    "https://doh.opendns.com/dns-query{?dns}";
constexpr char kDohTemplateNextDns[] =
    "https://dns.nextdns.io";

constexpr char kModeAutomatic[] = "automatic";
constexpr char kModeOff[] = "off";
constexpr char kModeSecure[] = "secure";

constexpr int kDnsRadioGroup = 1;

}  // namespace

BraveDnsButton::BraveDnsButton(BrowserWindowInterface* browser)
    : ToolbarButton(base::BindRepeating(&BraveDnsButton::ButtonPressed,
                                        base::Unretained(this))),
      browser_(*browser) {
  SetVectorIcon(kLeoNetworkSpeedFastIcon);
  button_controller()->set_notify_action(
      views::ButtonController::NotifyAction::kOnPress);

  PrefService* local_state =
      g_browser_process ? g_browser_process->local_state() : nullptr;
  if (local_state) {
    pref_change_registrar_.Init(local_state);
    pref_change_registrar_.Add(
        prefs::kDnsOverHttpsMode,
        base::BindRepeating(&BraveDnsButton::OnDnsPrefChanged,
                            base::Unretained(this)));
    pref_change_registrar_.Add(
        prefs::kDnsOverHttpsTemplates,
        base::BindRepeating(&BraveDnsButton::OnDnsPrefChanged,
                            base::Unretained(this)));
  }

  UpdateState();
}

BraveDnsButton::~BraveDnsButton() = default;

void BraveDnsButton::UpdateIcon() {
  SetVectorIcon(kLeoNetworkSpeedFastIcon);
}

void BraveDnsButton::OnDnsPrefChanged() {
  UpdateState();
}

void BraveDnsButton::UpdateState() {
  PrefService* local_state =
      g_browser_process ? g_browser_process->local_state() : nullptr;
  if (!local_state) {
    SetTooltipText(u"Secure DNS");
    SetHighlighted(false);
    return;
  }

  const std::string mode = local_state->GetString(prefs::kDnsOverHttpsMode);
  const std::string templates =
      local_state->GetString(prefs::kDnsOverHttpsTemplates);

  if (mode == kModeSecure) {
    SetHighlighted(true);
    if (templates == kDohTemplateCloudflare) {
      SetTooltipText(u"Secure DNS: Cloudflare (1.1.1.1)");
    } else if (templates == kDohTemplateGoogle) {
      SetTooltipText(u"Secure DNS: Google (Public DNS)");
    } else if (templates == kDohTemplateNextDns) {
      SetTooltipText(u"Secure DNS: NextDNS");
    } else if (templates == kDohTemplateCleanBrowsing) {
      SetTooltipText(u"Secure DNS: CleanBrowsing");
    } else if (templates == kDohTemplateOpenDns) {
      SetTooltipText(u"Secure DNS: OpenDNS");
    } else {
      SetTooltipText(u"Secure DNS: Custom");
    }
  } else {
    SetHighlighted(false);
    SetTooltipText(u"Secure DNS: OS default");
  }
}

void BraveDnsButton::ButtonPressed() {
  menu_model_ = CreateMenuModel();
  menu_runner_ = std::make_unique<views::MenuRunner>(
      menu_model_.get(), views::MenuRunner::HAS_MNEMONICS);
  menu_runner_->RunMenuAt(GetWidget(), /*button_controller=*/nullptr,
                          GetBoundsInScreen(),
                          views::MenuAnchorPosition::kTopRight,
                          ui::mojom::MenuSourceType::kMouse);
}

std::unique_ptr<ui::SimpleMenuModel> BraveDnsButton::CreateMenuModel() {
  auto model = std::make_unique<ui::SimpleMenuModel>(this);

  model->AddRadioItem(kOsDefault, u"OS default (when available)",
                      kDnsRadioGroup);
  model->AddRadioItem(kCloudflare, u"Cloudflare (1.1.1.1)",
                      kDnsRadioGroup);
  model->AddRadioItem(kGoogle, u"Google (Public DNS)",
                      kDnsRadioGroup);
  model->AddRadioItem(kCleanBrowsing, u"CleanBrowsing (Family Filter)",
                      kDnsRadioGroup);
  model->AddRadioItem(kOpenDns, u"OpenDNS",
                      kDnsRadioGroup);
  model->AddRadioItem(kNextDns, u"NextDNS",
                      kDnsRadioGroup);

  model->AddSeparator(ui::NORMAL_SEPARATOR);
  model->AddItem(kOpenDnsSettings, u"DNS Settings…");

  return model;
}

void BraveDnsButton::ExecuteCommand(int command_id, int event_flags) {
  if (command_id == kOpenDnsSettings) {
    ShowSingletonTab(base::to_address(browser_),
                     GURL("chrome://settings/security"));
    return;
  }

  PrefService* local_state =
      g_browser_process ? g_browser_process->local_state() : nullptr;
  if (!local_state) {
    return;
  }

  switch (command_id) {
    case kOsDefault:
      local_state->SetString(prefs::kDnsOverHttpsMode, kModeAutomatic);
      local_state->SetString(prefs::kDnsOverHttpsTemplates, "");
      break;
    case kCloudflare:
      local_state->SetString(prefs::kDnsOverHttpsMode, kModeSecure);
      local_state->SetString(prefs::kDnsOverHttpsTemplates,
                             kDohTemplateCloudflare);
      break;
    case kGoogle:
      local_state->SetString(prefs::kDnsOverHttpsMode, kModeSecure);
      local_state->SetString(prefs::kDnsOverHttpsTemplates,
                             kDohTemplateGoogle);
      break;
    case kCleanBrowsing:
      local_state->SetString(prefs::kDnsOverHttpsMode, kModeSecure);
      local_state->SetString(prefs::kDnsOverHttpsTemplates,
                             kDohTemplateCleanBrowsing);
      break;
    case kOpenDns:
      local_state->SetString(prefs::kDnsOverHttpsMode, kModeSecure);
      local_state->SetString(prefs::kDnsOverHttpsTemplates,
                             kDohTemplateOpenDns);
      break;
    case kNextDns:
      local_state->SetString(prefs::kDnsOverHttpsMode, kModeSecure);
      local_state->SetString(prefs::kDnsOverHttpsTemplates,
                             kDohTemplateNextDns);
      break;
    default:
      return;
  }
}

bool BraveDnsButton::IsCommandIdChecked(int command_id) const {
  PrefService* local_state =
      g_browser_process ? g_browser_process->local_state() : nullptr;
  if (!local_state) {
    return false;
  }

  const std::string mode = local_state->GetString(prefs::kDnsOverHttpsMode);
  const std::string templates =
      local_state->GetString(prefs::kDnsOverHttpsTemplates);

  switch (command_id) {
    case kOsDefault:
      return mode == kModeAutomatic || mode == kModeOff || templates.empty();
    case kCloudflare:
      return mode == kModeSecure && templates == kDohTemplateCloudflare;
    case kGoogle:
      return mode == kModeSecure && templates == kDohTemplateGoogle;
    case kCleanBrowsing:
      return mode == kModeSecure && templates == kDohTemplateCleanBrowsing;
    case kOpenDns:
      return mode == kModeSecure && templates == kDohTemplateOpenDns;
    case kNextDns:
      return mode == kModeSecure && templates == kDohTemplateNextDns;
    default:
      return false;
  }
}

BEGIN_METADATA(BraveDnsButton)
END_METADATA
