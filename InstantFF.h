#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class InstantFF: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase
{
	//std::shared_ptr<bool> enabled;

	void onLoad() override;
	void onUnload() override;

	void Forfeit();
	void MateFF();
	void TimedFF();

	bool InstantFFEnabled = false;
	bool MateFFEnabled = false;
	bool TimedFFEnabled = false;

public:
	void RenderSettings() override;
};
