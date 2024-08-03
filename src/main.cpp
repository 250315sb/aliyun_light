#include "Arduino.h"
#include "aliyun.h"
#include "light.h"

//LED parameters for reception
int Receive_LEDW_LastBrightness = 0;                        //Receives LEDW lastbrightness
int Receive_LEDC_LastBrightness = 0;                        //Receives LEDC lastbrightness

//LED parameters for sending
int Current_LEDW_Brightness = 0;                            //Cool brightness
int Current_LEDC_Brightness = 0;                            //Warm color brightness
int Sends_LEDW_LastBrightness = 0;                          //last Cool brightness
int Sends_LEDC_LastBrightness = 0;                           //last Warm color brightness

//Short press the button marker
bool BOTTON_flag = true; 

void setup()
{
    Serial.begin(115200);
    PinsInit();
    WifiConnect();
    MQTTCheckConnect();
    pubMsg();
}

void loop() 
{
    client.loop();
    //Adjust the brightness of warm and cold tones by pressing buttons
    if (KeyStroke_LongPass(LEDW_INCREASE_BUTTON))
    {
        PWM_AddDimming(&Current_LEDW_Brightness,LEDW_CHANNEL);                                                              //Long press the button to increase the PWM of the warm color channel continuously
    }
    if (KeyStroke_LongPass(LEDW_DECREASE_BUTTON ))                                                                       
    {
        PWM_MinusDimming(&Current_LEDW_Brightness,LEDW_CHANNEL);                                                            //Long press the button to reduce the PWM of warm color channel
    }
    if (KeyStroke_LongPass(LEDC_INCREASE_BUTTON))
    {
        PWM_AddDimming(&Current_LEDC_Brightness,LEDC_CHANNEL);                                                              //Long press the button to increase the PWM of the cold color channel continuously
    }
    if (KeyStroke_LongPass(LEDC_DECREASE_BUTTON ))
    {
        PWM_MinusDimming(&Current_LEDC_Brightness,LEDC_CHANNEL);                                                            //Long press the button to reduce the PWM of the cold color channel
    }

    //Adjust parameters of three different modes
    if (KeyStroke_ShortPass(MODE_PROTECT_BUTTON,&BOTTON_flag))                                              
    {
        Mode_Adjustment(&Current_LEDW_Brightness,LEDW_CHANNEL,PROTECT_LEDW_BRIGHTNESS);                                     //Eye protection mode warm tone parameter configuration
        Mode_Adjustment(&Current_LEDC_Brightness,LEDC_CHANNEL,PROTECT_LEDC_BRIGHTNESS);                                     //Eye protection mode cold tone parameter configuration
    }
    if (KeyStroke_ShortPass(MODE_DAYTIME_BUTTON,&BOTTON_flag))
    {
        Mode_Adjustment(&Current_LEDW_Brightness,LEDW_CHANNEL,DAYTIME_LEDW_BRIGHTNESS);                                     //Day mode warm tone parameter configuration
        Mode_Adjustment(&Current_LEDC_Brightness,LEDC_CHANNEL,DAYTIME_LEDC_BRIGHTNESS);                                     //Day mode cold tone parameter configuration
    }
    if (KeyStroke_ShortPass(MODE_NIGHT_BUTTON,&BOTTON_flag))
    {
        Mode_Adjustment(&Current_LEDW_Brightness,LEDW_CHANNEL,NIGHT_LEDW_BRIGHTNESS);                                       //Night mode warm tone parameter configuration
        Mode_Adjustment(&Current_LEDC_Brightness,LEDC_CHANNEL,NIGHT_LEDC_BRIGHTNESS);                                       //Night mode cold tone parameter configuration   
    }
    //The current LED data is sent
    SendLED_Msg(&Sends_LEDW_LastBrightness,&Current_LEDW_Brightness);

    //Check whether the received data changes the LED status
    Receive_ChangeLED(&Receive_LEDW_LastBrightness,&Current_LEDW_Brightness,LEDW_CHANNEL);
}


