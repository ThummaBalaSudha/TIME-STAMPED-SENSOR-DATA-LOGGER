#include "adc.h"          // ADC function declarations
#include "types.h"        // Custom data types (u32, f32, u8 etc.)
#include "adc_defines.h"  // ADC channel definitions

/*----------------------------------------------------
  Read_LM35()

  Reads temperature from LM35 sensor using ADC.

  Parameter:
    tType -> 'C' for Celsius
             'F' for Fahrenheit

  Returns:
    Temperature value (integer format)
----------------------------------------------------*/
u32 Read_LM35(u8 tType)
{
    u32 adcDVal;      // Stores raw ADC digital value
    f32 eAR, tDeg;    // eAR = analog voltage (in volts)
                      // tDeg = temperature in degrees

    // Read ADC channel CH0
    // eAR  -> equivalent analog voltage
    // adcDVal -> raw ADC digital value
    Read_ADC(CH0, &eAR, &adcDVal);
    
    // LM35 gives 10mV per degree Celsius
    // If voltage = 0.30V -> temperature = 30°C
    // So temperature = voltage * 100
    tDeg = eAR * 100;

    // If temperature type is Celsius
    if(tType == 'C')
    {
        // Do nothing (already in Celsius)
    }

    // If temperature type is Fahrenheit
    else if(tType == 'F')
    {
        // Convert Celsius to Fahrenheit
        // Formula: F = (C × 9/5) + 32
        tDeg = ((tDeg * (9/5.0)) + 32);
    }

    // Return temperature value (converted to integer automatically)
    return tDeg;
}