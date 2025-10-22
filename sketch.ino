

#include <ArduinoJson.h>    // Biblioteca para el formato JSON
#include <WiFi.h>    //Biblioteca para el manejo de WIFI del esp8266
#include <PubSubClient.h>   //Biblioteca para el manejo del protocolo MQTT
#include <ESP32Servo.h> 

StaticJsonDocument<200> doc;
StaticJsonDocument<200> doc2;
const char* ssid = "Wokwi-GUEST";  //Escribe la identificación de tu red
const char* password = ""; //Escribe el password de tu red
const char* mqtt_server = "157.230.214.127"; // Escribe la dirección de tu PC asignada por tu red

WiFiClient espClient;               // Crea una instancia tipo WifiCliente llamado espClient
PubSubClient client(espClient);     //Gestiona la Biblioteca de publicación-subscripción para el cliente creado

Servo myservo;
long lastMsg = 0;                   // Declaración de variable para conteo de tiempo de transmisión de mensaje
char msg[50];                       // Decalaración de buffer para guardar el mensaje recibido
String sr2 = "";                    //Declaración de la variable sr2 para guardar la cadena de caracteres recibidos
String inputString = "";            //Declaración de variable auxiliares para datos recibidos
static char buffer2[6];             //Declaración de variables para almacenar caracteres
char rec[50];                       //Declaración de varible auxiliar para la recepción de mensajes

void setup() {
  pinMode(2,OUTPUT);
  myservo.setPeriodHertz(50);// Standard 50hz servo
  myservo.attach(13, 500, 2400); 
  Serial.begin(115200);                    //Inicializa la comunicación serial a una velocidad de 115200
  setup_wifi();                            //Inicializa la Biblioteca WIFI
  client.setServer(mqtt_server, 1883);     //Inicializa el servidor con las dirección descrita en el encabezado y el puerto que usa
  client.setCallback(callback);            //Inicializa la función que se activa cuando llegan mensajes

}

void setup_wifi() {

  delay(10);                              // espera 10 milisegundos
  Serial.println();                       // Imprime un espacio de linea
  Serial.print("Conectando a ");          // Imprime la leyenda "Conectando a"
  Serial.println(ssid);                   // Imprime la identificación de la red a al que se conecta
  WiFi.begin(ssid, password);             // Realiza la conexión a la red configurada usando el password configurado en el encabezado

  while (WiFi.status() != WL_CONNECTED) { // Espera mientras se establece la conexión
    delay(500);                           // Espera 500 milisegundos
    Serial.print(".");                    // Imprime en la terminal "." hasta que se logre la conexión
  }

  Serial.println("");                     //Imprime en la terminal un espacio
  Serial.println("WiFi connecteda");       //Imprime en la terminal un espacio
  Serial.println("dirección IP: ");       //Imprime en la terminal "dirección IP: "
  Serial.println(WiFi.localIP());         //Imprime en la terminal "dirección IP asignada al microcontrolador
}


void reconnect() {                                   //función para la reconexión

  while (!client.connected()) {                      //Espera hasta que se establezca la reconexión
    Serial.print("Esperando Conexión MQTT...");

    if (client.connect("BOTON_ESPCliente")) {                //Se conecta como cliente con el nombre ESP8266Cliente
      Serial.println("connectada");                        //Imprime en la terminal conectado con el cliente
      //client.subscribe("M2MQTT_Unity/test"); 
      client.subscribe("cmqtt_a");  
      client.subscribe("cmqtt_s");           
    } else {                                               // si no logra conectarse
      Serial.print("falla, rc=");                            //Imprime en la terminal "falla, rc="
      Serial.print(client.state());                          //Imprime en la terminal el tipo de falla de conexión (un múmero)
      Serial.println("Intento de nuevo en 5 segundos");      //Imprime en la terminal "Intento de nuevo en 5 segundos"
      delay(5000);                                           //Espera 5 segundos
    }
  }
}


void loop() {

  if (!client.connected()) {                                  
    reconnect();                                                 
  }
  client.loop();                                             
                                                  

}

void callback(char* topic, byte* payload, unsigned int length) {


  sr2 = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    rec[i] = payload[i];
    sr2 += rec[i];
    inputString = sr2;

  }

  char msg2[inputString.length()];
  Serial.println(msg2);
  inputString.toCharArray(msg2, inputString.length() + 1);
  StaticJsonDocument<200> doc2;
  
  DeserializationError error = deserializeJson(doc2, msg2);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  String  value = doc2["Act1"];
  //String  method_ = doc2["method"];
  float  value_servo = doc2["Analog"];
 
    if (value.equals("OFF"))
    {
      digitalWrite(2, LOW);
    }
    if (value.equals("ON")) {
      
      digitalWrite(2, HIGH);  


    }

   if (value_servo!=0)
     {int val = map(value_servo, 0, 100, 0, 180);
      myservo.write(val);   
     }
  }

