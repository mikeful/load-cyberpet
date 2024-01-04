#include "toolbox.h"

String format_number4(uint64_t amount) {
  String parsed_amount;

  if (amount >= 100 * quintillion) {
    parsed_amount = String((int)(amount / quintillion)) + "Q";
  } else if (amount >= 10 * quintillion) {
    parsed_amount = "0" + String((int)(amount / quintillion)) + "Q";
  } else if (amount >= quintillion) {
    parsed_amount = String((double)amount / (double)quintillion, 1) + "Q";

  } else if (amount >= 100 * quadrillion) {
    parsed_amount = String((int)(amount / quadrillion)) + "q";
  } else if (amount >= 10 * quadrillion) {
    parsed_amount = "0" + String((int)(amount / quadrillion)) + "q";
  } else if (amount >= quadrillion) {
    parsed_amount = String((double)amount / (double)quadrillion, 1) + "q";

  } else if (amount >= 100 * trillion) {
    parsed_amount = String((int)(amount / trillion)) + "T";
  } else if (amount >= 10 * trillion) {
    parsed_amount = "0" + String((int)(amount / trillion)) + "T";
  } else if (amount >= trillion) {
    parsed_amount = String((double)amount / (double)trillion, 1) + "T";

  } else if (amount >= 100 * billion) {
    parsed_amount = String((int)(amount / billion)) + "B";
  } else if (amount >= 10 * billion) {
    parsed_amount = "0" + String((int)(amount / billion)) + "B";
  } else if (amount >= billion) {
    parsed_amount = String((double)amount / (double)billion, 1) + "B";

  } else if (amount >= 100 * million) {
    parsed_amount = String((int)(amount / million)) + "M";
  } else if (amount >= 10 * million) {
    parsed_amount = "0" + String((int)(amount / million)) + "M";
  } else if (amount >= million) {
    parsed_amount = String((double)amount / (double)million, 1) + "M";

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

String format_number3(uint64_t amount) {
  String parsed_amount;

  if (amount >= 10 * quintillion) {
    parsed_amount = String((int)(amount / quintillion)) + "Q";
  } else if (amount >= quintillion) {
    parsed_amount = "0" + String((int)(amount / quintillion)) + "Q";
  } else if (amount >= 100 * quadrillion) {
    parsed_amount = "." + String((int)((double)amount / (100.0 * (double)quadrillion))) + "Q";

  } else if (amount >= 10 * quadrillion) {
    parsed_amount = String((int)(amount / quadrillion)) + "q";
  } else if (amount >= quadrillion) {
    parsed_amount = "0" + String((int)(amount / quadrillion)) + "q";
  } else if (amount >= 100 * trillion) {
    parsed_amount = "." + String((int)((double)amount / (100.0 * (double)trillion))) + "q";

  } else if (amount >= 10 * trillion) {
    parsed_amount = String((int)(amount / trillion)) + "T";
  } else if (amount >= trillion) {
    parsed_amount = "0" + String((int)(amount / trillion)) + "T";
  } else if (amount >= 100 * billion) {
    parsed_amount = "." + String((int)((double)amount / (100.0 * (double)billion))) + "T";

  } else if (amount >= 10 * billion) {
    parsed_amount = String((int)(amount / billion)) + "B";
  } else if (amount >= billion) {
    parsed_amount = "0" + String((int)(amount / billion)) + "B";
  } else if (amount >= 100 * million) {
    parsed_amount = "." + String((int)((double)amount / (100.0 * (double)million))) + "B";

  } else if (amount >= 10 * million) {
    parsed_amount = String((int)(amount / million)) + "M";
  } else if (amount >= million) {
    parsed_amount = "0" + String((int)(amount / million)) + "M";
  } else if (amount >= 100000) {
    parsed_amount = "." + String((int)(amount / 100000.0)) + "M";

  } else if (amount >= 10000) {
    parsed_amount = String((int)(amount / 1000.0)) + "k";
  } else if (amount >= 1000) {
    parsed_amount = "0" + String((int)(amount / 1000.0)) + "k";
  } else if (amount >= 100) {
    parsed_amount = String(amount);
  } else if (amount >= 10) {
    parsed_amount = "0" + String(amount);
  } else if (amount >= 1) {
    parsed_amount = "00" + String(amount);
  } else {
    parsed_amount = "000";
  }

  return parsed_amount;
}

String format_number2(uint64_t amount) {
  String parsed_amount;

  if (amount >= 10 * quintillion) {
    parsed_amount = ">Q";

  } else if (amount >= quintillion) {
    parsed_amount = String((int)(amount / quintillion)) + "Q";
  } else if (amount >= 10 * quadrillion) {
    parsed_amount = "<Q";

  } else if (amount >= quadrillion) {
    parsed_amount = String((int)(amount / quadrillion)) + "q";
  } else if (amount >= 10 * trillion) {
    parsed_amount = "<q";

  } else if (amount >= trillion) {
    parsed_amount = String((int)(amount / trillion)) + "T";
  } else if (amount >= 10 * billion) {
    parsed_amount = "<T";

  } else if (amount >= billion) {
    parsed_amount = String((int)(amount / billion)) + "B";
  } else if (amount >= 10 * million) {
    parsed_amount = "<B";

  } else if (amount >= million) {
    parsed_amount = String((int)(amount / million)) + "M";
  } else if (amount >= 10000) {
    parsed_amount = "<M";

  } else if (amount >= 1000) {
    parsed_amount = String((int)(amount / 1000.0)) + "k";
  } else if (amount >= 100) {
    parsed_amount = "<k";
  } else if (amount >= 10) {
    parsed_amount = String(amount);
  } else if (amount >= 1) {
    parsed_amount = "0" + String(amount);
  } else {
    parsed_amount = "00";
  }

  return parsed_amount;
}

int mod(int x, int y) {
  if (x < 0) {
    return ((x + 1) % y) + (y - 1);
  } else {
    return x % y;
  }
}
