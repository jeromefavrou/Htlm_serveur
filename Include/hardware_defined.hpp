#ifndef HARDWARE_DEFINED_HPP_INCLUDED
#define HARDWARE_DEFINED_HPP_INCLUDED

#include "utility.hpp"

class Rasp_version : public Nocopyable
{
    public:
        typedef enum PI_3BP : std::uint8_t const
        {
            PIN_7=4,
            GPIO_4= PIN_7,
            GPIO_GCLK= PIN_7,

            PIN_3=2,
            GPIO_2=PIN_3,
            SDA1=PIN_3,

            PIN_5=3,
            GPIO3=PIN_5,
            SCL1=PIN_5,

            PIN_11=17,
            GPIO17=PIN_11,
            GPIO_GEN0=PIN_11,

        }Pin_gpio;
};

typedef volatile uint32_t * Hardware_ptr;

typedef enum class HARDWARE_VERSION  : int const {BCM2835=0,BCM2836,BCM2837,BCM2837B0,BCM2711} Soc_hardware_version;

typedef enum class PO_GP : int const {GPFSEL=0,GPSET,GPCLR,GPLEV,GPEDS,GPREN,GPFEN,GPHEN,GPLEN,GPAREN,GPAFEN,GPPUD,GPPUDCLK,} SoC_gp_physical_offset;

typedef enum class PA : int const {GPIO=0,ST,PADS,CLOCK,SPI,BSC0,BSC1,PWM,} SoC_physical_addr;

typedef enum class FSEL : std::uint8_t const {INPUT = 0,OUTPUT ,ALT0, ALT1 ,ALT2,ALT3 ,ALT4 ,ALT5,MASK,}Function_Select;

typedef enum class LEV : std::uint8_t const {LOW=0x00,HIGH=0x01,}Level;

typedef enum class BAUD : unsigned long const{BR_9600=9600,BR_19200=19200,BR_57600=57600,BR_115200=115200,} Baud_Rate;

#endif // HARDWARE_DEFINED_HPP_INCLUDED
