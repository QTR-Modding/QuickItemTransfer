#pragma once
#include "Settings.h"

namespace Utils {
    RE::TESObjectREFR* GetMenuContainer();

    void TransferItemsOfType(RE::TESObjectREFR* akSource, RE::TESObjectREFR* akTarget, const ItemTypes item_type);

    void StartTransfer(RE::StaticFunctionTag*, const int iAction, const int iSubType = 0);

    bool PapyrusFunctions(RE::BSScript::IVirtualMachine* vm);

    void SetupLog();
}
