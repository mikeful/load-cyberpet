#include "items.h"

// Equipment stats range 1-9, tier range 0-5, flaws range 0-4
int get_equip_stat(int equip_id, byte stat) {
  return equipments[equip_id][stat];
}

int get_equip_stat(int equip_id, byte stat, int tier) {
  return equipments[equip_id][stat] + tier;
}

int get_equip_stat(int equip_id, byte stat, int tier, int flaws, unsigned int seed) {
  int base_stat = equipments[equip_id][stat];
  int mod_stat = base_stat + tier;

  if (base_stat == 1 || base_stat == 4 || flaws == 0) {
    return mod_stat;
  }

  // TODO Distribute flaws to stats
  return mod_stat - 1;
}

// 10 -> 100 -> 1000 -> 10000 -> 100000
int get_equip_upgrade_cost(float base_cost, int tier) {
  return pow(base_cost, (float)tier + 1.0);
}

uint64_t get_attack_damage_stat(byte weapon_type, uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int) {
  switch(weapon_type) {
    case EQUIP_TYPE_WEAPON:
      return stat_str;
    break;
    case EQUIP_TYPE_RANGED:
      return stat_dex;
    break;
    case EQUIP_TYPE_MAGIC:
      return stat_int;
    break;
  }

  // Unarmed
  return stat_str + stat_dex + stat_int;
}

int get_attack_count(byte weapon_type) {
  switch(weapon_type) {
    case EQUIP_TYPE_WEAPON:
      return 2;
    break;
    case EQUIP_TYPE_RANGED:
      return 3;
    break;
    case EQUIP_TYPE_MAGIC:
      return 1;
    break;
  }

  // Unarmed
  return 3;
}

int get_attack_base_damage(byte weapon_type) {
  switch(weapon_type) {
    case EQUIP_TYPE_WEAPON:
      return 6;
    break;
    case EQUIP_TYPE_RANGED:
      return 8;
    break;
    case EQUIP_TYPE_MAGIC:
      return 10;
    break;
  }

  // Unarmed
  return 4;
}

int get_attack_cost(byte weapon_type) {
  switch(weapon_type) {
    case EQUIP_TYPE_WEAPON:
      return 2;
    break;
    case EQUIP_TYPE_RANGED:
      return 3;
    break;
    case EQUIP_TYPE_MAGIC:
      return 1;
    break;
  }

  // Unarmed
  return 9;
}

int get_attack_effect(byte weapon_type, byte effect_type, unsigned int seed) {
  int tile_variation = squirrel_2d((int)weapon_type, (int)effect_type, seed) % 3;

  switch(effect_type) {
    case EQUIP_EFFECT_MISS:
      return weapon_effects_miss[weapon_type][tile_variation];
    case EQUIP_EFFECT_HIT:
      return weapon_effects_hit[weapon_type][tile_variation];
    case EQUIP_EFFECT_CRIT:
      return weapon_effects_crit[weapon_type][tile_variation];
  }

  return -1;
}
