#include "pch.h"
#include "InstantFF.h"

void InstantFF::RenderSettings() {
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

    auto renderSlider = [this](const std::string& cvarName, const char* label, int& variable, const char* format, int min, int max) {
        CVarWrapper cvar = cvarManager->getCvar(cvarName);
        if (!cvar) { return; }
        variable = cvar.getIntValue();
        if (ImGui::SliderInt(label, &variable, min, max, format)) {
            cvar.setValue(variable);
        }
    };

    // Layout
    ImGui::TextUnformatted("InstantFF Settings: Toggle Instant FF, change settings and more!");
    renderCheckbox("InstantFF_enabled", "Enable plugin", "Toggle InstantFF");
    ImGui::Separator();

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Please note that the following settings are only available when the plugin is enabled:");
    renderCheckbox("InstantFF_MateFF_enabled", "Enable MateFF", "Toggle InstantFF AutoFF on Mate's Vote");
    renderCheckbox("InstantFF_TimedFF_enabled", "Enable TimedFF", "Toggle InstantFF AutoFF on Timer");

    ImGui::TextUnformatted("Delays:");
    renderSlider("InstantFF_MateFF_delay", "MateFF Delay", MateFFDelay, "%d seconds", 0, 10);
    renderSlider("InstantFF_TimedFF_delay", "TimedFF Delay", TimedFFDelay, "%d seconds", 0, 150);
}