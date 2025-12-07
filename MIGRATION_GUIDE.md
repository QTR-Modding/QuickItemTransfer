# TXT-Based Settings Migration Guide

## Overview

QuickItemTransfer has migrated from ESP-based form list loading to a TXT-based configuration system. This change provides better flexibility and easier maintenance of item categorizations.

## What Changed

### Before (ESP-based)
- Form lists were stored in "Quick Item Transfer.esp"
- Required xEdit or Creation Kit to modify
- Hard-coded FormIDs in the plugin file

### After (TXT-based)
- Form lists are stored in simple TXT files
- Easy to edit with any text editor
- Supports multiple FormID formats for maximum compatibility

## Configuration Location

All TXT configuration files are located in:
```
Data/SKSE/Plugins/QuickItemTransfer/
```

## File Structure

Each category has its own TXT file:
- `raw_food.txt` - Raw food items
- `cooked_food.txt` - Cooked food items
- `sweets.txt` - Sweet items
- `drinks.txt` - Beverages
- `ores.txt` - Ores and ingots
- `gems.txt` - Gems and precious stones
- `leather_and_pelts.txt` - Leather and animal pelts
- `building_materials.txt` - Building materials

## FormID Format

Each file contains one FormID per line. Three formats are supported:

### 1. Editor ID (Recommended for base game)
```
BearPelt
FoodRabbitMeat
IngotIron
```

### 2. Plugin~FormID (For mod items)
```
0x12345~MyMod.esp
0xABCDE~AnotherMod.esm
```

### 3. Hex FormID (Full FormID)
```
0x000DB5D2
0x00064B33
```

## Comments and Formatting

- Lines starting with `#` or `;` are treated as comments
- Empty lines are ignored
- Leading and trailing whitespace is automatically trimmed

Example:
```
# Raw Food Items for QuickItemTransfer

# Vanilla Skyrim meats
FoodRabbitMeat
FoodVenisonMeat

# Modded items from HuntingMod
0x00801~HuntingMod.esp
0x00802~HuntingMod.esp
```

## Migration Steps

If you were using the old ESP-based system:

1. Locate your existing "Quick Item Transfer.esp"
2. Open it in xEdit or Creation Kit
3. Export the FormLists to text format
4. Convert to the new TXT file format
5. Place files in `Data/SKSE/Plugins/QuickItemTransfer/`
6. Remove or disable the ESP file

## Performance

- All TXT files are loaded in parallel during plugin initialization
- Each file is processed by a separate thread for optimal performance
- Loading is fast even with large lists

## Troubleshooting

### Missing Files
If a TXT file is missing, that category will simply be empty. A warning will be logged but the plugin will continue to work.

### Invalid FormIDs
Invalid FormID entries are logged as warnings but don't prevent loading. Check your logs if items aren't categorizing correctly:
```
SKSE/Plugins/QuickItemTransfer.log
```

### Format Errors
Common mistakes:
- Forgetting the `0x` prefix for hex FormIDs
- Using the wrong delimiter (should be `~` not `|` or `:`)
- Including spaces in FormIDs (they will be trimmed but may cause issues)

## Advanced Usage

### Custom Categories
To create custom item lists, simply edit the appropriate TXT file with your preferred text editor. Changes take effect after restarting the game.

### Sharing Configurations
TXT files can be easily shared with other users or included in mod packages. Just distribute the files with clear instructions on where to place them.

### Version Control
TXT files work great with version control systems like Git, making it easy to track changes and collaborate on item categorizations.

## Technical Details

For developers interested in the implementation:
- Uses CLibUtilsQTR's FormReader helpers for parsing
- Thread-safe loading with mutex-protected merging
- Follows the same pattern as AlchemyOfTime mod
- Each thread builds a local set before merging to minimize contention
