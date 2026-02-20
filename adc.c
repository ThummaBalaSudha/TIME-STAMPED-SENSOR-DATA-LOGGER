#include "types.h"          // Custom data types (u32, f32 etc.)
#include "delay.h"          // Delay functions
#include <LPC21xx.h>        // LPC21xx register definitions
#include "adc_defines.h"    // ADC related macro definitions

/*----------------------------------------------------
  Array to select ADC channel pins
  Each element contains PINSEL1 configuration value
  for corresponding ADC channel:
    CH0 -> P0.27
    CH1 -> P0.28
    CH2 -> P0.29
    CH3 -> P0.30
----------------------------------------------------*/
u32 adcChSel[4] = 
{
    AIN0_PIN_0_27,
    AIN1_PIN_0_28,
    AIN2_PIN_0_29,
    AIN3_PIN_0_30
};

/*----------------------------------------------------
  Init_ADC()

  Initializes ADC module for selected channel.

  chNo -> ADC channel number (0 to 3)
----------------------------------------------------*/
void Init_ADC(u32 chNo)
{
    // Clear previous pin function bits
    PINSEL1 &= ~(adcChSel[chNo]);

    // Set selected pin as ADC function
    PINSEL1 |= adcChSel[chNo];

    // Enable ADC:
    // PDN_BIT  -> Power up ADC
    // CLKDIV   -> Set ADC clock divider
    ADCR |= (1 << PDN_BIT) | (CLKDIV << CLKDIV_BITS);
}

/*----------------------------------------------------
  Read_ADC()

  Reads analog value from selected ADC channel.

  chNo     -> Channel number
  eAR      -> Pointer to store equivalent analog voltage
  adcDVal  -> Pointer to store digital ADC value (0–1023)
----------------------------------------------------*/
void Read_ADC(u32 chNo, f32 *eAR, u32 *adcDVal)
{
    // Clear channel selection bits (lower 8 bits)
    ADCR &= 0xFFFFFF00;

    // Select ADC channel and start conversion
    ADCR |= ((1 << ADC_CONV_START_BIT) | (1 << chNo));

    delay_us(3);   // Small delay for stabilization

    // Wait until conversion is complete (DONE bit becomes 1)
    while(((ADDR >> DONE_BIT) & 1) == 0);

    // Stop ADC conversion
    ADCR &= ~(1 << ADC_CONV_START_BIT);

    // Extract 10-bit digital value from ADDR register
    *adcDVal = ((ADDR >> DIGITAL_DATA_BITS) & 1023);

    // Convert digital value to analog voltage
    // Formula: Voltage = (Digital Value × Vref) / 1023
    // Here Vref = 3.3V
    *eAR = *adcDVal * (3.3 / 1023);
}