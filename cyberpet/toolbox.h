#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "Arduino.h"

String format_number4(unsigned long amount);
String format_number3(unsigned long amount);
String format_number2(unsigned long amount);
int mod(int x, int y);

#endif