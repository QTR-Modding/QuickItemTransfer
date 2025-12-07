#pragma once
#include <unordered_set>

namespace FormLists {

    // Gem, Ore/Ingot, Animal Hide, Raw Food, Jewelry, Recipe
    inline std::array<RE::BGSKeyword*, 6> vendorItemKeywords = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

    inline std::unordered_set<FormID> all_raw_food;
    inline std::unordered_set<FormID> all_cooked_food;
    inline std::unordered_set<FormID> all_sweets;
    inline std::unordered_set<FormID> all_drinks;
    inline std::unordered_set<FormID> all_building_materials;

    // doesnt have folder, loaded on demand
    inline std::unordered_set<FormID> all_ingot_ores;
    inline std::unordered_set<FormID> all_gems;
    inline std::unordered_set<FormID> all_leather_n_pelts;
    inline std::unordered_set<FormID> all_jewelry;
    inline std::unordered_set<FormID> all_recipes;

    // Main entry point: loads all form lists from TXT files (multithreaded)
    void GetAllFormLists();
    void LoadKeywords();

    inline bool IsCookedFood(const FormID a_formid) { return all_cooked_food.contains(a_formid); }
    inline bool IsSweets(const FormID a_formid) { return all_sweets.contains(a_formid); }
    inline bool IsDrinks(const FormID a_formid) { return all_drinks.contains(a_formid); }
    inline bool IsBuildingMaterials(const FormID a_formid) { return all_building_materials.contains(a_formid); }

    // doesnt have folder, loaded on demand
    bool IsByKW(const RE::TESBoundObject* a_item, std::unordered_set<FormID>& a_cache, int a_kw_index);
    inline bool IsGems(const RE::TESBoundObject* a_item) { return IsByKW(a_item, all_gems, 0); }
    inline bool IsIngotsOres(const RE::TESBoundObject* a_item) { return IsByKW(a_item, all_ingot_ores, 1); }
    inline bool IsLeatherNPelts(const RE::TESBoundObject* a_item) { return IsByKW(a_item, all_leather_n_pelts, 2); }
    inline bool IsRawFood(const RE::TESBoundObject* a_item) { return IsByKW(a_item, all_raw_food, 3); }
    inline bool IsJewelry(const RE::TESBoundObject* a_item) { return IsByKW(a_item, all_jewelry, 4); }
    inline bool IsRecipe(const RE::TESBoundObject* a_item) { return IsByKW(a_item, all_recipes, 5); }

    bool IsShield(RE::TESBoundObject* a_item);
    bool IsClothing(RE::TESBoundObject* a_item);
    bool IsArmorStrict(RE::TESBoundObject* a_item);

    bool IsNote(RE::TESBoundObject* a_item);
    bool IsBookSkill(RE::TESBoundObject* a_item);
    bool IsBookSpell(RE::TESBoundObject* a_item);
    bool IsBookStrict(RE::TESBoundObject* a_item);
}

enum ItemTypes {
    kWeapon,
    kAmmo,
    kArmorStrict,
    kJewelry,
    kShield,
    kClothing,
    kPoison,
    kPotion,
    kScrollItem,
    kFood,
    kRawFood,
    kCookedFood,
    kSweets,
    kDrinks,
    kIngredient,
    kBookAll,
    kBookSpell,
    kBookSkill,
    kBookRecipe,
    kBookStrict,
    kKey,
    kMiscAll,
    kSoulGem,
    kOres,
    kGems,
    kLeatherNPelts,
    kBuildingMaterials,
    kNone
};

inline bool IsItemType(const ItemTypes a_itemtype) {
	return a_itemtype < kNone;
}

inline std::function<bool(RE::TESBoundObject*)> GetFilterFunc(const ItemTypes a_itemtype) {
	switch (a_itemtype) {
	case kWeapon:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Weapon); };
	case kAmmo:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Ammo); };
	case kArmorStrict:
		return [](RE::TESBoundObject* a_obj) { return FormLists::IsArmorStrict(a_obj); };
	case kJewelry:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsJewelry(a_obj); };
    case kShield:
        return [](RE::TESBoundObject* a_obj) { return FormLists::IsShield(a_obj); };
	case kClothing:
        return [](RE::TESBoundObject* a_obj) { return FormLists::IsClothing(a_obj); };
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
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsRawFood(a_obj); };
	case kCookedFood:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsCookedFood(a_obj->GetFormID()); };
	case kSweets:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsSweets(a_obj->GetFormID()); };
	case kDrinks:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsDrinks(a_obj->GetFormID()); };
	case kIngredient:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Ingredient); };
	case kBookAll:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::Book); };
	case kBookSkill:
        return [](RE::TESBoundObject* a_obj) { return FormLists::IsBookSkill(a_obj); };
    case kBookSpell:
	    return [](RE::TESBoundObject* a_obj) { return FormLists::IsBookSpell(a_obj); };
	case kBookRecipe:
        return [](RE::TESBoundObject* a_obj) { return FormLists::IsRecipe(a_obj); };
    case kBookStrict:
	    return [](RE::TESBoundObject* a_obj) { return FormLists::IsBookStrict(a_obj); };
	case kKey:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::KeyMaster); };
	case kMiscAll:
        return [](const RE::TESBoundObject* a_obj) {return a_obj->Is(RE::FormType::Misc) || a_obj->IsSoulGem() || a_obj->Is(RE::FormType::Light); };
	case kSoulGem:
		return [](const RE::TESBoundObject* a_obj) { return a_obj->Is(RE::FormType::SoulGem); };
	case kOres:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsIngotsOres(a_obj); };
	case kGems:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsGems(a_obj); };
	case kLeatherNPelts:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsLeatherNPelts(a_obj); };
	case kBuildingMaterials:
		return [](const RE::TESBoundObject* a_obj) { return FormLists::IsBuildingMaterials(a_obj->GetFormID()); };
	default:
        logger::error("GetFilterFunc: Unknown item type {}", static_cast<int>(a_itemtype));
		return [](RE::TESBoundObject*) { return false; };
	}
}
