#include "pch.h"
#include "InstantFF.h"
#include <chrono>
#include <thread>

BAKKESMOD_PLUGIN(InstantFF, "InstantFF - Loaded", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void InstantFF::onLoad()
{
    _globalCvarManager = cvarManager;
    LOG("InstantFF loaded!");
    gameWrapper->Toast("InstantFF", "Plugin enabled!", "TAGame", 5.0f, ToastType_OK);

    // Register cvars
    auto registerCvar = [this](const std::string& name, const std::string& defaultValue, const std::string& description, bool& variable) {
        cvarManager->registerCvar(name, defaultValue, description, true, true, 0, true, 1)
            .addOnValueChanged([this, &variable](std::string oldValue, CVarWrapper cvar) {
            variable = cvar.getBoolValue();
                });
        };

    registerCvar("InstantFF_enabled", "0", "Enable InstantFF Plugin", InstantFFEnabled);
    registerCvar("InstantFF_MateFF_enabled", "0", "Enable InstantFF AutoFF on Mate's Vote", MateFFEnabled);
    registerCvar("InstantFF_TimedFF_enabled", "0", "Enable InstantFF AutoFF on Timer", TimedFFEnabled);

    auto registerIntCvar = [this](const std::string& name, const std::string& defaultValue, const std::string& description, int& variable, int minValue, int maxValue) {
        cvarManager->registerCvar(name, defaultValue, description, true, true, minValue, true, maxValue)
            .addOnValueChanged([this, &variable](std::string oldValue, CVarWrapper cvar) {
            variable = cvar.getIntValue();
                });
        };

    registerIntCvar("InstantFF_MateFF_delay", "0", "Delay for MateFF in seconds", MateFFDelay, 0, 10);
    registerIntCvar("InstantFF_TimedFF_delay", "0", "Delay for TimedFF in seconds", TimedFFDelay, 0, 150);


    // Register commands for testing
    auto registerNotifier = [this](const std::string& name, void (InstantFF::* func)()) {
        cvarManager->registerNotifier(name, [this, func](std::vector<std::string> args) {
            (this->*func)();
            }, "", PERMISSION_ALL);
        };

    registerNotifier("Forfeit", &InstantFF::Forfeit);
    registerNotifier("MateFF", &InstantFF::MateFF);
    registerNotifier("TimedFF", &InstantFF::TimedFF);

    // Hook events
    gameWrapper->HookEvent("Function TAGame.VoteActor_TA.EventStarted", [this](std::string eventName) {
        cvarManager->executeCommand("MateFF");
        });

    gameWrapper->HookEvent("Function TAGame.GFxHUD_TA.HandleCanVoteForfeitChanged", [this](std::string eventName) {
        ServerWrapper sw = gameWrapper->IsInFreeplay() ? gameWrapper->GetGameEventAsServer() : gameWrapper->GetOnlineGame();
        if (sw.IsNull() || sw.GetSecondsRemaining() <= 1 || sw.GetSecondsRemaining() >= 299) {
            return;
        }

        cvarManager->executeCommand("TimedFF");
        });

    gameWrapper->HookEventWithCaller<ServerWrapper>("Function TAGame.GameEvent_Soccar_TA.OnGameTimeUpdated",
        [this](ServerWrapper caller, void* params, std::string eventname)
        {
            float currentGameTimeRemaining = caller.GetSecondsRemaining();

            if (currentGameTimeRemaining <= 1 || currentGameTimeRemaining >= 299) {
				return;
            }

            if (isMateFFActive && (startTimeRemaining - currentGameTimeRemaining >= MateFFDelay)) {
                LOG("MateFF delay completed. Proceeding to forfeit...");
                isMateFFActive = false;
                Forfeit();
            }

            if (isTimedFFActive && (startTimeRemaining - currentGameTimeRemaining >= TimedFFDelay)) {
                LOG("TimedFF delay completed. Proceeding to forfeit...");
                isTimedFFActive = false;
                Forfeit();
            }
        }
    );

    //Function TAGame.GFxHUD_TA.HandleCanVoteForfeitChanged
    //Function TAGame.GFxShell_TA.VoteToForfeit
    //Function TAGame.PRI_TA.ServerVoteToForfeit
    //Function TAGame.PRI_TA.OnStartVoteToForfeitDisabledChanged
    //Function TAGame.GFxData_LocalPlayer_TA.HandleVoteToForfeitDisabledChanged
    //Function TAGame.PRI_TA.EventStartVoteToForfeitDisabledChanged
    //Function TAGame.GameEvent_TA.EventCanVoteForfeitChanged"

    //// yet unknown events
    //Function TAGame.GameEvent_Soccar_TA.OnForfeitVoteStarted
    //Function TAGame.GameEvent_Soccar_TA.OnCanVoteForfeitChanged
    //Function TAGame.PRI_TA.EventVoteToForfeit
}

void InstantFF::Forfeit()
{
    LOG("Forfeiting...");

    // Check if the game is valid
    if (!gameWrapper->IsInOnlineGame() && !gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) {
        gameWrapper->Toast("InstantFF", "Not in an online game or freeplay!", "TAGame", 5.0f, ToastType_Error);
        return;
    }

    ServerWrapper sw = gameWrapper->IsInFreeplay() ? gameWrapper->GetGameEventAsServer() : gameWrapper->GetOnlineGame();
    if (sw.IsNull()) {
        gameWrapper->Toast("InstantFF", "GameWrapper is null!", "TAGame", 5.0f, ToastType_Error);
        return;
    }

    if (sw.GetbMatchEnded()) {
        gameWrapper->Toast("InstantFF", "Game has ended!", "TAGame", 5.0f, ToastType_Info);
        return;
    }

    for (PlayerControllerWrapper p : sw.GetLocalPlayers()) {
        PriWrapper pri = p.GetPRI();
        if (!pri)
            continue;
        pri.ServerVoteToForfeit();
    }

    gameWrapper->Toast("InstantFF", "Game has been successfully FFed!", "TAGame", 7.0f, ToastType_OK);
}

void InstantFF::MateFF()
{
    if (!InstantFFEnabled || !MateFFEnabled) {
        return;
    }

    ServerWrapper sw = gameWrapper->GetOnlineGame();
    if (sw.IsNull()) {
        LOG("GameWrapper is null");
        return;
    }

    startTimeRemaining = sw.GetSecondsRemaining();
    isMateFFActive = true;

    gameWrapper->Toast("InstantFF", "Your mate wants to forfeit! You will forfeit in " + std::to_string(MateFFDelay) + " seconds!", "TAGame", 5.0f, ToastType_Info);
}

void InstantFF::TimedFF()
{
    if (!InstantFFEnabled || !TimedFFEnabled) {
        return;
    }

    ServerWrapper sw = gameWrapper->GetOnlineGame();
    if (sw.IsNull()) {
        LOG("GameWrapper is null");
        return;
    }

    startTimeRemaining = sw.GetSecondsRemaining();
    isTimedFFActive = true;

    gameWrapper->Toast("InstantFF", "You will forfeit in " + std::to_string(TimedFFDelay) + " seconds!", "TAGame", 5.0f, ToastType_Info);
}


void InstantFF::onUnload()
{
    gameWrapper->Toast("InstantFF", "Plugin disabled!", "TAGame", 5.0f, ToastType_Info);
    LOG("InstantFF unloaded!");
}
