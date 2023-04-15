#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h>
#include <ESP_Mail_Client.h>
#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

String messageStatic = "Invernadero";
 
#define DHTTYPE DHT11
int pinDHT = 15;
DHT dht(pinDHT, DHTTYPE);

// Replace the next variables with your SSID/Password combination
const char* ssid = "JuanCabasTorres";
const char* password = "mamatoco99"; 
char* mqtt_server = "44.201.121.69";

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* Datos de acceso a cuenta. */
#define AUTHOR_EMAIL "nataprime22@gmail.com"
#define AUTHOR_PASSWORD "ijekgfxtzgeazrdx"

/* Correo electrÃ³nico del recipiente*/
#define RECIPIENT_EMAIL "nataprime22@gmail.com"

/* Objeto SMTP para enviar el correo electrÃ³nico */
SMTPSession smtp;

/* Busca el estado del correo enviado. */
void smtpCallback(SMTP_Status status);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int relay = 13;
int relay2 = 26;
//Decaramos el variable que almacena el pin a conectar el DHT11
float temperature = 0;
float humidity = 0;
float tmax = 0;
float tmin = 100;
float hmax = 0;
float hmin = 100;
int cont1 = 0;
int cont2 = 0;
int cont3 = 0;



// LED Pin
const int ledPin = 4; 

void setup() {
  pinMode(relay, OUTPUT);
  pinMode(relay2, OUTPUT);
  
  Serial.begin(115200);
  dht.begin();
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print static message
  lcd.print(messageStatic);
  // print scrolling message
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000) {
    scrollText(1,"Temperatura " + String(temperature) + "C" + "  Humedad " + String(humidity) + "%" , 250, lcdColumns);
    lastMsg = now;
    
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    
    Serial.println("Temperatura: " + String(temperature) + "°C");
    Serial.println("Humedad: " + String(humidity) + "%");
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    String Temperature= String(tempString);
    String temp="Clima Temperatura=";
    String tempT=temp+Temperature;
    char temptotal[50];
    tempT.toCharArray(temptotal, 25);
    client.publish("reporte", temptotal);

    // Convert the value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    String Humidity= String(humString);
    String hum="Clima Humedad=";
    String humT=hum+Humidity;
    char humtotal[50];
    humT.toCharArray(humtotal, 20);
    client.publish("reporte", humtotal);
    Serial.println("---");
   

    if (temperature > tmax){
      tmax = temperature;     
    }
    if (temperature < tmin){
      tmin = temperature;     
    }
    if (humidity > hmax){
      hmax = humidity;     
    }
    if (humidity < hmin){
      hmin = humidity;     
    }

    //PUBLICAR VALORES MAXIMOS Y MINIMOS DE HUMEDAD
    char HmaxString[8];
    dtostrf(hmax, 1, 2, HmaxString);
    String Hmax= String(HmaxString);
    String hummax="Clima Hmax=";
    String hummaxT=hummax+Hmax;
    char hummaxtotal[50];
    hummaxT.toCharArray(hummaxtotal, 20);
    client.publish("reporte", hummaxtotal);
    Serial.println("---");

    char HminString[8];
    dtostrf(hmin, 1, 2, HminString);
    String Hmin= String(HminString);
    String hummin="Clima Hmin=";
    String humminT=hummin+Hmin;
    char hummintotal[50];
    humminT.toCharArray(hummintotal, 20);
    client.publish("reporte", hummintotal);
    Serial.println("---");
    //////////////////////////////////////////////////////

    //PUBLICAR VALORES MAXIMOS Y MINIMOS DE TEMPERATURA
    char TmaxString[8];
    dtostrf(tmax, 1, 2, TmaxString);
    String Tmax= String(TmaxString);
    String temmax="Clima Tmax=";
    String temmaxT=temmax+Tmax;
    char temmaxtotal[50];
    temmaxT.toCharArray(temmaxtotal, 20);
    client.publish("reporte", temmaxtotal);
    Serial.println("---");

    char TminString[8];
    dtostrf(tmin, 1, 2, TminString);
    String Tmin= String(TminString);
    String temmin="Clima Tmin=";
    String temminT=temmin+Tmin;
    char temmintotal[50];
    temminT.toCharArray(temmintotal, 20);
    client.publish("reporte", temmintotal);
    Serial.println("---");
    delay(1000);
    //////////////////////////////////////////////////
    

    if (temperature >= 32 or humidity >= 75){
    digitalWrite(relay, HIGH); // envia señal alta al relay
    
  }

  else{
    digitalWrite(relay, LOW);  // envia señal baja al relay
    
  }
  if (humidity <= 80 ){
    digitalWrite(relay2, HIGH); // envia señal alta al relay
    
  }

  else{
    digitalWrite(relay2, LOW);  // envia señal baja al relay
    
  }
  if(humidity < 65){
    cont1 = cont1 + 1;
    if(cont1 >= 1 and cont1 <= 3){
      /** Enable the debug via Serial port
      * none debug or 0
      * basic debug or 1
      */
      smtp.debug(1);

      /* Solicita resultados de envÃ­o */
      smtp.callback(smtpCallback);

      /* Configura datos de sesiÃ³n */
      ESP_Mail_Session session;

      /* Configura la sesiÃ³n */
      session.server.host_name = SMTP_HOST;
      session.server.port = SMTP_PORT;
      session.login.email = AUTHOR_EMAIL;
      session.login.password = AUTHOR_PASSWORD;
      session.login.user_domain = "";

      /* Declara la clase del mensaje */
      SMTP_Message message;

      /* Configura cabecera del mensaje */
      message.sender.name = "INVERNADERO DE HONGOS";
      message.sender.email = AUTHOR_EMAIL;
      message.subject = "ALARMA HUMEDAD";
      message.addRecipient("Ingeniero", RECIPIENT_EMAIL);
  
      //Manda texto
      String textMsg = "ALERTA!! ---- LA HUMEDAD DEL INVERNADERO ESTA POR DEBAJO DEL 60%!!!";
      message.text.content = textMsg.c_str();
      message.text.charSet = "us-ascii";
      message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    
      message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
      message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

      /* Configura cabecera personalizada */
      //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

      /* Conecta al servidor */
      if (!smtp.connect(&session)){
        return;
      }
      /* Manda correo y cierra sesiÃ³n */
      if (!MailClient.sendMail(&smtp, &message)){
        Serial.println("Error sending Email, " + smtp.errorReason());
      }
    } 
  }
  else{
      cont1 = 0;
       }

  if(isnan(humidity)){
    cont2 = cont2 + 1;
    if(cont2 >= 1 and cont2 <= 3){
      /** Enable the debug via Serial port
      * none debug or 0
      * basic debug or 1
      */
      smtp.debug(1);

      /* Solicita resultados de envÃ­o */
      smtp.callback(smtpCallback);

      /* Configura datos de sesiÃ³n */
      ESP_Mail_Session session;

      /* Configura la sesiÃ³n */
      session.server.host_name = SMTP_HOST;
      session.server.port = SMTP_PORT;
      session.login.email = AUTHOR_EMAIL;
      session.login.password = AUTHOR_PASSWORD;
      session.login.user_domain = "";

      /* Declara la clase del mensaje */
      SMTP_Message message;

      /* Configura cabecera del mensaje */
      message.sender.name = "INVERNADERO HONGOS";
      message.sender.email = AUTHOR_EMAIL;
      message.subject = "ALARMA SENSOR";
      message.addRecipient("Ingeniero", RECIPIENT_EMAIL);
  
      //Manda texto
      String textMsg = "ALERTA!!! ------ EL SENSOR NO ESTA FUNCIONANDO DE MANERA CORRECTA!!!!";
      message.text.content = textMsg.c_str();
      message.text.charSet = "us-ascii";
      message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    
      message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
      message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

      /* Configura cabecera personalizada */
      //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

      /* Conecta al servidor */
      if (!smtp.connect(&session)){
        return;
      }
      /* Manda correo y cierra sesiÃ³n */
      if (!MailClient.sendMail(&smtp, &message)){
        Serial.println("Error sending Email, " + smtp.errorReason());
      }
    } 
  }
  else{
      cont2 = 0;
       }

  if(temperature >= 32){
    cont3 = cont3 + 1;
    if(cont3 >= 1 and cont3 <= 3){
      /** Enable the debug via Serial port
      * none debug or 0
      * basic debug or 1
      */
      smtp.debug(1);

      /* Solicita resultados de envÃ­o */
      smtp.callback(smtpCallback);

      /* Configura datos de sesiÃ³n */
      ESP_Mail_Session session;

      /* Configura la sesiÃ³n */
      session.server.host_name = SMTP_HOST;
      session.server.port = SMTP_PORT;
      session.login.email = AUTHOR_EMAIL;
      session.login.password = AUTHOR_PASSWORD;
      session.login.user_domain = "";

      /* Declara la clase del mensaje */
      SMTP_Message message;

      /* Configura cabecera del mensaje */
      message.sender.name = "INVERNADERO HONGOS";
      message.sender.email = AUTHOR_EMAIL;
      message.subject = "ALARMA TEMPERATURA";
      message.addRecipient("Ingeniero", RECIPIENT_EMAIL);
  
      //Manda texto
      String textMsg = "ALERTA!!! ----- LA TEMPERATURA DEL INVERNADERO ESTA POR ENCIMA DE LOS 32 GRADOS!!!";
      message.text.content = textMsg.c_str();
      message.text.charSet = "us-ascii";
      message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    
      message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
      message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

      /* Configura cabecera personalizada */
      //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

      /* Conecta al servidor */
      if (!smtp.connect(&session)){
        return;
      }
      /* Manda correo y cierra sesiÃ³n */
      if (!MailClient.sendMail(&smtp, &message)){
        Serial.println("Error sending Email, " + smtp.errorReason());
      }
    } 
  }
  else{
      cont3 = 0;
       }               
  }
}
/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}
void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

/*if (bme.readTemperature() >= 32 ){
    digitalWrite(relay, HIGH); // envia señal alta al relay
    
  }

  else{
    digitalWrite(relay, LOW);  // envia señal baja al relay
    
  }

}
*/
