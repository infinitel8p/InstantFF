#include "pch.h"
#include "InstantFF.h"

BAKKESMOD_PLUGIN(InstantFF, "Instant ForFeit", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void InstantFF::onLoad()
{
    _globalCvarManager = cvarManager;
    LOG("InstantFF loaded!");

    cvarManager->registerNotifier("Forfeit", [this](std::vector<std::string> args) {
        Forfeit();
        }, "", PERMISSION_ALL);

    gameWrapper->HookEvent("Function TAGame.VoteActor_TA.EventStarted", [this](std::string eventName) {
        LOG("Your Mate wants to Forfeit");
        cvarManager->executeCommand("Forfeit");
	});

    //Function TAGame.GFxHUD_TA.HandleCanVoteForfeitChanged
    //Function TAGame.GFxShell_TA.VoteToForfeit
    //Function TAGame.PRI_TA.ServerVoteToForfeit
    //Function TAGame.PRI_TA.OnStartVoteToForfeitDisabledChanged
    //Function TAGame.GFxData_LocalPlayer_TA.HandleVoteToForfeitDisabledChanged
    //Function TAGame.PRI_TA.EventStartVoteToForfeitDisabledChanged
    //Function TAGame.GameEvent_TA.EventCanVoteForfeitChanged"
    // 
    //// yet unknown events
    //Function TAGame.GameEvent_Soccar_TA.OnForfeitVoteStarted
    //Function TAGame.GameEvent_Soccar_TA.OnCanVoteForfeitChanged
    //Function TAGame.PRI_TA.EventVoteToForfeit
}

void InstantFF::Forfeit()
{
    LOG("Forfeit called");

    // Check if the game is valid
    if (!gameWrapper->IsInOnlineGame() && !gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay()) {
        LOG("Not in an online game or freeplay!");
        return;
    }

    ServerWrapper sw = NULL;
    if (gameWrapper->IsInFreeplay()) {
        LOG("GameMode: Freeplay");
        sw = gameWrapper->GetGameEventAsServer();
    }
    else {
        LOG("GameMode: Online");
        sw = gameWrapper->GetOnlineGame();
    }

    if (sw.IsNull()) {
        LOG("GameWrapper is null");
        return;
    }

    if (sw.GetbMatchEnded()) {
        LOG("Game has ended");
        return;
    }

    for (PlayerControllerWrapper p : sw.GetLocalPlayers())
    {
        PriWrapper pri = p.GetPRI();
        if (!pri)
            continue;

        LOG("So we Forfeit aswell!");
        pri.ServerVoteToForfeit();
    }
}