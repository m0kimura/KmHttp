//*******1*********2*********3*********4*********5*********6*********7*********8
/* KmHttp.cpp */
#include <Arduino.h>
#include "KmHttp.h"

WiFiClient Client;
WiFiServer Server(HTTP_PORT);

//##### wifiInit
//##### WiFiの初期化
KmHttp::KmHttp(int i3, int i4, String ssid, String psw){
  ssid.toCharArray(Ssid, 32); psw.toCharArray(Psw, 16); I3=i3; I4=i4;
  Humidity=0; Temperature=0; Lux=0;
  Debugger="192.168.1.5"; DebuggerPort=8088;
}

bool KmHttp::begin(){

  WiFi.begin(Ssid, Psw);

  if(I4>0){
    IPAddress ip(192, 168, I3, I4); IPAddress gateway(192, 168, I3, 1);
    IPAddress subnet(255, 255, 255, 0); WiFi.config(ip, gateway, subnet);
  }
  
  while (WiFi.status() != WL_CONNECTED){delay(500);}

  if (!MDNS.begin("esp8266")){return false;}
  
  Server.begin();
  MDNS.addService("http", "tcp", HTTP_PORT);
  return true;
}

//##### request
//##### TCPポートからの読み込み
bool KmHttp::request(){
  unsigned int i, j; String x, d;

  Client=Server.available(); if (!Client){return false;}
  while(Client.connected() && !Client.available()){delay(1);}

  d=Client.readStringUntil('\r');
  if(d==""){return false;} for(i=0; i<9; i++){Req[i]="";}
  int addr_start=d.indexOf(' ');
  int addr_end=d.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {return false;}
  d=d.substring(addr_start + 1, addr_end); Data=d;
  Client.flush();
  j=0; Req[j]=""; for(i=0; i<d.length(); i++){
    x=d.charAt(i);
    if(x=="/"){j++; if(j>8){break;} Req[j]="";}else{Req[j].concat(x);}
  }
  return true;
}

void KmHttp::send(int rc, String data){
  String out, x; out="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
//  sprintf(
//    x, "{\"cd\":%d,\"hum\":%d,\"tmp\": %d, \"data\":\"%s\"}", rc, Dht_hum, Dht_tmp, data
//  );
  out.concat("{\"cd\":"); out.concat(rc);
  out.concat(",\"hum\":"); out.concat(Humidity);
  out.concat(",\"tmp\":"); out.concat(Temperature);
  out.concat(",\"lux\":"); out.concat(Lux);
  out.concat(",\"person\":"); out.concat(Person);
  out.concat(",\"pressure\":"); out.concat(Pressure);
  out.concat(",\"data\":"); out.concat(data);
  out.concat("}"); Client.print(out);
}

//##### get
//##### GET リクエスト
bool KmHttp::get(String host, int port, String url){
  char h[32]; host.toCharArray(h, 32);
  KmHttp::encode(url); String u=Out;  
  if (!Client.connect(h, port)){return false;}
  Client.print(String("GET ") + u + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  Out=""; delay(10);
  while(Client.available()){
    String line=Client.readStringUntil('\r');
    Out.concat(line);
  }
  return true;
}

//##### post
//##### POST リクエスト
bool KmHttp::post(String host, int port, String url, String data){
  char h[32]; host.toCharArray(h, 32); KmHttp::encode(data); String d=Out;
  if (!Client.connect(h, port)){return false;}
  Client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n" + 
//               "Authorization: Basic " + unameenc + " \r\n" + 
               "Content-Length: "+String(d.length())+"\r\n" + 
               d+"\r\n");
  Out=""; delay(10);
  while(Client.available()){
    String line=Client.readStringUntil('\r');
    Out.concat(line);
  }
  return true;
}

bool KmHttp::debug(String msg){
  return KmHttp::get(Debugger, DebuggerPort, "/log/"+msg);
}

void KmHttp::encode(String data){
  const char *hex = "0123456789abcdef";
  char msg; int i; Out="";

  for(i=0; i<data.length(); i++){
    msg=data.charAt(i);
    if(('a'<=msg&&msg<='z')||('A'<=msg&&msg<='Z')||('0'<=msg&&msg<='9')||(msg=='/')){
      Out += msg;
    }else{
      Out += '%';
      Out += hex[msg >> 4];
      Out += hex[msg & 15];
    }
  }
}
