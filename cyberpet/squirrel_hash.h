#ifndef SQUIRREL_HASH_H
#define SQUIRREL_HASH_H

unsigned int squirrel(int pos, unsigned int seed);
unsigned int squirrel_2d(int pos_x, int pos_y, unsigned int seed);
unsigned int squirrel_3d(int pos_x, int pos_y, int pos_z, unsigned int seed);
unsigned int squirrel_4d(int pos_x, int pos_y, int pos_z, int pos_t, unsigned int seed);

#endif