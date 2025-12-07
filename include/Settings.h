#pragma once

namespace FormLists {
    inline std::set<FormID> all_raw_food;
    inline std::set<FormID> all_cooked_food;
    inline std::set<FormID> all_sweets;
    inline std::set<FormID> all_drinks;
    inline std::set<FormID> all_ores;
    inline std::set<FormID> all_gems;
    inline std::set<FormID> all_leather_n_pelts;
    inline std::set<FormID> all_building_materials;

    void GetAllFormLists();
    void GetFormList(RE::FormID a_localid, std::set<FormID>& a_formset);
    inline void GetAllGems() { GetFormList(0x1, all_gems); }
    inline void GetAllOres() { GetFormList(0x2, all_ores); }
    inline void GetAllRawFood(){GetFormList(0x3, all_raw_food);}
    inline void GetAllCookedFood() { GetFormList(0x4, all_cooked_food); }
    inline void GetAllDrinks() { GetFormList(0x5, all_drinks); }
    inline void GetAllSweets() { GetFormList(0x801, all_sweets); }
    inline void GetAllLeatherNPelts() { GetFormList(0x802, all_leather_n_pelts); }
    inline void GetAllBuildingMaterials() { GetFormList(0x803, all_building_materials); }

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
