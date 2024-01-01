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
