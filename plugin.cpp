#include "PCH.h" 
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace {
    void OnMsg(SKSE::MessagingInterface::Message* m) {
        using M = SKSE::MessagingInterface;
        if (m->type == M::kNewGame || m->type == M::kPostLoadGame) {
            RE::DebugNotification("test skse succesful!");
            if (auto* con = RE::ConsoleLog::GetSingleton()) con->Print("test skse succesful!");
        }
    }
}

extern "C" {
__declspec(dllexport) bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* skse, SKSE::PluginInfo* info) {
    info->infoVersion = SKSE::PluginInfo::kVersion;
    info->name = "testskse";
    info->version = 1;
    return !skse->IsEditor();
}

__declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    if (auto* m = SKSE::GetMessagingInterface()) {
        m->RegisterListener(OnMsg);
        return true;
    }
    return false;
}
}
