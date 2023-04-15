#include <Wire.h>
#include <DHT.h>

#define DHTTYPE DHT11
int pinDHT = 15;
DHT dht(pinDHT, DHTTYPE);

float temperature = 0;
float humidity = 0;
float Tmax = 0;
float Tmin = 100;
float Hmax = 0;
float Hmin = 100;

void setup() { 
  Serial.begin(115200);
  dht.begin();
}
void loop() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  Serial.println("Temperatura: " + String(temperature) + "°C");
  Serial.println("Humedad: " + String(humidity) + "%");
  Serial.println("-------------------------------------");
  

  if (temperature > Tmax){
      Tmax = temperature;     
    }
  if (temperature < Tmin){
      Tmin = temperature;     
    }
  if (humidity > Hmax){
      Hmax = humidity;     
    }
  if (humidity < Hmin){
      Hmin = humidity;     
    }
  Serial.println("Temperatura Máxima: " + String(Tmax) + "°C");
  Serial.println("Temperatura Mínima: " + String(Tmin) + "°C");
  Serial.println("--------------------------------------------");
  Serial.println("Humedad Máxima: " + String(Hmax) + "%");
  Serial.println("Humedad Mínima: " + String(Hmin) + "%");
  Serial.println("--------------------------------------------");
  delay(5000); 
}
