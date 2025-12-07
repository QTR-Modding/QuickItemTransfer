#include "Utils.h"

RE::TESObjectREFR* Utils::GetMenuContainer() {
    RE::TESObjectREFR* container = nullptr;

    const auto UI = RE::UI::GetSingleton();
    if (const auto menu = UI ? UI->GetMenu<RE::ContainerMenu>() : nullptr) {
        const auto refHandle = menu->GetTargetRefHandle();
        RE::TESObjectREFRPtr refr;
        RE::LookupReferenceByHandle(refHandle, refr);

        container = refr.get();
    }

    return container;
}

void Utils::TransferItemsOfType(RE::TESObjectREFR* akSource, RE::TESObjectREFR* akTarget, const ItemTypes item_type) {
    if (!akSource || !akTarget) return;
    if (!IsItemType(item_type)) return;

    float remaining_capacity = FLT_MAX;
    if (!akTarget->IsPlayerRef()) {
        if (const auto a_actor = akTarget->As<RE::Actor>()) {
            if (const auto actor_val_owner = a_actor->AsActorValueOwner()) {
                const auto total_capacity = actor_val_owner->GetActorValue(RE::ActorValue::kCarryWeight);
                const auto current_weight = actor_val_owner->GetActorValue(RE::ActorValue::kInventoryWeight);
                remaining_capacity = total_capacity - current_weight;
            }
        }
    }

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
        if (bExcludeSpecials && (data.second->IsWorn() || data.second->IsFavorited() || data.second->IsQuestObject()))
            continue;
        forms.emplace_back(item, data.first);
    }

    for (const auto& [item, count] : forms) {
        if (remaining_capacity <= 0.0f) break;
        akSource->RemoveItem(item, count, RE::ITEM_REMOVE_REASON::kRemove, nullptr, akTarget);
        remaining_capacity -= item->GetWeight() * count;
    }

    SKSE::GetTaskInterface()->AddUITask([akTarget, akSource]() {
        RE::SendUIMessage::SendInventoryUpdateMessage(akTarget, nullptr);
        RE::SendUIMessage::SendInventoryUpdateMessage(akSource, nullptr);
    });
}

void Utils::StartTransfer(RE::StaticFunctionTag*, const int iAction, const int iSubType) {
    const bool bIsTaking = iAction > 0 && iAction < 10;
    const auto container = GetMenuContainer();
    const auto player_ref = RE::PlayerCharacter::GetSingleton()->AsReference();
    RE::TESObjectREFR* akSource = bIsTaking ? container : player_ref;
    RE::TESObjectREFR* akTarget = bIsTaking ? player_ref : container;

    auto type = kNone;
    if (iAction == 1 || iAction == 12) {
        if (iSubType == 0) type = kWeapon;
        else type = kAmmo;
    } else if (iAction == 2 || iAction == 13) {
        switch (iSubType) {
            case 1:
                type = kJewelry;
                break;
            case 2:
                type = kShield;
                break;
            case 3:
                type = kClothing;
                break;
            default:
                type = kArmorStrict;
                break;
        }
    } else if (iAction == 3 || iAction == 14) {
        switch (iSubType) {
            case 1:
                type = kPoison;
                break;
            default:
                type = kPotion;
                break;
        }
    } else if (iAction == 4 || iAction == 15) {
        type = kScrollItem;
    } else if (iAction == 5 || iAction == 16) {
        switch (iSubType) {
            case 1:
                type = kRawFood;
                break;
            case 2:
                type = kCookedFood;
                break;
            case 3:
                type = kDrinks;
                break;
            case 4:
                type = kSweets;
                break;
            default:
                type = kFood;
                break;
        }
    } else if (iAction == 6 || iAction == 17) {
        type = kIngredient;
    } else if (iAction == 7 || iAction == 18) {
        switch (iSubType) {
            case 1:
                type = kBookStrict;
                break;
            case 2:
                type = kBookSpell;
                break;
            case 3:
                type = kBookSkill;
                break;
            case 4:
                type = kBookRecipe;
                break;
            default:
                type = kBookAll;
                break;
        }
    } else if (iAction == 8 || iAction == 19) {
        type = kKey;
    } else if (iAction == 9 || iAction == 20) {
        // Misc Items
        switch (iSubType) {
            case 1:
                type = kSoulGem;
                break;
            case 2:
                type = kOres;
                break;
            case 3:
                type = kGems;
                break;
            case 4:
                type = kLeatherNPelts;
                break;
            case 5:
                type = kBuildingMaterials;
                break;
            default:
                type = kMiscAll;
                break;
        }
    }

    TransferItemsOfType(akSource, akTarget, type);
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