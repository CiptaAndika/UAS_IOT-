// Library yang diperlukan
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>  
// Koneksi
#define FIREBASE_HOST "iotteori-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "D0AHLI9M54UK96yQJbQpJxpYOHoAjHj6hnzqLeK7"
#define WIFI_SSID "Cipta"
#define WIFI_PASSWORD "12345678"  

// mendeklarasikan objek data dari FirebaseESP8266
FirebaseData firebaseData;

// Mendefinisikan pin dan tipe sensor
int trigPin = D5;  //Pin Trigger HC-SR04 pada NodeMCU
int echoPin = D6;  //Pin Echo HC-SR04 pada NodeMCU
#define pinBuzzer D1
WiFiClient  client; 
unsigned long ChannelNumber = 1989512;
const char * ReadAPIKey = "0M8VF0PGSVLKVGZX";
const int FieldNumber1 = 1;

#define ldrPin D2
const float gama = 0.7;
const float rl10 = 50;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  ThingSpeak.begin(client);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}

void loop() {
  // put your main code here, to run repeatedly:
    int A = ThingSpeak.readLongField(ChannelNumber, FieldNumber1, ReadAPIKey);
  String aktif = "Aktif";
  String tidak = "Tidak-Aktif";
  if(A==1){
      sensorUpdate();
      fuzzy();
      Firebase.setString(firebaseData, "/Jarak/Status-sensor", aktif);
    }else if(A==0){
      Firebase.setString(firebaseData, "/Jarak/Status-sensor", tidak);
      }
}

void sensorUpdate(){

  long d,r;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int nilaiLDR = analogRead(ldrPin);
  d = pulseIn(echoPin, HIGH) ;
  r = (d/2) / 29.1;

  Serial.print(r);
  Serial.println(" cm");
  
  if (Firebase.setFloat(firebaseData, "/Jarak/cm", r)){
      Serial.println("Jarak disimpan");
      Firebase.setFloat(firebaseData, "/Jarak/ldr", nilaiLDR);
    } else{
      Serial.println("jarak tidak tersimpan");
      Serial.println("Karena: " + firebaseData.errorReason());
    } 
    

}

void fuzzy (){
    long d,r;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  String banjir = "banjir";
  String normal = "normal";
  d = pulseIn(echoPin, HIGH) ;
  r = (d/2) / 29.1;
  int nilaiLDR = analogRead(ldrPin);

  if(r<=10 && r>=0 && nilaiLDR<=0){
    Firebase.setString(firebaseData, "/Jarak/status", banjir);
    analogWrite(pinBuzzer, 200);
    }else if(r<=10 && r>=0 && nilaiLDR>=1000){
      Firebase.setString(firebaseData, "/Jarak/status", banjir);
      analogWrite(pinBuzzer, 200);
      }else if(r>=11 && r<=25 && nilaiLDR<=0){
        Firebase.setString(firebaseData, "/Jarak/status", normal);
        }else if(r>=11 && r<=25 && nilaiLDR>=1000){
          Firebase.setString(firebaseData, "/Jarak/status", banjir);
          analogWrite(pinBuzzer, 200);
          }else if(r>=26 && r<=50 && nilaiLDR<=0){
            Firebase.setString(firebaseData, "/Jarak/status", normal);
            }else if (r>=26 && r<=50 && nilaiLDR>=1000){
              Firebase.setString(firebaseData, "/Jarak/status", normal);
              }else{
                Firebase.setString(firebaseData, "/Jarak/status", normal);
                   analogWrite(pinBuzzer, 0);
        }
  }
