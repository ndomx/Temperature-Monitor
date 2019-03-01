#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define LED_WIFI LED_BUILTIN
#define INPUT_ADC A0

#define ssid "YOUR-SSID"
#define password "YOUR-SECRET-PASSWORD"

#define SERVER_URL "http://192.168.1.105:8000/"

void setup_wifi(void);
void read_data(void);
void upload_data(void);

float temperature;
HTTPClient http;

void setup()
{
    pinMode(LED_WIFI, OUTPUT);

    Serial.begin(115200);
    setup_wifi();
}

void loop()
{
    read_data();
    upload_data();

    delay(5e3);
}

void setup_wifi()
{
    delay(10);
    
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    bool wifi_state = LOW;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");

        digitalWrite(LED_WIFI, wifi_state);
        wifi_state ^= HIGH;
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Show that device is connected
    digitalWrite(LED_WIFI, HIGH);
}

void read_data()
{
    volatile float Vout = analogRead(INPUT_ADC) * 3.3f / 1024;
    /*
     * TMP35 Temperature curve:
     * Vout = 0.01 [V/Celsius] * Temp
     * ~~
     * Temp = 100 * Vout
     */

    temperature = 100 * Vout;
}

void upload_data()
{
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST("\{\"data\": " + String(temperature) + "\}");

    if (httpCode > 0)
    {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // if (httpCode == HTTP_CODE_OK)
        // {
        //     String payload = http.getString();
        //     Serial.println(payload);
        // }

        String payload = http.getString();
        Serial.println(payload);
    }
    else
    {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}
