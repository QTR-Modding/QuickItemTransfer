#include "Settings.h"
#include "CLibUtilsQTR/FormReader.hpp"
#include <future>
#include <fstream>
#include <filesystem>
#include <mutex>

namespace {
    // Base folder for all TXT configuration files
    constexpr auto TXT_BASE_FOLDER = "Data/SKSE/Plugins/QuickItemTransfer";
    
    // Mutex for thread-safe loading
    // Note: Each thread builds a local set, so mutex is only used during final merge
    // This minimizes contention and preserves most of the parallel loading benefit
    std::mutex g_loadingMutex;
    
    // Helper function to load FormIDs from a TXT file into a set
    void LoadFormIDsFromFile(const std::string& filepath, std::set<FormID>& target_set) {
        if (!std::filesystem::exists(filepath)) {
            logger::warn("TXT file not found: {}", filepath);
            return;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            logger::error("Failed to open TXT file: {}", filepath);
            return;
        }
        
        // Local set to collect FormIDs (for thread safety)
        // Each thread builds its own local set independently (no contention)
        std::set<FormID> local_set;
        std::string line;
        int line_number = 0;
        
        while (std::getline(file, line)) {
            line_number++;
            
            // Trim whitespace
            line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), line.end());
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Parse the FormID using CLibUtilsQTR helper
            if (const auto form_id = FormReader::GetFormEditorIDFromString(line); form_id > 0) {
                local_set.insert(form_id);
            } else {
                logger::warn("Invalid FormID at line {} in file {}: {}", line_number, filepath, line);
            }
        }
        
        file.close();
        
        // Merge local set into target set with mutex protection
        // This is the only point where contention can occur, but it's very brief
        if (!local_set.empty()) {
            std::lock_guard<std::mutex> lock(g_loadingMutex);
            target_set.insert(local_set.begin(), local_set.end());
            logger::info("Loaded {} forms from {}", local_set.size(), filepath);
        }
    }
}

void FormLists::GetAllFormLists() {
    logger::info("Loading form lists from TXT files in {}", TXT_BASE_FOLDER);
    
    // Create the base directory if it doesn't exist
    std::filesystem::create_directories(TXT_BASE_FOLDER);
    
    // Define the file mappings (filename -> target set pointer)
    // Using pointers to avoid reference lifetime issues with async
    struct FileSetMapping {
        std::string filename;
        std::set<FormID>* target_set;
    };
    
    std::vector<FileSetMapping> mappings = {
        {"raw_food.txt", &all_raw_food},
        {"cooked_food.txt", &all_cooked_food},
        {"sweets.txt", &all_sweets},
        {"drinks.txt", &all_drinks},
        {"ores.txt", &all_ores},
        {"gems.txt", &all_gems},
        {"leather_and_pelts.txt", &all_leather_n_pelts},
        {"building_materials.txt", &all_building_materials}
    };
    
    // Load files in parallel using std::async
    std::vector<std::future<void>> futures;
    futures.reserve(mappings.size());
    
    for (const auto& mapping : mappings) {
        const std::string filepath = std::string(TXT_BASE_FOLDER) + "/" + mapping.filename;
        std::set<FormID>* target_ptr = mapping.target_set;
        
        // Launch async task for each file
        // Capture by value to avoid dangling references
        futures.emplace_back(std::async(std::launch::async, [filepath, target_ptr]() {
            LoadFormIDsFromFile(filepath, *target_ptr);
        }));
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.get();
    }
    
    logger::info("Form list loading complete. Loaded {} raw food, {} cooked food, {} sweets, {} drinks, {} ores, {} gems, {} leather/pelts, {} building materials",
        all_raw_food.size(), all_cooked_food.size(), all_sweets.size(), all_drinks.size(),
        all_ores.size(), all_gems.size(), all_leather_n_pelts.size(), all_building_materials.size());
}

// Legacy GetFormList function removed - now using TXT-based loading
void FormLists::GetFormList(const RE::FormID a_localid, std::set<FormID>& a_formset) {
    // This function is no longer used but kept for API compatibility
    // All form loading now happens through TXT files
    logger::warn("GetFormList called with local ID {:x} but ESP-based loading is deprecated", a_localid);
}
