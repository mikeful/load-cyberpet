#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "Arduino.h"

const uint64_t million = 1000000;
const uint64_t billion = 1000000000;
const uint64_t trillion = 1000000000000;
const uint64_t quadrillion = 1000000000000000;
const uint64_t quintillion = 1000000000000000000;

String format_number4(uint64_t amount);
String format_number3(uint64_t amount);
String format_number2(uint64_t amount);
int mod(int x, int y);

#endif