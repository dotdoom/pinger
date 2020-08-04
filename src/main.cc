#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ping.h>
#include <stdint.h>
#include <SPI.h>

// #incldue <Esp.h> EspClass.deepSleep

#include "wifi_password.h"

void ICACHE_FLASH_ATTR _ping_recv_cb(void *v_opt, void *v_resp)
{
    ping_option *opt = reinterpret_cast<ping_option *>(v_opt);
    ping_resp *resp = reinterpret_cast<ping_resp *>(v_resp);

    Serial.printf("PING %s [%.4d]: ", IPAddress(opt->ip).toString().c_str(),
                  resp->seqno);
    if (resp->ping_err == -1)
    {
        digitalWrite(LED_BUILTIN_AUX, HIGH);
        Serial.println("ERROR");
    }
    else
    {
        digitalWrite(LED_BUILTIN_AUX, LOW);
        Serial.printf("%3d ms, %d bytes\n",
                      resp->resp_time, resp->bytes);
    }
}

void setup()
{
    pinMode(LED_BUILTIN_AUX, OUTPUT);
    digitalWrite(LED_BUILTIN_AUX, HIGH);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(115200);
    while (!Serial)
    {
        ;
    }

    WiFi.hostname("pinger");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);
    // Max Power.
    WiFi.setOutputPower(20.5);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.printf("Connecting (my MAC: %s)...\n",
                      WiFi.macAddress().c_str());
        delay(1000);
    }

    IPAddress host;
    if (!WiFi.hostByName("google.com", host))
    {
        Serial.println("Failed to resolve host");
        return;
    }

    digitalWrite(LED_BUILTIN, LOW);
    Serial.printf("Host resolved to: %s\n", host.toString().c_str());

    ping_option *options = new ping_option();
    memset(options, 0, sizeof(ping_option));
    options->count = UINT32_MAX;
    options->coarse_time = 2; // seconds
    options->ip = host;
    ping_regist_recv(options, _ping_recv_cb);
    if (!ping_start(options))
    {
        Serial.println("Failed to start ping");
    }
}

void loop()
{
    delay(1000);

    if (!WiFi.isConnected())
    {
        Serial.println("Disconnected");
        digitalWrite(LED_BUILTIN, HIGH);
    }

    digitalWrite(LED_BUILTIN, LOW);
}
