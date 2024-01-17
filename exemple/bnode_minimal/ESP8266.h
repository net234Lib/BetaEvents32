// definition de IO pour beta four

//#define __BOARD__ESP8266__

#define LED_ON LOW



//Pin out NODEMCU ESP8266
#define D0  16    //!LED_BUILTIN only on old versio
#define D1  5     //       I2C_SCL
#define D2  4     //       I2C_SDA
#define D3  0     //!FLASH    BEEP_PIN on bNode
#define D4  2     //!LED2     LED_BUILTIN on new vesion

#define D5  14    //!SPI_CLK    
#define D6  12    //!SPI_MISO   
#define D7  13    //!SPI_MOSI   
#define D8  15    //!BOOT_STS            

#define BP0_PIN   D3                 //  Flash button
#define LED0_PIN  LED_BUILTIN   //   By default Led0 is on LED_BUILTIN should be set to 2 on new nodemcu
