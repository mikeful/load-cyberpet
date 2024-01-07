#ifndef PLAYER_H
#define PLAYER_H

#include "Arduino.h"
#include "squirrel_hash.h"

const unsigned int exp_gain_base = 45;
const unsigned int exp_gain_multiplier = 5;
const float exp_base = 1000.0;
const float exp_growth = 1.13;

int gain_exp(int target_level, unsigned int* level, uint64_t* experience, unsigned int* multiplier, unsigned int seed);
unsigned int get_exp_gain(int level, int target_level, int multiplier);
uint64_t get_level_exp_req(int level);
unsigned int get_level_up_bonus(int level, int current_multiplier, unsigned int seed);

#endif