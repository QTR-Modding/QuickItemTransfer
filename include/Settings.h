#pragma once

// FormLists: TXT-based settings loading
// All form lists are now loaded from TXT files located in:
//   Data/SKSE/Plugins/QuickItemTransfer/
// 
// Expected TXT files (one per set):
//   - raw_food.txt              -> all_raw_food
//   - cooked_food.txt           -> all_cooked_food
//   - sweets.txt                -> all_sweets
//   - drinks.txt                -> all_drinks
//   - ores.txt                  -> all_ores
//   - gems.txt                  -> all_gems
//   - leather_and_pelts.txt     -> all_leather_n_pelts
//   - building_materials.txt    -> all_building_materials
//
// Each TXT file should contain one FormID per line in one of these formats:
//   - Editor ID (e.g., "BearPelt")
//   - Plugin~FormID (e.g., "0x12345~Skyrim.esm")
//   - Hex FormID (e.g., "0x00012345")
//
// Lines starting with # or ; are treated as comments and ignored.
// Empty lines are also ignored.
//
// Loading is performed in parallel for better startup performance.

namespace FormLists {
    inline std::set<FormID> all_raw_food;
    inline std::set<FormID> all_cooked_food;
    inline std::set<FormID> all_sweets;
    inline std::set<FormID> all_drinks;
    inline std::set<FormID> all_ores;
    inline std::set<FormID> all_gems;
    inline std::set<FormID> all_leather_n_pelts;
    inline std::set<FormID> all_building_materials;

    // Main entry point: loads all form lists from TXT files (multithreaded)
    void GetAllFormLists();
    
    // Legacy function kept for API compatibility (deprecated)
    void GetFormList(RE::FormID a_localid, std::set<FormID>& a_formset);

    inline bool IsRawFood(const FormID a_formid) { return all_raw_food.contains(a_formid); }
    inline bool IsCookedFood(const FormID a_formid) { return all_cooked_food.contains(a_formid); }
    inline bool IsSweets(const FormID a_formid) { return all_sweets.contains(a_formid); }
    inline bool IsDrinks(const FormID a_formid) { return all_drinks.contains(a_formid); }
    inline bool IsOres(const FormID a_formid) { return all_ores.contains(a_formid); }
    inline bool IsGems(const FormID a_formid) { return all_gems.contains(a_formid); }
    inline bool IsLeatherNPelts(const FormID a_formid) { return all_leather_n_pelts.contains(a_formid); }
    inline bool IsBuildingMaterials(const FormID a_formid) { return all_building_materials.contains(a_formid); }
}



enum ItemTypes {
    kWeapon,
    kAmmo,
    kArmor,
    kPoison,
    kPotion,
    kScrollItem,
    kFood,
    kRawFood,
    kCookedFood,
    kSweets,
    kDrinks,
    kIngredient,
    kBook,
    kKey,
    kMisc,
    kSoulGem,
    kOres,
    kGems,
    kLeatherNPelts,
    kBuildingMaterials,
    kNone
};

inline bool IsItemType(const ItemTypes a_itemtype) {
	return a_itemtype >= kWeapon && a_itemtype < kNone;
}

inline std::function<bool(RE::TESBoundObject*)> GetFilterFunc(const ItemTypes a_itemtype) {
	switch (a_itemtype) {
	case kWeapon:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Weapon); };
	case kAmmo:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Ammo); };
	case kArmor:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Armor); };
    case kPoison:
        return [](RE::TESBoundObject* a_obj) {
            const auto temp = a_obj->As<RE::AlchemyItem>();
            return temp && !temp->IsFood() && temp->IsPoison();
        };
	case kPotion:
		return [](RE::TESBoundObject* a_obj) {
			const auto temp = a_obj->As<RE::AlchemyItem>();
			return temp && !temp->IsFood() && !temp->IsPoison();
		};
	case kScrollItem:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Scroll); };
	case kFood:
		return [](const RE::TESBoundObject* a_obj) { const auto alch_item = a_obj->As<RE::AlchemyItem>(); return alch_item && alch_item->IsFood(); };
	case kRawFood:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsRawFood(a_obj->GetFormID()); };
	case kCookedFood:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsCookedFood(a_obj->GetFormID()); };
	case kSweets:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsSweets(a_obj->GetFormID()); };
	case kDrinks:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsDrinks(a_obj->GetFormID()); };
	case kIngredient:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Ingredient); };
	case kBook:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Book); };
	case kKey:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::KeyMaster); };
	case kMisc:
        return [](const RE::TESBoundObject* a_obj) {return a_obj->Is(RE::FormType::Misc) || a_obj->Is(RE::FormType::Light); };
	case kSoulGem:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::SoulGem); };
	case kOres:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsOres(a_obj->GetFormID()); };
	case kGems:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsGems(a_obj->GetFormID()); };
	case kLeatherNPelts:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsLeatherNPelts(a_obj->GetFormID()); };
	case kBuildingMaterials:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsBuildingMaterials(a_obj->GetFormID()); };
	default:
		return [](RE::TESBoundObject*) { return false; };
	}
}
