#include "PCH.h" 
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace {
    void OnMsg(SKSE::MessagingInterface::Message* m) {
        using M = SKSE::MessagingInterface;
        if (m->type == M::kNewGame || m->type == M::kPostLoadGame) {
            RE::DebugNotification("test skse succesful!");
            if (auto* con = RE::ConsoleLog::GetSingleton()) con->Print("test skse successful!");
        }
    }
}

extern "C" {
__declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    if (auto* m = SKSE::GetMessagingInterface()) {
        m->RegisterListener(OnMsg);
        return true;
    }
    return false;
}
}
