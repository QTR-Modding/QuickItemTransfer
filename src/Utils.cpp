#include "Utils.h"

RE::TESObjectREFR* Utils::GetMenuContainer() {
    RE::TESObjectREFR* container = nullptr;

    const auto UI = RE::UI::GetSingleton();
    if (const auto menu = UI ? UI->GetMenu<RE::ContainerMenu>() : nullptr) {
        const auto           refHandle = menu->GetTargetRefHandle();
        RE::TESObjectREFRPtr refr;
        RE::LookupReferenceByHandle(refHandle, refr);

        container = refr.get();
    }

    return container;
}

void Utils::TransferItemsOfType(RE::TESObjectREFR* akSource, RE::TESObjectREFR* akTarget, const ItemTypes item_type) {

    if (!akSource || !akTarget) return;
    if (!IsItemType(item_type)) return;

    const bool bExcludeSpecials = akSource->IsPlayerRef();
    const auto filter_func = GetFilterFunc(item_type);
    std::vector<std::pair<RE::TESBoundObject*, std::int32_t>> forms;

    for (const auto akSource_inv = akSource->GetInventory();
         auto& [item,data] : akSource_inv) {
        if (item->Is(RE::FormType::LeveledItem) || !item->GetPlayable()) {
			continue;
		}
        if (data.first <= 0) continue;
        if (!filter_func(item)) {
            continue;
        }
        if (bExcludeSpecials && (data.second->IsWorn() || data.second->IsFavorited() || data.second->IsQuestObject())) continue;
        forms.emplace_back(item, data.first);
    }

    for (const auto& [item, count] : forms) {
        akSource->RemoveItem(item, count, RE::ITEM_REMOVE_REASON::kRemove,nullptr, akTarget);
    }

    SKSE::GetTaskInterface()->AddUITask([akTarget, akSource]() {
        RE::SendUIMessage::SendInventoryUpdateMessage(akTarget,nullptr);
        RE::SendUIMessage::SendInventoryUpdateMessage(akSource,nullptr);
    });
}

void Utils::StartTransfer(RE::StaticFunctionTag*, const int iAction, const int iSubType) {
    const bool bIsTaking = iAction > 0 && iAction < 10;
    const auto container = GetMenuContainer();
    const auto player_ref = RE::PlayerCharacter::GetSingleton()->AsReference();
    RE::TESObjectREFR* akSource = bIsTaking ? container : player_ref;
    RE::TESObjectREFR* akTarget = bIsTaking ? player_ref : container;

    if (iAction == 1 || iAction == 12) {
        TransferItemsOfType(akSource, akTarget, kWeapon); // kWeapon
        TransferItemsOfType(akSource, akTarget, kAmmo); // kAmmo
    }
    else if (iAction == 2 || iAction == 13) {
        TransferItemsOfType(akSource, akTarget, kArmor); // kArmor
    }
    else if (iAction == 3 || iAction == 14) {
        if (iSubType == 0) TransferItemsOfType(akSource, akTarget, kPotion);  // kPotion
        if (iSubType == 1) TransferItemsOfType(akSource, akTarget, kPoison);  // kPoison
    }
    else if (iAction == 4 || iAction == 15) {
        TransferItemsOfType(akSource, akTarget, kScrollItem); // kScrollItem
    }
    else if (iAction == 5 || iAction == 16) { // Food Items, same as Potion form
        if (iSubType == 0) TransferItemsOfType(akSource, akTarget, kFood);
        if (iSubType == 1) TransferItemsOfType(akSource, akTarget, kRawFood);
        else if (iSubType == 2) TransferItemsOfType(akSource, akTarget, kCookedFood);
        else if (iSubType == 3) TransferItemsOfType(akSource, akTarget, kDrinks);
        else if (iSubType == 4) TransferItemsOfType(akSource, akTarget, kSweets);
    }
    else if (iAction == 6 || iAction == 17) {
        TransferItemsOfType(akSource, akTarget, kIngredient); // kIngredient
    }
    else if (iAction == 7 || iAction == 18) {
        TransferItemsOfType(akSource, akTarget, kBook); // kBook
    }
    else if (iAction == 8 || iAction == 19) {
        TransferItemsOfType(akSource, akTarget, kKey); // kKey
    }
    else if (iAction == 9 || iAction == 20) { // Misc Items
        if (iSubType == 0) TransferItemsOfType(akSource, akTarget, kMisc);
        else if (iSubType == 1) TransferItemsOfType(akSource, akTarget, kSoulGem);
        else if (iSubType == 2) TransferItemsOfType(akSource, akTarget, kOres);
        else if (iSubType == 3) TransferItemsOfType(akSource, akTarget, kGems);
        else if (iSubType == 4) TransferItemsOfType(akSource, akTarget, kLeatherNPelts);
        else if (iSubType == 5) TransferItemsOfType(akSource, akTarget, kBuildingMaterials);
    }
}

bool Utils::PapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("StartTransfer", "QuickItemTransfer_Script", StartTransfer);
    return true;
}


void Utils::SetupLog() {
    const auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    const auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
#else
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);
#endif
    logger::info("Name of the plugin is {}.", pluginName);
    logger::info("Version of the plugin is {}.", SKSE::PluginDeclaration::GetSingleton()->GetVersion());
}