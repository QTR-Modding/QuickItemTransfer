#include "Utils.h"

namespace {
    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    void OnMessage(SKSE::MessagingInterface::Message* a_message) {
        if (a_message->type == SKSE::MessagingInterface::kDataLoaded) {
            FormLists::GetAllFormLists();
            FormLists::LoadKeywords();
            SKSE::GetPapyrusInterface()->Register(Utils::PapyrusFunctions);
        }
    }
}


SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    Utils::SetupLog();
    logger::info("Plugin loaded");
    Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    return true;
}