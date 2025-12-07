# QuickItemTransfer Configuration Files

This directory contains TXT configuration files that define which items belong to each category.

## File Format

Each TXT file should contain one FormID per line. Supported formats:

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

## Configuration Files

- `raw_food.txt` - Raw food items (e.g., raw meat, fish)
- `cooked_food.txt` - Cooked food items (e.g., cooked beef, grilled leeks)
- `sweets.txt` - Sweet food items (e.g., sweetrolls, honeycomb)
- `drinks.txt` - Drink items (e.g., wine, ale, mead)
- `ores.txt` - Ore and ingot items
- `gems.txt` - Gem items
- `leather_and_pelts.txt` - Leather and pelt items
- `building_materials.txt` - Building materials (e.g., clay, stone)

## Performance

All files are loaded in parallel during plugin initialization for optimal performance.

## Notes

- The example files contain placeholder entries. Replace them with actual FormIDs for your setup.
- If a file is missing, the corresponding category will be empty (no error).
- Invalid FormID entries will be logged as warnings but won't prevent loading.
