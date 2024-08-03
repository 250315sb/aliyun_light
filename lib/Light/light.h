#ifndef LIGHT_H
#define LIGHT_H

#include "Arduino.h"

#define LED_FREQ                                5000     //frequency
#define LED_RESOLUTION                          13       //Resolution

#define LEDW_CHANNEL                            0        //Warm tone PWM channel
#define LEDW_INCREASE_BUTTON                    32       //Warm tone brightening button
#define LEDW_DECREASE_BUTTON                    33       //Warm tone dimming button
#define LED_WARM                                18       //Warm tone PWM output port

#define LEDC_CHANNEL                            2        //Cool tone PWM channel
#define LEDC_INCREASE_BUTTON                    14       //Cool tone brightening button
#define LEDC_DECREASE_BUTTON                    12       //Cool tone dimming button
#define LED_COLD                                5        //Cool color PWM output port

#define MODE_PROTECT_BUTTON                     17       //Eye protection mode button
#define MODE_DAYTIME_BUTTON                     16       //Day mode button
#define MODE_NIGHT_BUTTON                       4        //Night mode button

//各模式下冷暖色调参数
#define PROTECT_LEDW_BRIGHTNESS                 300      //Eye protection mode warm color brightness
#define PROTECT_LEDC_BRIGHTNESS                 150      //Eye protection mode cool color brightness
#define DAYTIME_LEDW_BRIGHTNESS                 100      //Day mode warm brightness
#define DAYTIME_LEDC_BRIGHTNESS                 400      //Day mode cool brightness
#define NIGHT_LEDW_BRIGHTNESS                   400      //Night mode warm color brightness
#define NIGHT_LEDC_BRIGHTNESS                   100      //Night mode cool brightness

void PinsInit();
bool KeyStroke_LongPass(int pin);
bool KeyStroke_ShortPass(int pin,bool *botton);
void PWM_AddDimming(int *Brightness,int channel);
void PWM_MinusDimming(int *Brightness,int channel);
void Mode_Adjustment(int *Brightness,int LEDX_CHANNEL,int MODE_LED_BRIGHTNESS);
void Receive_ChangeLED(int *Receive_LastBrightness,int *Receive_Brightness,int CHANNEL);

#endif // !LIGHT_H

