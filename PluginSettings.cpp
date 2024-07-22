#include "pch.h"
#include "InstantFF.h"

void InstantFF::RenderSettings() {
    ImGui::TextUnformatted("InstantFF Settings: Toggle Instant FF, change settings and more!");

    auto renderCheckbox = [this](const std::string& cvarName, const char* label, const char* tooltip) {
        CVarWrapper cvar = cvarManager->getCvar(cvarName);
        if (!cvar) { return; }
        bool enabled = cvar.getBoolValue();
        if (ImGui::Checkbox(label, &enabled)) {
            cvar.setValue(enabled);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(tooltip);
        }
    };

    renderCheckbox("InstantFF_enabled", "Enable plugin", "Toggle InstantFF");
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Please note that the following settings are only available when the plugin is enabled:");
    renderCheckbox("InstantFF_MateFF_enabled", "Enable MateFF", "Toggle InstantFF AutoFF on Mate's Vote");
    renderCheckbox("InstantFF_TimedFF_enabled", "Enable TimedFF", "Toggle InstantFF AutoFF on Timer");
}
