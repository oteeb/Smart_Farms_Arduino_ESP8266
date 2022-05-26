#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
#include <WiFiClientSecureAxTLS.h> // กรณีขึ้น Error ให้เอาบรรทัดนี้ออก
void Line_Notify(String message) ;

#define DHTPIN D2
#define DHTPIN1 D3
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
DHT dht1(DHTPIN1, DHTTYPE);

// Line config
#define LINE_TOKEN "ewonB8LTLgYcCb5haMwmqn1DqhWPikmgx4p3P225u9T"
String message = "%E0%B8%AD%E0%B8%B8%E0%B8%9B%E0%B8%81%E0%B8%A3%E0%B8%93%E0%B9%8C%E0%B8%95%E0%B8%A3%E0%B8%A7%E0%B8%88%E0%B8%AA%E0%B8%AD%E0%B8%9A%E0%B8%AD%E0%B8%B8%E0%B8%93%E0%B8%AB%E0%B8%A0%E0%B8%B9%E0%B8%A1%E0%B8%B4%20%E0%B9%81%E0%B8%A5%E0%B8%B0%E0%B8%84%E0%B8%A7%E0%B8%B2%E0%B8%A1%E0%B8%8A%E0%B8%B7%E0%B9%89%E0%B8%99%E0%B8%A1%E0%B8%B5%E0%B8%9B%E0%B8%B1%E0%B8%8D%E0%B8%AB%E0%B8%B2"; // ArduinoIDE เวอร์ชั่นใหม่ ๆ ใส่ภาษาไทยลงไปได้เลย

/* Put your SSID & Password */
const char* ssid = "oteeb";  // Enter SSID here
const char* password = "1111111111";  //Enter Password here
int buttonPin = D1;
int buttonState = 0;
int sensorPin = A0; // select the input pin for the potentiometer
int sensorValue = 0; // variable to store the value coming from the senso
int LED1 = D6; // led connected to D6
int LED2 = D7; // led connected to D7
int LED3 = D8; // led connected to D8

WiFiServer server(80);
//ESP8266WebServer server(80);
HTTPClient http;
ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(115200);
  dht.begin();
  dht1.begin();
  pinMode(buttonPin, INPUT);// กำหนดขาทำหน้าที่ให้ขา D1 เป็น INPUT รับค่าจากสวิตช์
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, 1);
  digitalWrite(LED2, 1);
  digitalWrite(LED3, 1);
  
  Serial.print("Connecting to the Newtork");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  IPAddress local_ip = {192,168,137,154};   //ตั้งค่า IP
  IPAddress gateway={192,168,137,1};    //ตั้งค่า IP Gateway
  IPAddress subnet={255,255,255,0};   //ตั้งค่า Subnet
  WiFi.config(local_ip,gateway,subnet);   //setค่าไปยังโมดูล
  
  Serial.println("WiFi connected");  
  server.begin();  // Starts the Server
  Serial.println("Server started");

  Serial.print("IP Address of network: "); // Prints IP address on Serial Monitor
  Serial.println(WiFi.localIP());
}

void automatic(){
  digitalWrite(LED1, 1);
  digitalWrite(LED2, 1);
  digitalWrite(LED3, 1);
  delay(1000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();

  if (isnan(h) || isnan(t) || isnan(h1) || isnan(t1)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Serial.println("Enter !");
    Line_Notify(message);
    delay(1000);
    return;
  }
  float sumh = (h+h1)/2;
  float sumt = (t+t1)/2;

  sensorValue = analogRead(sensorPin);
  int sensormh= map(sensorValue,0,1023,0,100);
  Serial.print("sensor = " );
  Serial.println(sensormh);

  if(sensormh <= 50){
    Serial.println("พัดลมทำงาน");
    digitalWrite(LED1, 1);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 1);
    delay(10000);
  }else if(sensormh >= 80){
    Serial.print("ปลั้มทำงาน " );
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 1);
    digitalWrite(LED3, 1);
    delay(10000);
  }else{
    Serial.print("ความชื้นปกติ");
  }

  if(sumt <= 25){
    Serial.println("หลอดไฟทำงาน");
    digitalWrite(LED1, 1);
    digitalWrite(LED2, 1);
    digitalWrite(LED3, 0);
    delay(10000);
  }else if(sumt >= 40){
    Serial.println("พัดลมระบายความร้อนทำงาน");
    digitalWrite(LED1, 1);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 1);
    delay(10000);
  }else{
    Serial.print("อุณหภูมิปกติ");
  }
}

void manual(){
   WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  Serial.println("Waiting for new client");
  while(!client.available())
  {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  int value = LOW;
  if(request.indexOf("/LED1=ON") != -1)
  {
    digitalWrite(LED1, 0); // Turn ON LED
    value = HIGH;
  }
  if(request.indexOf("/LED1=OFF") != -1)
  {
    digitalWrite(LED1, 1); // Turn OFF LED
    value = LOW;
  }

   if(request.indexOf("/LED2=ON") != -1)
  {
    digitalWrite(LED2, 0); // Turn ON LED
    value = HIGH;
  }
  if(request.indexOf("/LED2=OFF") != -1)
  {
    digitalWrite(LED2, 1); // Turn OFF LED
    value = LOW;
  }

   if(request.indexOf("/LED3=ON") != -1)
  {
    digitalWrite(LED3, 0); // Turn ON LED
    value = HIGH;
  }
  if(request.indexOf("/LED3=OFF") != -1)
  {
    digitalWrite(LED3, 1); // Turn OFF LED
    value = LOW;
  }

/*------------------HTML Page Creation---------------------*/

  client.println("HTTP/1.1 200 OK"); // standalone web server with an ESP8266
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print("<h3>API_Smart_Farm</h3>");
  client.print("Status : ");
 
  if(value == HIGH)
  {
    client.print("ON");
  }
  else
  {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("Water pump Control <br>");
  client.println("<a href=\"/LED1=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/LED1=OFF\"\"><button>OFF</button></a><br />");

  client.println("<br>");
  client.println("Fan Control <br>");
  client.println("<a href=\"/LED2=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/LED2=OFF\"\"><button>OFF</button></a><br />");

  client.println("<br>");
  client.println("lamp Control <br>");
  client.println("<a href=\"/LED3=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/LED3=OFF\"\"><button>OFF</button></a><br />");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}


void loop() {
  
  //server.handleClient();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();

  if (isnan(h) || isnan(t) || isnan(h1) || isnan(t1)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Serial.println("Enter !");
    Line_Notify(message);
    delay(1000);
    return;
  }
  float sumh = (h+h1)/2;
  float sumt = (t+t1)/2;
  Serial.print(F("ความชื้น : "));
  Serial.print(sumh);
  Serial.print(F(" %  อุณหภูมิ : "));
  Serial.print(sumt);
  Serial.println(F(" C "));
  
  String text1 = "text= " + String(sumh) + "&text1= " + String(sumt);
  Serial.println(text1);

    if ((WiFiMulti.run() == WL_CONNECTED)) {
    http.begin("http://100.125.10.104/API_Smart_Farm/api.php"); //HTTP
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(text1);
    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(5000);
  
  buttonState = digitalRead(buttonPin); // อ่านค่าสถานะขาD0
  Serial.println(buttonState);
  if (buttonState == 0) { //กำหนดเงื่อนไขถ้าตัวแปล buttonState เก็บ ค่า 1(HIGH) ให้ทำในปีกกา  
    automatic();
  
  }else { //ถ้าตัวแปล buttonState ไม่ได้เก็บ ค่า 1(HIGH) คือ ตัวแปล buttonState เก็บค่า 0(LOW) อยู่ ให้ทำปีกกาข้างล่าง
    manual();
  }
}

void Line_Notify(String message) {
  axTLS::WiFiClientSecure client; // กรณีขึ้น Error ให้ลบ axTLS:: ข้างหน้าทิ้ง

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Connection: close\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  client.print(req);
    
  delay(20);

  // Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}
