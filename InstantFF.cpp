#include "pch.h"
#include "InstantFF.h"

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
        LOG("Your Mate wants to forfeit!");
        cvarManager->executeCommand("MateFF");
	});

    gameWrapper->HookEvent("Function TAGame.GFxHUD_TA.HandleCanVoteForfeitChanged", [this](std::string eventName) {
        LOG("Forfeiting is now possible!");
        cvarManager->executeCommand("TimedFF");
    });

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

    // toast to show that we voted to forfeit
    gameWrapper->Toast("InstantFF", "Game has been FFed!", "TAGame", 5.0f, ToastType_OK);
}

void InstantFF::MateFF()
{
    if (!InstantFFEnabled || !MateFFEnabled) {
        return;
    }

    LOG("So we are forfeiting as well...");
    Forfeit();
}

void InstantFF::TimedFF()
{
    // This function exists to later add timer functionalities, hence the double check for the cvars
    if (!InstantFFEnabled || !TimedFFEnabled) {
        return;
    }
    LOG("So we forfeit...");
    Forfeit();
}

void InstantFF::onUnload()
{
    LOG("InstantFF unloaded!");
    gameWrapper->Toast("InstantFF", "Plugin disabled!", "TAGame", 5.0f, ToastType_Info);
}