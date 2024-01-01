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

#define EQUIP_MELEE 5
#define EQUIP_RANGED 6
#define EQUIP_MAGIC 7
#define EQUIP_ICON 8

const int equipments[][8] = {
    /* None */     {1,1,1,1, 0,0,0, -1},
    /* Ring */     {1,2,3,4, 0,0,1, 61,},
    /* WizHat */   {2,1,3,4, 0,0,1, 23,},
    /* Armor */    {3,1,2,4, 0,0,0, 39,},
    /* Scope */    {1,3,2,4, 0,0,0, 2003,},
    /* Ammo */     {2,3,1,4, 0,0,0, 133,},
    /* Shield */   {3,2,1,4, 0,0,0, 3,},
    /* ZapStaff */ {3,2,4,1, 1,1,1, 145,},
    /* FireWand */ {2,3,4,1, 0,1,1, 138,},
    /* Spear */    {4,3,2,1, 1,1,0, 123,},
    /* Daggers */  {3,4,2,1, 1,1,0, 102,},
    /* FireBow */  {2,4,3,1, 1,1,1, 129,},
    /* IceSword */ {4,2,3,1, 1,0,1, 111,},
    /* ZapSword */ {4,1,3,2, 1,0,1, 109,},
    /* VoidGun */  {1,4,3,2, 0,1,1, 2027,},
    /* 2Swords */  {3,4,1,2, 1,0,0, 1538,},
    /* Bow */      {4,3,1,2, 0,1,0, 128,},
    /* VoidWand */ {1,3,4,2, 0,1,1, 140,},
    /* WizStaff */ {3,1,4,2, 1,1,1, 141,},
    /* FireBook */ {2,1,4,3, 0,1,1, 177,},
    /* ZapBook */  {1,2,4,3, 0,1,1, 178,},
    /* Sword */    {4,2,1,3, 1,0,0, 103,},
    /* Crossbow */ {2,4,1,3, 0,1,0, 130,},
    /* Gun */      {1,4,2,3, 0,1,0, 2004,},
    /* Axe */      {4,1,2,3, 1,0,0, 117,},
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

//const byte items[][] = {};

int get_equip_stat(int equip_id, byte stat);
int get_equip_stat(int equip_id, byte stat, int tier);
int get_equip_stat(int equip_id, byte stat, int tier, int flaws, unsigned int seed);
int get_equip_upgrade_cost(int base_cost, int tier);

#endif
