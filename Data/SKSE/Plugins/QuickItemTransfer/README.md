# QuickItemTransfer Configuration Files

This directory contains TXT configuration files organized into category-specific folders.

## Folder Structure

Each category has its own dedicated folder:

- `raw_food/` - Raw food items
- `cooked_food/` - Cooked food items
- `sweets/` - Sweet food items
- `drinks/` - Drink items
- `ores/` - Ore and ingot items
- `gems/` - Gem items
- `leather_and_pelts/` - Leather and pelt items
- `building_materials/` - Building materials

## File Format

Each folder contains a TXT file (typically named after the category). Each TXT file should contain one FormID per line. Supported formats:

1. **Editor ID** (recommended for base game items):
   ```
   BearPelt
   FoodRabbitMeat
   ```

2. **Plugin~FormID** (for mod items):
   ```
   0x12345~MyMod.esp
   0xABCDE~AnotherMod.esm
   ```

3. **Hex FormID** (full FormID):
   ```
   0x000DB5D2
   0x00064B33
   ```

## Comments

Lines starting with `#` or `;` are treated as comments and ignored.
Empty lines are also ignored.

## Example Structure

```
Data/SKSE/Plugins/QuickItemTransfer/
├── raw_food/
│   └── raw_food.txt
├── cooked_food/
│   └── cooked_food.txt
├── sweets/
│   └── sweets.txt
├── drinks/
│   └── drinks.txt
├── ores/
│   └── ores.txt
├── gems/
│   └── gems.txt
├── leather_and_pelts/
│   └── leather_and_pelts.txt
└── building_materials/
    └── building_materials.txt
```

## Performance

All files are loaded in parallel during plugin initialization for optimal performance.

## Notes

- The example files contain placeholder entries. Replace them with actual FormIDs for your setup.
- If a file is missing, the corresponding category will be empty (no error).
- Invalid FormID entries will be logged as warnings but won't prevent loading.
- Each category folder can contain additional TXT files if needed for organization.
