// This work by Digital Concepts is licensed under a Creative Commons Attribution-Noncommercial-Share Alike 2.5 Australia License. Permissions beyond the scope of this license may be available at info@digitalconcepts.net.au.

// https://digitalconcepts.net.au/arduino/index.php?op=ESP32#wl32v3

#define Builtin_LED 35

#define GPIO0 0
#define GPIO1 1
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define GPIO5 5
#define GPIO6 6
#define GPIO7 7

#define GPIO10 10
#define GPIO11 11
#define GPIO12 12
#define GPIO13 13
#define GPIO14 14

#define GPIO19 19
#define GPIO20 20
#define GPIO21 21

#define GPIO26 26

#define GPIO33 33
#define GPIO34 34
#define GPIO35 35
#define GPIO36 36
#define GPIO37 37
#define GPIO38 38
#define GPIO39 39
#define GPIO40 40
#define GPIO41 41
#define GPIO42 42
#define GPIO43 43
#define GPIO44 44
#define GPIO45 45
#define GPIO46 46
#define GPIO47 47
#define GPIO48 48

// Avoid GPIO0, GPIO1, GPIO3, GPIO45, GPIO46
// OK to use if you understand the consequences — these
// pins are strapped at boot

// General
#define USR_Button GPIO0
#define VBAT_Read GPIO1
#define LED_Write GPIO35
#define Vext_Ctrl GPIO36
#define ADC_Ctrl GPIO37

#define TX GPIO43
#define RX GPIO44

// I2C
#define SDA GPIO19
#define SCL GPIO20

#define OLED_SDA GPIO17
#define OLED_SCL GPIO18
#define OLED_RST GPIO21

// LoRa SPI
#define CS GPIO8
#define NSS GPIO8
#define MOSI GPIO10
#define MISO GPIO11
#define SCK GPIO9
#define RST GPIO12
#define DIO0 GPIO26  // This may not be correct...
#define DIO1 GPIO14
