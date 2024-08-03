#include "Arduino.h"
#include "light.h"

/**
 * @brief Define channel, press key
 *
 * Define buttons, channels and bind each tone PWM output pin to the channel
 */
void PinsInit()
{
//Configure eye protection, daytime, night mode buttons
  pinMode(MODE_PROTECT_BUTTON,INPUT_PULLUP);
  pinMode(MODE_DAYTIME_BUTTON,INPUT_PULLUP);
  pinMode(MODE_NIGHT_BUTTON,INPUT_PULLUP);

//Warm colors
  pinMode(LEDW_INCREASE_BUTTON,INPUT_PULLUP);
  pinMode(LEDW_DECREASE_BUTTON,INPUT_PULLUP);
  ledcSetup(LEDW_CHANNEL,LED_FREQ,LED_RESOLUTION);        //Set the warm tone channel
  ledcAttachPin(LED_WARM,LEDW_CHANNEL);                   //Binding Pins to Channels

//Cool colors
  pinMode(LEDC_INCREASE_BUTTON,INPUT_PULLUP);             
  pinMode(LEDC_DECREASE_BUTTON,INPUT_PULLUP);
  ledcSetup(LEDC_CHANNEL,LED_FREQ,LED_RESOLUTION);        //Set up the cool channel
  ledcAttachPin(LED_COLD,LEDC_CHANNEL);                   //Binding Pins to Channels
}


/**
 * @brief Long press the button
 *
 * When the button is long pressed, the function always returns True, which is used to flexibly adjust the brightness
 *
 * @param pin Button corresponding pin
 */
bool KeyStroke_LongPass(int pin)
{
  bool LED_flag = false;

  if(!digitalRead(pin))
  {
    delay(10);
    if(!digitalRead(pin))
    {
      LED_flag = true;
    }
  }
  else
  {
    LED_flag = false;
  }
  return LED_flag;
}

/**
 * @brief Short press button
 *
 * When the button is pressed, the function returns True to display the data in the current mode.
 *
 * @param pin    Button corresponding pin
 * @param botton Select mode button flag pointer
 */
bool KeyStroke_ShortPass(int pin,bool *botton_flag)
{
  bool LED_flag = false;
  if(!digitalRead(pin) && *botton_flag)
  {
    delay(100);
    if(!digitalRead(pin))
    {
      LED_flag = true;
      *botton_flag = false;
    }
  }
  else 
  {
    LED_flag = false;
  }
  if(digitalRead(pin))
  {
    *botton_flag = true;
  }
  return LED_flag;
}

/**
 * @brief Within the rated range, adjust PWM to increase the brightness
 *
 * If the brightness is less than the maximum resolution, the brightness increases by 1 unit per cycle.
 *
 * @param Brightness Current brightness pointer
 * @param channel    PWM pin corresponding to the channel number
 */
void PWM_AddDimming(int *Brightness,int channel)
{
  if (*Brightness < pow(2,LED_RESOLUTION))
  {
    *Brightness = *Brightness + 1;
  }
  ledcWrite(channel,*Brightness);
  delay(10);
}

/**
 * @brief Within the rated range, adjust PWM to reduce the brightness
 *
 * If the brightness is greater than 0, the brightness decreases by 1 unit each time the cycle is completed.
 *
 * @param Brightness Current brightness pointer
 * @param channel    PWM pin corresponding to the channel number
 */
void PWM_MinusDimming(int *Brightness,int channel)
{
  if (*Brightness > 0)
  {
    *Brightness = *Brightness - 1;
  }
  ledcWrite(channel,*Brightness);
  delay(10);
}

/**
 * @brief Choose different modes, warm and cool tones, brightness
 *
 * Fill in different cold and warm color parameters to achieve different modes
 *
 * @param Brightness           Current brightness pointer
 * @param LEDX_CHANNEL         The hue channel that needs editing
 * @param MODE_LEDX_BRIGHTNESS Brightness of a certain hue in the current mode
 */
void Mode_Adjustment(int *Brightness,int LEDX_CHANNEL,int MODE_LED_BRIGHTNESS)
{
  *Brightness = MODE_LED_BRIGHTNESS;
  ledcWrite(LEDX_CHANNEL,*Brightness);
}

/**
 * @brief After the receive data ,Change yhe LED
 * 
 * Change the lamp parameters after receiving the data
 * 
 * @param Receive_LastBrightness     Receives the data from the previous LED
 * @param Receive_Brightness         The data from the LED light is received
 * @param CHANNEL                    Channels that need to be changed
 */
void Receive_ChangeLED(int *Receive_LastBrightness,int *Receive_Brightness,int CHANNEL)
{
  if(*Receive_LastBrightness != *Receive_Brightness)
  {
    ledcWrite(CHANNEL,*Receive_Brightness);
    *Receive_LastBrightness = *Receive_Brightness;
  }
}

/*example*/
/*#include "light.h"

//Current brightness of each hue
int Current_LEDW_Brightness = 0;                       //Cool brightness
int Current_LEDC_Brightness = 0;                       //Warm color brightness
//Select mode button flag
bool BOTTON_flag = true; 

void setup() 
{
//Initialization pin
  PinsInit();
}
void loop() 
{
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
}
*/

