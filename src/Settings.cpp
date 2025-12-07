#include "Settings.h"
#include "CLibUtilsQTR/FormReader.hpp"
#include <thread>
#include <atomic>
#include <cassert>

namespace {
    constexpr auto TXT_BASE_FOLDER = "Data/SKSE/Plugins/QuickItemTransfer";
    
    std::mutex g_loadingMutex;
    
    std::string trim(const std::string& str) {
        const auto start = std::ranges::find_if(str, [](const unsigned char ch) {
            return !std::isspace(ch);
        });
        const auto end = std::find_if(str.rbegin(), str.rend(), [](const unsigned char ch) {
            return !std::isspace(ch);
        }).base();
        return (start < end) ? std::string(start, end) : std::string();
    }
    
    // Helper function to load FormIDs from a TXT file into a set
    void LoadFormIDsFromFile(const std::filesystem::path& filepath, std::unordered_set<FormID>& target_set) {
        if (!std::filesystem::exists(filepath)) {
            logger::warn("TXT file not found: {}", filepath.string());
            return;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            logger::error("Failed to open TXT file: {}", filepath.string());
            return;
        }
        
        std::set<FormID> local_set;
        std::string line;
        int line_number = 0;
        
        while (std::getline(file, line)) {
            line_number++;
            
            line = trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }

            const auto form_id = FormReader::GetFormEditorIDFromString(line);
            if (form_id != 0) {
                local_set.insert(form_id);
            } else {
                logger::warn("Invalid FormID at line {} in file {}: {}", line_number, filepath.string(), line);
            }
        }
        
        if (!local_set.empty()) {
            std::lock_guard lock(g_loadingMutex);
            target_set.insert(local_set.begin(), local_set.end());
            logger::info("Loaded {} forms from {}", local_set.size(), filepath.string());
        }
    }
}

void FormLists::GetAllFormLists() {
    logger::info("Loading form lists from TXT files in {}", TXT_BASE_FOLDER);

    std::filesystem::create_directories(TXT_BASE_FOLDER);

    struct CategoryMapping {
        std::string category_folder;
        std::unordered_set<FormID>* target_set;
    };

    std::vector<CategoryMapping> mappings = {{"raw_food", &all_raw_food},
                                             {"cooked_food", &all_cooked_food},
                                             {"sweets", &all_sweets},
                                             {"drinks", &all_drinks},
                                             {"building_materials", &all_building_materials}};

    // ---- collect all tasks first ----
    struct LoadTask {
        std::filesystem::path filepath;
        std::unordered_set<FormID>* target_set;
    };

    std::vector<LoadTask> tasks;

    for (const auto& [category_folder, target_set] : mappings) {
        std::filesystem::path dirpath = std::filesystem::path(TXT_BASE_FOLDER) / category_folder;

        if (!std::filesystem::exists(dirpath)) {
            logger::warn("Category folder not found: {}", dirpath.string());
            continue;
        }

        for (const auto& entry : std::filesystem::directory_iterator(dirpath)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            const auto& path = entry.path();
            if (!path.has_extension() || path.extension() != ".txt") {
                continue;
            }

            tasks.push_back(LoadTask{path, target_set});
        }
    }

    if (tasks.empty()) {
        logger::info("No TXT files found for any category.");
        return;
    }

    constexpr std::size_t MAX_WORKERS = 8;

    const unsigned hw = std::thread::hardware_concurrency();
    std::size_t worker_count = hw ? static_cast<std::size_t>(hw) : 2;
    worker_count = std::min<std::size_t>(worker_count, MAX_WORKERS);
    worker_count = std::min<std::size_t>(worker_count, tasks.size());

    std::atomic<std::size_t> nextIndex{0};
    std::vector<std::thread> workers;
    workers.reserve(worker_count);

    for (std::size_t i = 0; i < worker_count; ++i) {
        workers.emplace_back([&]() {
            for (;;) {
                const std::size_t index = nextIndex.fetch_add(1, std::memory_order_relaxed);
                if (index >= tasks.size()) {
                    break;
                }

                auto& [filepath, target_set] = tasks[index];
                LoadFormIDsFromFile(filepath, *target_set);
            }
        });
    }

    for (auto& t : workers) {
        if (t.joinable()) {
            t.join();
        }
    }

    for (const auto& [category_folder, target_set] : mappings) {
        logger::info("Total loaded for category '{}': {}", category_folder, target_set->size());
    }
}

void FormLists::LoadKeywords() {
    vendorItemKeywords[0] = RE::TESForm::LookupByID<RE::BGSKeyword>(0x914ed);
    vendorItemKeywords[1] = RE::TESForm::LookupByID<RE::BGSKeyword>(0x914ec);
    vendorItemKeywords[2] = RE::TESForm::LookupByID<RE::BGSKeyword>(0x914ea);
    vendorItemKeywords[3] = RE::TESForm::LookupByID<RE::BGSKeyword>(0xA0E56);
    vendorItemKeywords[4] = RE::TESForm::LookupByID<RE::BGSKeyword>(0x6BBE9);
    vendorItemKeywords[5] = RE::TESForm::LookupByID<RE::BGSKeyword>(0xF5CB0);
    assert(std::ranges::all_of(FormLists::vendorItemKeywords, [](const auto& keyword) { return keyword != nullptr; }) && "Failed to load all vendor item keywords");
}

bool FormLists::IsByKW(const RE::TESBoundObject* a_item, std::unordered_set<FormID>& a_cache, const int a_kw_index) {
    const auto formid = a_item->GetFormID();
    if (a_cache.contains(formid)) {
        return true;
    }
    if (a_item->HasKeywordInArray({ vendorItemKeywords[a_kw_index] }, false)) {
        a_cache.insert(formid);
        return true;
    }
    return false;
}

bool FormLists::IsShield(RE::TESBoundObject* a_item) {
    if (const auto asd = a_item->As<RE::BGSBipedObjectForm>()) {
        return asd->IsShield();
    }
    return false;
}

bool FormLists::IsClothing(RE::TESBoundObject* a_item) {
    if (const auto asd = a_item->As<RE::BGSBipedObjectForm>()) {
        return asd->IsClothing();
    }
    return false;
}

bool FormLists::IsArmorStrict(RE::TESBoundObject* a_item) { 
    if (a_item->IsArmor() && !IsClothing(a_item) && !IsShield(a_item) && !IsJewelry(a_item)) {
        return true;
    } 
    return false;
}

bool FormLists::IsBookSkill(RE::TESBoundObject* a_item) {
    const auto book = a_item->As<RE::TESObjectBOOK>();
    return book ? book->TeachesSkill() : false;
}

bool FormLists::IsBookSpell(RE::TESBoundObject* a_item) {
    const auto book = a_item->As<RE::TESObjectBOOK>();
    return book ? book->TeachesSpell() : false;
}

bool FormLists::IsBookStrict(RE::TESBoundObject* a_item) {
    const auto book = a_item->As<RE::TESObjectBOOK>();
    return book ? !IsBookSkill(a_item) && !IsBookSpell(a_item) && !IsRecipe(a_item) : false;
}
