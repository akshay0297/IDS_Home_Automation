#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

#include <HCSR04.h>

#define BUTTON  5
 
String apiKey = "ThingSpeak Channel WRITE API Key"; 

const char *ssid =  "wifi_ssid";     // replace with your wifi ssid and wpa2 key
const char *pass =  "wifi_password";
const char* server = "api.thingspeak.com";

WiFiClient client;

int switchState = 0; 
int oldSwitchState = 0; 
int lightsOn = 0; 

int button_led = 15;
int led = 16;
int sensor = 13;
int state = LOW;
int val = 0; 

int flag = 0;
int testing  = 0;

const int trigPin = 2; 
const int echoPin = 0;

UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);

void setup() { 
 pinMode(BUTTON, INPUT); 
 pinMode(led, OUTPUT);
 pinMode(button_led, OUTPUT); 
 pinMode(sensor, INPUT); 
 
 Serial.begin(115200);
 delay(10);

   Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
void loop() {
 switchState = digitalRead(BUTTON); 
 if (switchState != oldSwitchState) 
    {
      oldSwitchState = switchState;
       if (switchState == HIGH)
       {
          lightsOn = !lightsOn;
       }
    }
 if(lightsOn)
   {
        Serial.println("--------USER MODE---------");
        digitalWrite(button_led, HIGH);
        val = digitalRead(sensor);   
        if (val == HIGH) 
        {           
          digitalWrite(led, LOW);  
          delay(200);                
          if (state == LOW) 
          {
              if(flag == 1)
              {
                Serial.println("User is Moving !"); 
              }
              else if(flag == 0)
              {
                Serial.println("User is Moving !"); 
                Serial.println("Switchin on the Lights and Air conditioner ");
                flag = 1;
              }
              state = HIGH;
          }
        } 
        else 
        {
            digitalWrite(led, HIGH); 
            delay(200);            
            if (state == HIGH)
            {
                Serial.println("User Not Moving !!! ");
                double distance = distanceSensor.measureDistanceCm();
                Serial.print(distance);
                Serial.println(" cm");
                if(distance < 1.0 || distance > 450.0)
                {
                  if(flag == 1)
                  {
                     Serial.println("User not in the room. Switching off the lights and Air Conditioner ");
                     flag = 0;
                  }
                }
                else
                {
                   Serial.println("User in the room and not moving. Keeping the lights and AC on ");
                }
                state = LOW;
                delay(1500);
            }
        }
   } 
   else 
   {
    digitalWrite(button_led, LOW); 
    Serial.println("--------INTRUDER DETECTION MODE----------");
    val = digitalRead(sensor);   
    if (val == HIGH) 
    {           
      digitalWrite(led, LOW);   
      delay(200);              
      if (state == LOW) 
      {
        Serial.println("Unidentified Motion detected!"); 
        Serial.println("Sending Notification to the USER");
        state = HIGH;
           if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String("Yes");
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
        delay(10000);
      }
    } 
  
    else 
    {
        digitalWrite(led, HIGH); 
        delay(200);
        if (state == HIGH)
      {
          Serial.println("Motion stopped!");
          state = LOW;
      }
    }
   }
 }
