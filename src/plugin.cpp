#include "Utils.h"

void OnMessage(SKSE::MessagingInterface::Message* a_message) {
	if (a_message->type == SKSE::MessagingInterface::kDataLoaded) {
		logger::info("Data loaded");
		GetAllFormLists();
	    SKSE::GetPapyrusInterface()->Register(Utils::PapyrusFunctions);
	}
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    Utils::SetupLog();
    logger::info("Plugin loaded");
    Init(skse);
	SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    return true;
}