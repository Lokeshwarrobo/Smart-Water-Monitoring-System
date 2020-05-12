#include <WiFi.h>
#include <PubSubClient.h>

#define WIFISSID "Nokia" // Put your WifiSSID here
#define PASSWORD "asdfghjkl" // Put your wifi password here
#define TOKEN "#" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "Lokeshwar" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                           //it should be a random and unique ascii string and different from all other devices

/****************************************
 * Define Constants
 ****************************************/
#define VARIABLE_LABEL "flow1" // Assing the variable label
#define VARIABLE_LABEL1 "flow2"
#define VARIABLE_LABEL2 "flow3"
#define VARIABLE_LABEL3 "ph"
#define DEVICE_LABEL "lokeshwar" // Assig the device label

#define SENSOR 26 
#define SENSOR1 27
#define SENSOR2 25
float phvalue1;
long currentMillis = 0;
long previousMillis = 0;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
int interval = 1000;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;

long currentMillis2 = 0;
long previousMillis2 = 0;
unsigned int flowMilliLitres2;
unsigned long totalMilliLitres2;
volatile byte pulseCount2;
byte pulse1Sec2 = 0;

long currentMillis1 = 0;
long previousMillis1 = 0;
unsigned int flowMilliLitres1;
unsigned long totalMilliLitres1;
volatile byte pulseCount1;
byte pulse1Sec1 = 0;

float flowRate;
float flowRate1;
float flowRate2;
char mqttBroker[]  = "industrial.api.ubidots.com";
char payload[100];
char topic[150];
char payload1[100];
char topic1[150];
char payload2[100];
char topic2[150];
char payload3[100];
char topic3[150];
// Space to store values to send
char str_flowRate[10];
char str_flowRate1[10];
char str_flowRate2[10];
char str_sensor[10];
char phvalue[10];
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}
void IRAM_ATTR pulseCounter1()
{
  pulseCount1++;
}
void IRAM_ATTR pulseCounter2()
{
  pulseCount2++;
}

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  Serial.write(payload, length);
  Serial.println(topic);
}

void callback1(char* topic1, byte* payload1, unsigned int length) {
  char p1[length + 1];
  memcpy(p1, payload1, length);
  p1[length] = NULL;
  String message(p1);
  Serial.write(payload1, length);
  Serial.println(topic1);
}
void callback2(char* topic2, byte* payload2, unsigned int length) {
  char p2[length + 1];
  memcpy(p2, payload2, length);
  p2[length] = NULL;
  String message(p2);
  Serial.write(payload2, length);
  Serial.println(topic2);
}
void callback3(char* topic3, byte* payload3, unsigned int length) {
  char p3[length + 1];
  memcpy(p3, payload3, length);
  p3[length] = NULL;
  String message(p3);
  Serial.write(payload3, length);
  Serial.println(topic3);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFISSID, PASSWORD);
  // Assign the pin as INPUT 
 pinMode(SENSOR, INPUT_PULLUP);  
 pinMode(SENSOR1,INPUT_PULLUP);
 pinMode(SENSOR2,INPUT_PULLUP);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  pulseCount1 = 0;
  flowRate1 = 0.0;
  flowMilliLitres1 = 0;
  totalMilliLitres1 = 0;
  previousMillis1 = 0;
  
  pulseCount2 = 0;
  flowRate2 = 0.0;
  flowMilliLitres2 = 0;
  totalMilliLitres2 = 0;
  previousMillis2 = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR1), pulseCounter1, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2), pulseCounter2, FALLING);
  Serial.println();
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);  
  client.setCallback(callback1);
  client.setCallback(callback2);  
  client.setCallback(callback3); 
}
void f2(){
  currentMillis2 = millis();
  if (currentMillis2 - previousMillis2 > interval) {
    
    pulse1Sec2 = pulseCount2;
    pulseCount2 = 0;

    
    flowRate2 = ((1000.0 / (millis() - previousMillis2)) * pulse1Sec2) / calibrationFactor;
  
    previousMillis2 = millis();

   
    flowMilliLitres2 = (flowRate2 / 60) * 1000;

   
    totalMilliLitres2 += flowMilliLitres2;
 
  }
       
}
void fl(){
  currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 > interval) {
    
    pulse1Sec1 = pulseCount1;
    pulseCount1 = 0;

    
    flowRate1 = ((1000.0 / (millis() - previousMillis1)) * pulse1Sec1) / calibrationFactor;
  
    previousMillis1 = millis();

   
    flowMilliLitres1 = (flowRate1 / 60) * 1000;

   
    totalMilliLitres1 += flowMilliLitres1;
 
  }
       
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }

  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(topic1, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(topic2, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(topic3, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload1, "%s", "");
  sprintf(payload2, "%s", "");
  sprintf(payload3, "%s", "");
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label
  sprintf(payload1, "{\"%s\":", VARIABLE_LABEL1);
  sprintf(payload2, "{\"%s\":", VARIABLE_LABEL2);
  sprintf(payload3, "{\"%s\":", VARIABLE_LABEL3);
    currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;

    
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
  
    previousMillis = millis();

   
    flowMilliLitres = (flowRate / 60) * 1000;

   
    totalMilliLitres += flowMilliLitres;
 
    
    Serial.println(String("flow : ")+int(flowRate));  
    fl();
    Serial.println(String("flow1 : ")+int(flowRate1));  
    f2();
    Serial.println(String("flow2 : ")+int(flowRate2)); 
   
   String phdata = Serial.readStringUntil(':'); 
   if(phdata != ""){
   String ph = Serial.readStringUntil('$');
  phvalue1=ph.toFloat();  
   Serial.println(phvalue1);
   
   }
    
   int sensor = int(phvalue1);
  /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
  dtostrf(flowRate, 4, 2, str_flowRate);
  dtostrf(flowRate1, 4, 2, str_flowRate1);
  dtostrf(flowRate2, 4, 2, str_flowRate2);
   dtostrf(sensor, 4, 2, str_sensor);
  
  sprintf(payload, "%s {\"value\": %s}}", payload, str_flowRate); // Adds the value
   sprintf(payload1, "%s {\"value\": %s}}", payload1, str_flowRate1);
   sprintf(payload2, "%s {\"value\": %s}}", payload2, str_flowRate2);
    sprintf(payload3, "%s {\"value\": %s}}", payload3, str_sensor);
  Serial.println("Publishing data to Ubidots Cloud");
  client.publish(topic, payload);
  client.publish(topic1, payload1);
  client.publish(topic2, payload2);
  client.publish(topic3, payload3);
  client.loop();
  delay(1000);
}
}
  
