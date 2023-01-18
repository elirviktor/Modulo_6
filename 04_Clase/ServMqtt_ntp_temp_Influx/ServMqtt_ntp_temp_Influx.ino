#include <ESP8266WiFi.h>        // WiFi 1.2.7
#include <ESP8266HTTPClient.h>  // WiFi 1.2.7
#include <ArduinoJson.h>        //ArduinoJson 6.19.14
#include <PubSubClient.h>
#include <NTPClient.h>  //importamos la librería del cliente NTP
#include <WiFiUdp.h>    // importamos librería UDP para comunicar con NTP
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Oregon.h> 
              
//SENSOR DE HUMEDAD DE SUELO
const int analogo = A0;
int dryValue = 1023;
int wetValue = 0;
int friendlyDryValue = 0;
int friendlyWetValue = 100;

// GPIO donde está conectado el DS18B20
const int oneWireBus = 14; //14 es D5
OneWire oneWire(oneWireBus);
// Configure una instancia de oneWire para comunicarse con cualquier dispositivo OneWire
DallasTemperature sensors(&oneWire);
float temp = 0;

/*iniciamos el cliente udp para su uso con el server NTP*/
String hora="";
String currentDate="";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.shoa.cl",-10800,6000); 
//pool.ntp.org
//0.south-america.pool.ntp.org
//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
/************************************************************************************/
/************************************************************************************/
void NTP(String hora,String currentDate){
  
  timeClient.update(); //sincronizamos con el server NTP
  
  /*obtenemos la hora*/
  //Serial.println(timeClient.getFormattedTime());
  time_t epochTime = timeClient.getEpochTime();
  hora=timeClient.getFormattedTime();
 
  /*obtenemos la fecha*/
  String weekDay = weekDays[timeClient.getDay()];
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = months[currentMonth-1];
  int currentYear = ptm->tm_year+1900;
  currentDate = String(currentYear) + "/" + String(currentMonth) + "/" + String(monthDay);
  String timeStamp= currentDate + "T" + hora;
  
  /*Obtenemos la temperatura*/
  // Envíe el comando para que todos los dispositivos en el bus realicen una conversión de temperatura:
  sensors.requestTemperatures();
  // Obtener la temperatura en grados Celsius para el índice del dispositivo:
  temp = sensors.getTempCByIndex(0); // el índice 0 se refiere al primer dispositivo
  /*Humedad*/
  int sensorvalue = analogRead(analogo);
  float valReal = sensorvalue;
  float friendlyValue = map(sensorvalue, dryValue, wetValue,friendlyDryValue,friendlyWetValue);
  float humedadR = random(15,40);
  mqtt(timeStamp, temp, humedadR);
  /*mostramos por serial*/
  /*Serial.print(currentDate);
  Serial.print(";");
  Serial.println(hora);
  delay(1000);*/
}
/************************************************************************************/
/************************************************************************************/


 

/************************************************************************************/
/************************************************************************************/
void wifi(){
  /*Asignamos los contadores para los re-intentos*/
  byte cont = 0;
  byte max_intentos = 10;
  /*Variables de user y pass*/
  const char* ssid_ = "smart";
  const char* password_ = "Global2022";
  /*imprime puntos de intentos por serial*/
  Serial.println("");
  Serial.println("\n");
  /*Configuracion conexion WIFI*/
  WiFi.mode(WIFI_STA);                                             //Indicamos que tipo de conexion vamos a usar                               //Pasamos los datos de red de data.h
  WiFi.begin(ssid_, password_);                                    //Inciamos el modulo, pasamos las variables
  while (WiFi.status() != WL_CONNECTED and cont < max_intentos) {  //Cuenta hasta 10 si no se puede conectar lo cancela
    cont++;
    delay(500);
    Serial.println(".");
  }
  /*si conecta muestra los datos por el terminal*/
  if (cont < max_intentos) {  //Si se conectó
    Serial.println("********************************************");
    Serial.print("Conectado a la red WiFi: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Subnet: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("macAdress: ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
    Serial.println("*********************************************");
  /*Si no se conectó muestra mensaje por terminal*/
  } else {  
    Serial.println("------------------------------------");
    Serial.println("Error de conexion");
    Serial.println("------------------------------------");
  }
}
/************************************************************************************/
/************************************************************************************/
/*Configuraciones servidor MQTT*/
  const char* mqtt_server = "test.mosquitto.org";
  WiFiClient espClient;
  PubSubClient client(espClient);
  unsigned long lastMsg = 0;
  #define MSG_BUFFER_SIZE  (50)
  char msg[MSG_BUFFER_SIZE];
  int value = 0;
/*Servicio MQTT*/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Llegó el mensaje [");
  Serial.print(topic);
  Serial.print("] ");
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Encender el LED si se recibió un 1 como primer carácter
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   //Encienda el LED (tenga en cuenta que BAJO es el nivel de voltaje
     //pero en realidad el LED está encendido; esto es porque está activo bajo en el ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Apague el LED haciendo que el voltaje sea ALTO
  }
}
/************************************************************************************/
/************************************************************************************/
void reconnect() {
  // Bucle hasta que estemos reconectados
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    // Crear una identificación de cliente aleatoria
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // intento de conexión
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      // Una vez conectado, publicar un anuncio...
      client.publish("pucv/iot/m6/p3/g5", "comienzo de publicaciones");
      // ... y volver a suscribirte
      client.subscribe("inTopic5");
    } else {
      Serial.print("ha fallado, rc=");
      Serial.print(client.state());
      Serial.println(" inténtalo de nuevo en 5 segundoss");
      // Espere 5 segundos antes de volver a intentarlo
      delay(5000);
    }
  }
}
/************************************************************************************/

/************************************************************************************/
void mqtt(String timeStamp, float temp, float humedad){
  //char buffer[7];
  //String tempC = dtostrf(temp,6,2,buffer);
  //String humedad = dtostrf(humedad,6,2,buffer);
  //String datos = timeStamp + ";" + tempC;
  //char const* mensaje=datos.c_str();
  int tempc = int(temp);
  delay(1000);
  /*JSON*/
  StaticJsonDocument<256> doc;
  doc["sensor_id"] = "SensorItalo";
  doc["temperatura"] = tempc;
  //doc["timeStamp"] = timeStamp;
  //doc["value"] = tempC;
  doc["humedad"] = humedad;
  //doc["value"] = humedad;
  /*JsonArray data = doc.createNestedArray("data");
  data.add(tempC);*/
  char out[128];
  int b =serializeJson(doc, out);
  //Serial.print("bytes = ");
  //Serial.println(b,DEC);
  /*FIN JSON*/  
    
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    //snprintf (msg, MSG_BUFFER_SIZE, mensaje);
    //Serial.print("Publicar mensaje: ");
    Serial.println(out);
    //client.publish("pucv/iot/m6/p3/g5", msg);
    client.publish("pucv/iot/m6/p3/g3", out);
    client.publish("pucv/iot/m6/p3/g5", out);
  }
}
/************************************************************************************/
/************************************************************************************/
void setup() {
  Serial.begin(115200);
  sensors.begin();
  wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
/************************************************************************************/
/************************************************************************************/
void loop(){
  NTP(hora, currentDate);
}
