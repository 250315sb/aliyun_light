#include "Arduino.h"
#include "aliyun.h"

extern int Current_LEDW_Brightness;
WiFiClient espClient;
PubSubClient client(espClient);

/**
 * @brief Process JSON messages
 *
 * Parse the incoming JSON string and process the data processing in it
 *
 * @param message JSON message characters data
 */
void processJsonMessage(char* message) 
{
  StaticJsonDocument<256> doc;                                                           // Set up a JSON data to ensure that you can store data
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Extract data from JSON
  const char* method = doc["method"];                                              // "thing.service.property.set"
  const char* id = doc["id"];                                                      // "248166957"
  int lcon0 = doc["items"]["lcon0"]["value"];                                                // -32767 ~ 32767
  int LightLcon = doc["params"]["LightLcon"];                                      // 1-10000
  const char* version = doc["version"];                                            // "1.0.0"

  Serial.print("Method: ");
  Serial.println(method);
  Serial.print("ID: ");
  Serial.println(id);
  Serial.print("lcon0: ");
  Serial.println(lcon0);
  Serial.print("Version: ");
  Serial.println(version);

  //Received information changes the state of the lamp
  Current_LEDW_Brightness = lcon0;

  //Serial.printf("%d",Current_LEDW_Brightness);
}


/**
 * @brief Callback function
 *
 * This callback function will be called when a message is received
 *
 * @param topic   Topic strings
 * @param payload Message payload byte pointer
 * @param length  The length of the message payload (in bytes)
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Convert payload to string
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  // Print the original message
  Serial.println(message);

  // Call the processJsonMessage function to process the message
  processJsonMessage(message);
}


/**
 * @brief Connect to wifi and give feedback
 *
 * If the connection is not successful, it will be connected all the time, and the feedback will be given after the WiFi connection is successful
 */
void WifiConnect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
    Serial.printf("WiFi Connecting" );
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(700);
      Serial.printf(".");
    }
    Serial.printf("\nConnection successful"); 
    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());  

    client.setServer(MQTT_SERVER, MQTT_PORT);                                         //Connect to MQTT
    client.setCallback(callback);
}

/**
 * @brief Check the MQTT connection
 *
 * Cycle check if the MQTT client is connected. If it is not connected, the system tries to connect to the MQTT server and outputs the connection status information
 *
 * If the connection is successful, will it be lost in the serial port? "McEtt Connecht"
 * If the connection fails, the serial port outputs "McEtt Connect:" plus the current status of the McEtter client
 * If the connection fails, the program waits for 5 seconds and then tries to connect again
 */
void MQTTCheckConnect() 
{
  while (!client.connected()) 
  {
    Serial.println("Connecting to MQTT Server...");
    if (client.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) 
    {
      Serial.println("MQTT Connected");
    } 
    else 
    {
      Serial.print("MQTT Connect err: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

/**
 * @brief Post a message
 *
 * Create a StaticJsonDocument object, set the JSON data, and serialize it as a string,
 * The JSON string is then published to the specified topic using the MQTT client
 *
 * @note Make sure that the MQTT client is properly connected before calling this function
 */
void pubMsg() 
{
  // Create a StaticJsonDocument object large enough to store the expected JSON data
  StaticJsonDocument<256> doc;
 
  // Set JSON data
  doc["id"] = "123";
  doc["version"] = "1.0";
  doc["sys"]["ack"] = 0;
  doc["params"]["lcon0"] = 0;
  doc["method"] = "thing.event.property.post";

  // Serialize JSON data as strings
  char jsonOutput[256];
  serializeJson(doc, jsonOutput);

  // Use the modified pubMsg function to send JSON characters
  if (client.publish(PubTopic, jsonOutput)) 
  {
    Serial.println("Publish success");
    // Subscribe to the topic after the connection is successful
    client.subscribe(SubTopic);
  } 
  else 
  {
    Serial.println("Publish fail");
  }
}

/**
 * @brief Sends LED brightness
 *
 * The LED brightness is sent with JSON data
 *
 * @param last_Brightness    LED previous brightness
 * @param Brightness         The current brightness of the LED
 */
void SendLED_Msg(int *last_Brightness,int *Brightness)
{
  // Create a StaticJsonDocument object that is large enough to store the expected JSON data
  StaticJsonDocument<256> light_doc;

  // Set JSON data
  light_doc["id"] = "123";
  light_doc["version"] = "1.0";
  light_doc["sys"]["ack"] = 0;
  light_doc["params"]["lcon0"] = *Brightness;
  light_doc["method"] = "thing.event.property.post";

  char light_jsonOutput[256];
  serializeJson(light_doc, light_jsonOutput);
  if(*last_Brightness != *Brightness)
  {
    client.publish(PubTopic, light_jsonOutput);                                  //ESP32 sends data to Alibaba Cloud Topic
    //client.publish(PubTopic_computer, light_jsonOutput);                         //ESP32 sends data to Computer Topic
    *last_Brightness = *Brightness;
  }
}


