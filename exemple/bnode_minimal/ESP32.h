// definition de IO pour esp32

//#define __BOARD__ESP8266__

#define LED_ON LOW




//Pin out NODEMCU ESP32
#define G0  0     //BP BOOT
//#define D1  5     //       I2C_SCL
//#define D2  4     //       I2C_SDA
#define G2  2     //!FLASH    BEEP_PIN


#define LED_BUILTIN G2
#define BP0_PIN   G0                 //  Flash button
#define LED0_PIN  LED_BUILTIN   //   By default Led0 is on LED_BUILTIN you can change it 16 on ESP
