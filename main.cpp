/*------------------------------------------------------------------------------
DVXLAB 
RX
  {
    "message":"led"
    "led1":1,
    "led2":0,
    "led3":1,
    "led4":1
  }

TX
  {
   "temperature":50,
   "humidity":75,
  }
Linux users 
curl -F "file=@$PWD/index.html" 192.168.1.XX/upload
                    bootstrap.min.css
                    bootstrap.min.js
                    css-circular-prog-bar.css
                    jquery-3.2.1.min.js
                    material-design-iconic-font.min.css
                    theme.css

Windows users 
curl -F "file=@%cd%/index.html" 192.168.1.XX/upload

curl -X "DELETE" 192.168.1.XX/format
------------------------------------------------------------------------------*/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <Adafruit_MotorShield.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define LED1 D5
#define LED2 D6
#define LED3 D7
#define LED4 D8

#define DHTPIN D1     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(80);
char* ssid = "YOUR_SSID";
char* password = "YOUR_PASSWORD";

File fsUploadFile;
WebSocketsServer webSocket = WebSocketsServer(81);

void handleIndexFile()
{
  File file = SPIFFS.open("/index.html","r");
  server.streamFile(file, "text/html");
  file.close();
}

void handleFileUpload()
{
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START)
  {
    String filename = upload.filename;
    if(!filename.startsWith("/"))
       filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
  } 
  else if(upload.status == UPLOAD_FILE_WRITE)
  {
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } 
  else if(upload.status == UPLOAD_FILE_END)
  {
    if(fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleFileList()
{
  String path = "/";
  // Assuming there are no subdirectories
  Dir dir = SPIFFS.openDir(path);
  String output = "[";
  while(dir.next())
  {
    File entry = dir.openFile("r");
    // Separate by comma if there are multiple files
    if(output != "[")
      output += ",";
    output += String(entry.name()).substring(1);
    entry.close();
  }
  output += "]";
  server.send(200, "text/plain", output);
}

void handleFileDelete(){
  if(server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  Serial.println("handleFileDelete: " + path);
  if(path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if(!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileDeleteAll(){
  String path = "/";
  // Assuming there are no subdirectories
  Dir dir = SPIFFS.openDir(path);
  while(dir.next())
  {
    File entry = dir.openFile("r");
    Serial.print("\nRemoving: "); Serial.println(entry.name());
    if(!SPIFFS.exists(String(entry.name()).substring(1)))
    {
        if(SPIFFS.remove(String(entry.name())))
           Serial.print("\nDelete :Success"); 
        else
            Serial.print("\nDelete :Failed"); 
    }
    //entry.close();
  }
  server.send(200, "text/plain", "");
  path = String();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if(type == WStype_TEXT)
  {
    String payload_str = String((char*) payload);
    Serial.println(payload_str);
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload_str);

    String msg = doc["message"];
    if(msg == "led") 
    {
      int Val = doc["led1"];
      analogWrite(LED1,Val);

      Val = doc["led2"];
      if (Val==1)
          digitalWrite(LED2, HIGH);
      else 
          digitalWrite(LED2, LOW);

      Val = doc["led3"];
      if (Val==1)
          digitalWrite(LED3, HIGH);
      else 
          digitalWrite(LED3, LOW);

      Val = doc["led4"];
      if (Val==1)
          digitalWrite(LED4, HIGH);
      else 
          digitalWrite(LED4, LOW);
      Serial.println("Setting LEDs");
    } 
  }
}


void setup()
{  
  bool res = SPIFFS.begin();
  pinMode(DHTPIN, INPUT);
  dht.begin();
  analogWrite(LED1,0);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);

  WiFi.begin(ssid,password);
  Serial.begin(9600);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (res) 
  {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    Serial.print("fs_info.totalBytes = ");
    Serial.println(fs_info.totalBytes);
    Serial.print("fs_info.usedBytes = ");
    Serial.println(fs_info.usedBytes);
    Serial.print("fs_info.blockSize = ");
    Serial.println(fs_info.blockSize);
    Serial.print("fs_info.pageSize = ");
    Serial.println(fs_info.pageSize);
    Serial.print("fs_info.maxOpenFiles = ");
    Serial.println(fs_info.maxOpenFiles);
    Serial.print("fs_info.maxPathLength = ");
    Serial.println(fs_info.maxPathLength);
  }
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) 
  {
    Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.size());
  }

  server.serveStatic("/bootstrap.min.css", SPIFFS, "/bootstrap.min.css");
  server.serveStatic("/bootstrap.min.js", SPIFFS, "/bootstrap.min.js");
  server.serveStatic("/css-circular-prog-bar.css", SPIFFS, "/css-circular-prog-bar.css");
  server.serveStatic("/jquery-3.2.1.min.js", SPIFFS, "/jquery-3.2.1.min.js");
  server.serveStatic("/material-design-iconic-font.min.css", SPIFFS, "/material-design-iconic-font.min.css");
  server.serveStatic("/theme.css", SPIFFS, "/theme.css");

  server.serveStatic("/css", SPIFFS, "/css");
  server.serveStatic("/img", SPIFFS, "/img");
  server.serveStatic("/", SPIFFS, "/index.html");

  // list available files
  server.on("/list", HTTP_GET, handleFileList);
  // handle file upload
  server.on("/upload", HTTP_POST, [](){
    server.send(200, "text/plain", "{\"success\":1}");
  }, handleFileUpload);
  server.on("/edit", HTTP_DELETE, handleFileDelete); 
  server.on("/format", HTTP_DELETE, handleFileDeleteAll);//handle with care :)
  server.begin();
  webSocket.begin();
  // function to be called whenever there's a websocket event
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  webSocket.loop();
  server.handleClient();
  
  //2000ms task for temp/hum update
  static unsigned long l = 0;
  unsigned long t = millis();
  if((t - l) > 2000)
  {
    l = t;
    float hVal = dht.readHumidity();
    float tVal = dht.readTemperature();
    float fVal = dht.readTemperature(true);
    Serial.print(F("Humidity: \n"));
    Serial.print(hVal);
    Serial.print(F("%  Temperature: \n"));
    Serial.print(tVal);
    String msgTx="";
    msgTx= msgTx+= "{\"temperature\":";
    msgTx= msgTx+ String(tVal) ;
    msgTx= msgTx+ ",\"humidity\":";
    msgTx= msgTx+ String(hVal);
    msgTx= msgTx+"}";
    webSocket.broadcastTXT(msgTx);
  }

}




