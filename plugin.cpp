#include "PCH.h" 
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

// =============== INPUT HANDLER (for "I" key) ===============
class InputSink final : public RE::BSTEventSink<RE::InputEvent*>
{
public:
    static InputSink* GetSingleton()
    {
        static InputSink instance;
        return std::addressof(instance);
    }

    // DirectInput scancode for 'I' is 0x17
    static constexpr std::uint32_t kScanCode_I = 0x17;

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_events,
                                          RE::BSTEventSource<RE::InputEvent*>*) override
    {
        if (!a_events) return RE::BSEventNotifyControl::kContinue;

        for (auto e = *a_events; e; e = e->next) {
            if (e->eventType != RE::INPUT_EVENT_TYPE::kButton)
                continue;

            const auto* be = e->AsButtonEvent();
            if (!be || be->GetDevice() != RE::INPUT_DEVICE::kKeyboard)
                continue;

            // Only fire on "pressed" transition
            // Value semantics: IsDown() is true on press; isHeld has repeats—press is enough for proof.
            if (be->idCode == kScanCode_I && be->IsDown()) {
                RE::DebugNotification("I key detected!");
                if (auto* con = RE::ConsoleLog::GetSingleton())
                    con->Print("I key detected!");
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }
};

// =============== MENU OPEN/CLOSE HANDLER (Inventory) ===============
class MenuSink final : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
    static MenuSink* GetSingleton()
    {
        static MenuSink instance;
        return std::addressof(instance);
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event,
                                          RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
    {
        if (!a_event) return RE::BSEventNotifyControl::kContinue;

        // InventoryMenu open?
        if (a_event->opening && a_event->menuName == RE::InventoryMenu::MENU_NAME) {
            RE::DebugNotification("Inventory opened!");
            if (auto* con = RE::ConsoleLog::GetSingleton())
                con->Print("Inventory opened!");
            // 👉 Future: instead of just notifying, open our custom menu or swap the view here.
        }
        return RE::BSEventNotifyControl::kContinue;
    }
};

// =============== SKSE ENTRY ===============
namespace
{
    void RegisterSinks()
    {
        // Input
        if (auto* inputMgr = RE::BSInputDeviceManager::GetSingleton()) {
            inputMgr->AddEventSink(InputSink::GetSingleton());
        }

        // Menu open/close
        if (auto* ui = RE::UI::GetSingleton()) {
            ui->AddEventSink(MenuSink::GetSingleton());
        }
    }

    void OnSKSEMessage(SKSE::MessagingInterface::Message* m)
    {
        // Wait until data/UI are in a safe state, then register sinks
        using M = SKSE::MessagingInterface;
        switch (m->type) {
        case M::kDataLoaded:      // game data + UI system ready
        case M::kPostLoadGame:    // after a save loaded
        case M::kNewGame:         // after new game started
            RegisterSinks();
            break;
        default:
            break;
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);

    if (auto* msg = SKSE::GetMessagingInterface()) {
        msg->RegisterListener(OnSKSEMessage);
    }

    // Also register early (harmless if repeated after DataLoaded)
    RegisterSinks();

    RE::DebugNotification("test skse successful!");
    if (auto* con = RE::ConsoleLog::GetSingleton())
        con->Print("test skse successful!");

    return true;
}
