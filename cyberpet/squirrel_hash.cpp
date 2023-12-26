/*
    squirrel hash

    Provides a noise function which is fast and had good distribution. Based on SquirrelNoise5 -
    Squirrel's Raw Noise utilities (version 5) by Squirrel Eiserloh.

    This code is made available under the Creative Commons attribution 3.0 license (CC-BY-3.0 US):
    Attribution in source code comments (even closed-source/commercial code) is sufficient.
    License summary and text available at: https://creativecommons.org/licenses/by/3.0/us/
*/
#include "squirrel_hash.h"

// The base bit-noise constants were crafted to have distinctive and interesting
// bits, and have so far produced excellent experimental test results.
#define NOISE1 0xd2a80a3f  // 11010010101010000000101000111111
#define NOISE2 0xa884f197  // 10101000100001001111000110010111
#define NOISE3 0x6C736F4B  // 01101100011100110110111101001011
#define NOISE4 0xB79F3ABB  // 10110111100111110011101010111011
#define NOISE5 0x1b56c4f5  // 00011011010101101100010011110101

#define PRIME1 198491317
#define PRIME2 6542989
#define PRIME3 357239

#define MAX_VALUE 1 << 32


unsigned int squirrel(int pos, unsigned int seed = 0) {
  /*
    Returns an unsigned integer containing 32 reasonably-well-scrambled
    bits, based on a given (signed) integer input parameter `pos` and optional
    `seed`.  Kind of like looking up a value in an infinitely large
    non-existent table of previously generated random numbers.
    */
  unsigned int value = (unsigned int)pos;
  value *= NOISE1;
  value += seed;
  value ^= (value >> 9);
  value += NOISE2;
  value ^= (value >> 11);
  value *= NOISE3;
  value ^= (value >> 13);
  value += NOISE4;
  value ^= (value >> 15);
  value *= NOISE5;
  value ^= (value >> 17);

  // Cast into uint32 like the original `Squirrel`.
  //if as_float:
  //    return (pos % MAX_VALUE) / float(MAX_VALUE)

  //return pos % MAX_VALUE;
  return value;
}

unsigned int squirrel_2d(int pos_x, int pos_y, unsigned int seed = 0) {
  // Munge positions together to single position and get noise value for it
  return squirrel(pos_x + (PRIME1 * pos_y), seed = seed);
}

unsigned int squirrel_3d(int pos_x, int pos_y, int pos_z, unsigned int seed = 0) {
  // Munge positions together to single position and get noise value for it
  return squirrel(pos_x + (PRIME1 * pos_y) + (PRIME2 * pos_z), seed = seed);
}

unsigned int squirrel_4d(int pos_x, int pos_y, int pos_z, int pos_t, unsigned int seed = 0) {
  // Munge positions together to single position and get noise value for it
  return squirrel(
    pos_x + (PRIME1 * pos_y) + (PRIME2 * pos_z) + (PRIME3 * pos_t), seed = seed);
}

/*
class SquirrelRandom(Random):
    """
    Build Python Random compatible random number generator based on noise
    """

    _state = 0

    def seed(self, a=None, version=2):
        if a is None:
            a = 0
        self._seed = a

    def random(self):
        current_state = self._state
        self._state += 1
        return squirrel(current_state, self._seed) / float(MAX_VALUE)
*/
