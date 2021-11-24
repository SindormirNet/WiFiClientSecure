//#include <WiFi.h> //Librería para uso de MQTT sin SSL
#include "src/dependencies/WiFiClientSecure/WiFiClientSecure.h" //Librería para uso de MQTTS !!Usa versión antigua de WiFiClientSecure -<WiFiClientSecure.h> no funciona!!
#include <PubSubClient.h>

const char* ssid = ""; //SSID WiFi
const char* password = ""; //Contraseña SSID WiFi
const char* mqtt_server = ""; //Nombre o IP del broker MQTT
#define MQTT_PORT 8883 //Puerto de conexión para broker MQTT
#define MQTT_USER "sindormir.net" //Usuario de MQTT
#define MQTT_PASSWORD "espaciomiscela" //Contraseña para usuario MQTT
#define MQTT_SERIAL_PUBLISH_CH "test" //Topic de publicación
#define MQTT_SERIAL_RECEIVER_CH "test" //Topic de suscripción

//ca.crt
const char CA_PUBLIC_CERT[] = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

//client.crt
const char CLIENT_CERT[] = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

//client.key
const char PRIVATE_KEY[] = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"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\n" \
"-----END RSA PRIVATE KEY-----\n";

//WiFiClient wifiClient; //Usar para MQTT sin SSL
WiFiClientSecure wifiClient; //Usar para MQTTS
PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
}

void reconnect() {
  wifiClient.setCACert(CA_PUBLIC_CERT); //Usar sólo para MQTTS
  wifiClient.setCertificate(CLIENT_CERT); //Usar sólo para MQTTS
  wifiClient.setPrivateKey(PRIVATE_KEY); //Usar sólo para MQTTS

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    //if (client.connect(clientId.c_str())) { //Pa autentificación anónima
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) { //Para autentificación con usuario y contraseña
      Serial.println("connected");
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(callback);
  reconnect();
}

void loop() {
   client.loop();
   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     if (!client.connected()) {
        reconnect();
     }
     client.publish(MQTT_SERIAL_PUBLISH_CH, mun);
   }
 }
