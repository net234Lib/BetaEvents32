// definition de IO pour beta four

//#define __BOARD__ESP8266__

#define LED_ON LOW



//Pin out NODEMCU ESP8266
#define D0  16    //!LED_BUILTIN
#define D1  5     //       I2C_SCL
#define D2  4     //       I2C_SDA
#define D3  0     //!FLASH    BEEP_PIN
#define D4  2     //!LED2     PN532_POWER_PIN

#define D5  14    //!SPI_CLK    DOORLOCK_PIN Entrée porte verouillée
#define D6  12    //!SPI_MISO   GACHE_PIN
#define D7  13    //!SPI_MOSI   BP0_PIN
#define D8  15    //!BOOT_STS            

#define BP0_PIN   D3                 //  Flash button
#define LED0_PIN  LED_BUILTIN   //   By default Led0 is on LED_BUILTIN you can change it 16 on ESP
