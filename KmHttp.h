//*******1*********2*********3*********4*********5*********6*********7*********8
/* KmHttp.h */
#ifndef HTTP_PORT
#define HTTP_PORT 80
#endif

#ifndef KMHTTP_H
#define KMHTTP_H

#include "Arduino.h"
#include "WiFiClient.h"
#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"

class KmHttp{
  public:
    KmHttp(int i3, int i4, String ssid, String psw);
    bool begin();
    bool request();
    void send(int rc, int num, String data[]);
    bool get(String host, int port, String url);
    bool post(String host, int port, String url, String dara);
    bool debug(String msg);
    void encode(String data);

    String Buf[9];
    String Data;
    int Humidity;
    int Temperature;
    int Lux;
    int Person;
    int Pressure;
    String Out;
    int DebuggerPort;
    String Debugger;

  private:
    int I3;
    int I4;
    char Ssid[32];
    char Psw[16];
};


#endif
