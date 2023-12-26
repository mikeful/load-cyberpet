#include "toolbox.h"

String format_number(unsigned long amount) {
  String parsed_amount;

  //if (amount > 1000000000) {
  //	parsed_amount = String((unsigned long)(amount / 1000000000.0)) + "B";
  //} else
  if (amount >= 1000000000) {
    parsed_amount = String(amount / 1000000000.0, 1) + "B";
  } else if (amount >= 100000000) {
    parsed_amount = String((int)(amount / 1000000.0)) + "M";
  } else if (amount >= 10000000) {
    parsed_amount = "0" + String((int)(amount / 1000000.0)) + "M";
  } else if (amount >= 1000000) {
    parsed_amount = String(amount / 1000000.0, 1) + "M";
  } else if (amount >= 100000) {
    parsed_amount = String((int)(amount / 1000.0)) + "k";
  } else if (amount >= 10000) {
    parsed_amount = "0" + String((int)(amount / 1000.0)) + "k";
  } else if (amount >= 1000) {
    parsed_amount = String(amount);
  } else if (amount >= 100) {
    parsed_amount = "0" + String(amount);
  } else if (amount >= 10) {
    parsed_amount = "00" + String(amount);
  } else if (amount >= 1) {
    parsed_amount = "000" + String(amount);
  } else {
    parsed_amount = "0000";
  }

  return parsed_amount;
}