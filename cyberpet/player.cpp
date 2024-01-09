#include "player.h"

int gain_exp(int target_level, unsigned int *level, uint64_t *experience, unsigned int *multiplier, unsigned int seed) {
  int level_ups = 0;
  unsigned int multiplier_bonus = 0;
  uint64_t level_up_req = get_level_exp_req(*level + 1);

  unsigned int gain = get_exp_gain(*level, target_level, *multiplier);
  *experience += gain;

  while (*experience >= level_up_req) {
    *level = *level + 1;
    *experience = *experience - level_up_req;

    level_ups++;
    level_up_req = get_level_exp_req(*level + 1);

    multiplier_bonus = get_level_up_bonus(*level, *multiplier, seed);
    if (multiplier_bonus > 0) {
      *multiplier += multiplier_bonus;
    }
  }

  return level_ups;
}

unsigned int get_exp_gain(int level, int target_level, int multiplier) {
  float level_diff_factor = pow((float)target_level / (float)level, 4.0);

  unsigned int base_gain = exp_gain_base + (exp_gain_multiplier * (unsigned int)target_level);
  unsigned int mod_gain = (unsigned int)((float)base_gain * level_diff_factor);

  return (unsigned int)multiplier * max((unsigned int)1, mod_gain);
}

uint64_t get_level_exp_req(int level) {
  return (uint64_t)(exp_base * pow(exp_growth, (float)level));
}

unsigned int get_level_up_bonus(int level, int current_multiplier, unsigned int seed) {
  if (level >= 10 && level % 5 == 0) {
    if (current_multiplier <= 7) {
      return 1;
    } else {
      int max_bonus = 2 + max(1, level / 10);

      if (max_bonus <= 2) {
        return max_bonus;
      }

      return 2 + (squirrel_2d(level, current_multiplier, seed) % (max_bonus - 2));
    }
  }

  return 0;
}