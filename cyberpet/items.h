#ifndef STAT_STR
#define STAT_STR 0
#define STAT_DEX 1
#define STAT_INT 2
#define STAT_VIT 3
#endif

#ifndef ITEMS_H
#define ITEMS_H

#include "Arduino.h"
#include "squirrel_hash.h"

#define EQUIP_TYPE 4
#define EQUIP_ICON 5

#define EQUIP_TYPE_UNARMED 0
#define EQUIP_TYPE_WEAPON 1
#define EQUIP_TYPE_RANGED 2
#define EQUIP_TYPE_MAGIC 3

const int equipments[][6] = {
    /* None */     {1,1,1,1, EQUIP_TYPE_UNARMED, -1},
    /* Ring */     {1,2,3,4, EQUIP_TYPE_UNARMED, 61,},
    /* WizHat */   {2,1,3,4, EQUIP_TYPE_UNARMED, 23,},
    /* Armor */    {3,1,2,4, EQUIP_TYPE_UNARMED, 39,},
    /* Scope */    {1,3,2,4, EQUIP_TYPE_UNARMED, 2003,},
    /* Ammo */     {2,3,1,4, EQUIP_TYPE_UNARMED, 133,},
    /* Shield */   {3,2,1,4, EQUIP_TYPE_UNARMED, 3,},
    /* ZapStaff */ {3,2,4,1, EQUIP_TYPE_MAGIC,   145,},
    /* FireWand */ {2,3,4,1, EQUIP_TYPE_MAGIC,   138,},
    /* Spear */    {4,3,2,1, EQUIP_TYPE_RANGED,  123,},
    /* Daggers */  {3,4,2,1, EQUIP_TYPE_WEAPON,  102,},
    /* FireBow */  {2,4,3,1, EQUIP_TYPE_RANGED,  129,},
    /* IceSword */ {4,2,3,1, EQUIP_TYPE_WEAPON,  111,},
    /* ZapSword */ {4,1,3,2, EQUIP_TYPE_WEAPON,  109,},
    /* VoidGun */  {1,4,3,2, EQUIP_TYPE_RANGED,  2027,},
    /* 2Swords */  {3,4,1,2, EQUIP_TYPE_WEAPON,  1538,},
    /* Bow */      {4,3,1,2, EQUIP_TYPE_WEAPON,  128,},
    /* VoidWand */ {1,3,4,2, EQUIP_TYPE_MAGIC,   140,},
    /* WizStaff */ {3,1,4,2, EQUIP_TYPE_MAGIC,   141,},
    /* FireBook */ {2,1,4,3, EQUIP_TYPE_MAGIC,   177,},
    /* ZapBook */  {1,2,4,3, EQUIP_TYPE_MAGIC,   178,},
    /* Sword */    {4,2,1,3, EQUIP_TYPE_WEAPON,  103,},
    /* Crossbow */ {2,4,1,3, EQUIP_TYPE_RANGED,  130,},
    /* Gun */      {1,4,2,3, EQUIP_TYPE_RANGED,  2004,},
    /* Axe */      {4,1,2,3, EQUIP_TYPE_WEAPON,  117,},
};

const String equipment_names[] = {
  "None",
  "Ring",
  "WizHat",
  "Armor",
  "Scope",
  "Ammo",
  "Shield",
  "ZapStaff",
  "FireWand",
  "Spear",
  "Daggers",
  "FireBow",
  "IceSword",
  "ZapSword",
  "VoidGun",
  "2Swords",
  "Bow",
  "VoidWand",
  "WizStaff",
  "FireBook",
  "ZapBook",
  "Sword",
  "Crossbow",
  "Gun",
  "Axe",
};

#define EQUIP_EFFECT_MISS 0
#define EQUIP_EFFECT_HIT 1
#define EQUIP_EFFECT_CRIT 2

const int weapon_effects_miss[4][3] = {
  {213,226,274}, // EQUIP_TYPE_UNARMED
  {213,226,274}, // EQUIP_TYPE_WEAPON
  {213,226,274}, // EQUIP_TYPE_RANGED
  {213,226,274}, // EQUIP_TYPE_MAGIC
};
const int weapon_effects_hit[4][4] = {
  {214,229,230}, // EQUIP_TYPE_UNARMED
  {214,229,230}, // EQUIP_TYPE_WEAPON
  {282,278,277}, // EQUIP_TYPE_RANGED
  {248,249,250}, // EQUIP_TYPE_MAGIC
};
const int weapon_effects_crit[4][4] = {
  {215,235,221}, // EQUIP_TYPE_UNARMED
  {215,235,221}, // EQUIP_TYPE_WEAPON
  {133,134,135}, // EQUIP_TYPE_RANGED
  {267,269,271}, // EQUIP_TYPE_MAGIC
};

//const byte items[][] = {};

int get_equip_stat(int equip_id, byte stat);
int get_equip_stat(int equip_id, byte stat, int tier);
int get_equip_stat(int equip_id, byte stat, int tier, int flaws, unsigned int seed);
int get_equip_upgrade_cost(int base_cost, int tier);

int get_attack_base_damage(byte weapon_type);
uint64_t get_attack_damage_stat(byte weapon_type, uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int);
uint64_t get_armor_rating(byte weapon_type);
int get_attack_count(byte weapon_type);
int get_attack_cost(byte weapon_type);
int get_attack_effect(byte weapon_type, byte effect_type, unsigned int seed);

#endif
