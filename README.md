# ESP8266 WebServer Websocket DH11

Use Arduino or PlatformIO IDE 

1.Set the Wifi Settings 
2.Add the sensor libray
3.Compile & download the firmware
3.Upload the HTML files using the below commands




curl -F "file=@$PWD/index.html" 192.168.1.XX/upload
                    bootstrap.min.css
                    bootstrap.min.js
                    css-circular-prog-bar.css
                    jquery-3.2.1.min.js
                    material-design-iconic-font.min.css
                    theme.css
                    
                    
# Here is the message format used for Websocket communication
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
  
  
  
# Screenshot

![Image Main UI](https://github.com/dvxlab/ESP8266_WebServer_DH11/blob/main/screenshot/screenshot.png)
