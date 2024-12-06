#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Stepper.h>

// Configuración de la red WiFi
const char* ssid = "D08D16";
const char* password = "82C26C2C40627";

// Configuración del servidor MQTT
const char* mqtt_server = "47fd6b6ea9ce48fbb951678b77c1bc22.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "espetito";
const char* mqtt_password = "PELUche123";

// Canales MQTT
const char* CONTROL_FOCOS_TOPIC = "control-focos";
const char* CONTROL_LED_TOPIC = "control-led";
const char* CONTROL_MOTOR_TOPIC = "control-motor";

// Pines de los dispositivos
const int focoPin1 = 12;      // Foco
const int ledPin = 14;        // LED
const int stepsPerRevolution = 2048; // Motor a pasos
Stepper myStepper(stepsPerRevolution, 33, 25, 26, 27); // Motor a pasos


// Certificado raíz 
static const char* root_ca PROGMEM = R"EOF( 
-----BEGIN CERTIFICATE----- 
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw 
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh 
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4 
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu 
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY 
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc 
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+ 
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U 
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW 
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH 
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC 
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv 
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn 
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn 
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw 
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI 
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV 
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq 
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL 
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ 
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK 
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5 
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur 
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC 
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc 
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq 
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA 
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d 
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc= 
-----END CERTIFICATE----- 
)EOF"; 
 
WiFiClientSecure espClient; 
PubSubClient client(espClient); 

// Configuración de WiFi
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi.");
}

// Callback para el mensaje recibido
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en el canal: ");
  Serial.println(topic);

  if (strcmp(topic, CONTROL_FOCOS_TOPIC) == 0) {
    int estado = payload[0] - '0'; // Interpretar como '0' (apagado) o '1' (encendido)
    digitalWrite(focoPin1, estado);
    Serial.print("Estado del foco: ");
    Serial.println(estado ? "Encendido" : "Apagado");
  } else if (strcmp(topic, CONTROL_LED_TOPIC) == 0) {
    int brillo = atoi((char*)payload); // Interpretar como valor de brillo (0-255)
    analogWrite(ledPin, brillo);
    Serial.print("Brillo del LED: ");
    Serial.println(brillo);
  } else if (strcmp(topic, CONTROL_MOTOR_TOPIC) == 0) {
    int steps = atoi((char*)payload); // Convertir el payload a entero
    if (steps >= 1 && steps <= 50) {
      myStepper.step(steps); // Girar a la derecha
      Serial.println("Motor girando a la derecha.");
    } else if (steps >= 51 && steps <= 100) {
      myStepper.step(-steps); // Girar a la izquierda
      Serial.println("Motor girando a la izquierda.");
    } else {
      Serial.println("Comando fuera de rango.");
    }
  }
}

// Reconexión a MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT.");
      client.subscribe(CONTROL_FOCOS_TOPIC);
      client.subscribe(CONTROL_LED_TOPIC);
      client.subscribe(CONTROL_MOTOR_TOPIC);
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  pinMode(focoPin1, OUTPUT);
  digitalWrite(focoPin1, LOW); // Apagar foco al inicio

  pinMode(ledPin, OUTPUT);
  myStepper.setSpeed(12); // Configurar velocidad del motor
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}