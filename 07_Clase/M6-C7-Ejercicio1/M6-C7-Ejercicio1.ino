#include <OneWire.h>
#include <DallasTemperature.h>
//#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED             D4
#define ONE_WIRE_PIN    D5 //D7
#define MSG_BUFFER_SIZE 250

//LiquidCrystal_I2C lcd( 0x27, 16, 2 );

OneWire oneWire( D5 );
DallasTemperature sensor( &oneWire );

// Actualizar parámetros
const char* ssid = "smart";
const char* password = "Global2022";
const char* mqtt_server = "industrial.api.ubidots.com";
const int mqtt_port = 1883;
String device_label = "temp_italo";
String mqtt_topic = "/v1.6/devices/"+device_label;

// cambiar por token Ubidots
const char* mqtt_user = "BBFF-wnE62JZRhHPKqZwvs2JjiRjr3WDP5Y";
const char* mqtt_password = "";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
int counter = 0;

// función que conecta a WiFi
void setup_wifi() {

  delay(10);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //lcd.setCursor( 0, 0 );
  //lcd.print("Connecting to ");
  //lcd.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //lcd.setCursor( 0, 1 );

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    //lcd.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //lcd.setCursor( 0, 0 );
  //lcd.print( "IP:" );
  //lcd.print(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "WemosClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // inicializar el módulo LCD
  //lcd.init();
  //lcd.setBacklight(1);

  // inicializar sensor Dallas
  sensor.begin();

  // inicializar Wemos
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);

  // inicializar y conectar a WiFi
  setup_wifi();
  
  // inicializar el generador de números aleatorios
  randomSeed(micros());
  
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  sensor.requestTemperatures();
  float temperature = sensor.getTempCByIndex( 0 );

  //lcd.setCursor( 0, 1 );
  //lcd.print( temperature );
  //lcd.print(" C");

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 15000) {
    lastMsg = now;
    ++counter;
    
    DynamicJsonDocument doc(1024);
    doc["temperature"] = temperature;
    
    char msg[MSG_BUFFER_SIZE];
    serializeJson(doc, msg);
    
    // publicar el mensaje en JSON serializado
    Serial.printf("Publish message to '%s': ", mqtt_topic.c_str());
    Serial.println(msg);
 
    client.publish(mqtt_topic.c_str(), msg);
    
  }
}
