#include "pch.h"
#include "InstantFF.h"

BAKKESMOD_PLUGIN(InstantFF, "InstantFF", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void InstantFF::onLoad()
{
    _globalCvarManager = cvarManager;
    LOG("InstantFF loaded!");

    cvarManager->registerNotifier("InitializeCurrentPlayers", [this](std::vector<std::string> args) {
        InitializeCurrentPlayers();
        }, "", PERMISSION_ALL);

    //gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnAllTeamsCreated", [this](std::string eventName) {
    //    LOG("Initialize Game Session");
    //    cvarManager->executeCommand("InitializeCurrentPlayers");
    //    });

    //gameWrapper->HookEventWithCaller<ServerWrapper>("Function TAGame.GameEvent_Soccar_TA.OnGameTimeUpdated",
    //    [this](ServerWrapper caller, void* params, std::string eventname)
    //    {
    //        cvarManager->executeCommand("InitializeCurrentPlayers");
    //    }
    //);

    gameWrapper->HookEvent("Function TAGame.VoteActor_TA.EventStarted", [this](std::string eventName) {
	LOG("EventStarted ############################################################################################################################################################################################################################");
	});

    gameWrapper->HookEvent("Function TAGame.GFxHUD_TA.HandleCanVoteForfeitChanged", [this](std::string eventName) {
        LOG("0 - Forfeiting is now possible");
    });

    gameWrapper->HookEvent("Function TAGame.GFxShell_TA.VoteToForfeit", [this](std::string eventName) {
        LOG("1 - VoteToForfeit #######################################################");
        });

    gameWrapper->HookEvent("Function TAGame.PRI_TA.ServerVoteToForfeit", [this](std::string eventName) {
        LOG("2 - ServerVoteToForfeit #######################################################");
        });

    gameWrapper->HookEvent("Function TAGame.PRI_TA.OnStartVoteToForfeitDisabledChanged", [this](std::string eventName) {
        LOG("3 - OnStartVoteToForfeitDisabledChanged #######################################################");
        });

    gameWrapper->HookEvent("Function TAGame.GFxData_LocalPlayer_TA.HandleVoteToForfeitDisabledChanged", [this](std::string eventName) {
        LOG("4 - HandleVoteToForfeitDisabledChanged #######################################################");
        });

    gameWrapper->HookEvent("Function TAGame.PRI_TA.EventStartVoteToForfeitDisabledChanged", [this](std::string eventName) {
        LOG("5 - EventStartVoteToForfeitDisabledChanged #######################################################");
        });

    gameWrapper->HookEvent("Function TAGame.GameEvent_TA.EventCanVoteForfeitChanged", [this](std::string eventName) {
        LOG("9 - EventCanVoteForfeitChanged #######################################################");
        });


    // yet unknown events
    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnForfeitVoteStarted", [this](std::string eventName) {
        LOG("? - OnForfeitVoteStarted ##########################################################");
        });

    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnCanVoteForfeitChanged", [this](std::string eventName) {
        LOG("11 - OnCanVoteForfeitChanged #######################################################");
        });

    // Hook event to log forfeit initiator
    gameWrapper->HookEvent("Function TAGame.PRI_TA.EventVoteToForfeit", [this](std::string eventName) {
        LOG("Forfeit initiated #######################################################");
        LogForfeitInitiator();
        });
}

void InstantFF::InitializeCurrentPlayers()
{
    LOG("InitializeCurrentPlayers called");

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

        LOG("############ Player: " + pri.GetPlayerName().ToString());
        pri.ServerVoteToForfeit();
    }
}

void InstantFF::LogForfeitInitiator()
{
    ServerWrapper sw = gameWrapper->GetOnlineGame();
    if (sw.IsNull()) {
        LOG("ServerWrapper is null");
        return;
    }

    auto forfeitInitiators = sw.GetForfeitInitiatorIDs();
    if (forfeitInitiators.Count() == 0) {
        LOG("No forfeit initiators found");
        return;
    }

    for (int i = 0; i < forfeitInitiators.Count(); ++i) {
        SteamID initiatorID = forfeitInitiators.Get(i);
        LOG("Forfeit Initiator: " + std::to_string(initiatorID.ID));
    }
}