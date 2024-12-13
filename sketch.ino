#include <WiFi.h>
#include <HTTPClient.h>

// Definition of leds pins
#define green_led 2  // Pin to control green led
#define red_led 40   // Pin to control red led
#define yellow_led 9 // Pin to control yellow led

// LDR Sensor specs and usage variables
#define analogLDR 4      // Analog pin to read LDR
#define digitalLDR 5     // Digital pin to read LDR

// Global variables for general use purpose
const int buttonPin = 18;         // the number of the pushbutton pin
int buttonState = 0;              // variable for reading the pushbutton status
unsigned long previousMillis = 0; // will store last time LED was updated
int threshold = 600; // defining ilumination limiar, close to a computer screen light ~633

void setup()
{

    // Initial configuration of led pins as OUTPUTS of ESP32
    pinMode(yellow_led, OUTPUT);
    pinMode(green_led, OUTPUT);
    pinMode(red_led, OUTPUT);

    // Initialize the pushbutton pin as an input
    pinMode(buttonPin, INPUT);

    // Turn off all leds
    digitalWrite(yellow_led, LOW);
    digitalWrite(green_led, LOW);
    digitalWrite(red_led, LOW);

    // Setup of degub through Serial interface between ESP32 and PC, with 9600 baud rate
    Serial.begin(9600);

    // Connect to WiFi network with SSID Wokwi-GUEST
    WiFi.begin("Wokwi-GUEST", "");

    while (WiFi.status() == WL_CONNECT_FAILED)
    {
        delay(100);
        Serial.print(".");
    }
    // Considering it came out of the loop above, the ESP32 is now connected to WiFi
    Serial.println("Conectado ao WiFi com sucesso!");

    // Verifies button state
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH)
    {
        Serial.println("Botão pressionado!");
    }
    else
    {
        Serial.println("Botão não pressionado!");
    }

    if (WiFi.status() != WL_CONNECT_FAILED)
    { // If ESP32 is connected to WiFi
        HTTPClient http;

        String serverPath = "http://www.google.com.br/"; // HTTP request ENDPOINT

        http.begin(serverPath.c_str());

        int httpResponseCode = http.GET(); // HTTP request result

        if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }

    else
    {
        Serial.println("WiFi Disconnected");
    }
    previousMillis = millis();
}

void loop()
{
    int noLightChanger = 0;
    int LightChanger[3] = {3000, 2000, 5000};
    int LightIndex = 0;
    int ldrstatus = analogRead(analogLDR);

    // the darker it is, the higher the value (according to https://docs.wokwi.com/pt-BR/parts/wokwi-photoresistor-sensor#:~:text=Condi%C3%A7%C3%A3o,8)
    if (ldrstatus >= threshold)
    {
        Serial.println("its dark turn on led");
        Serial.println(ldrstatus);
        if (millis() - previousMillis > 1000 && noLightChanger == 0)
        {
            digitalWrite(yellow_led, HIGH);
            previousMillis = millis();
            noLightChanger = 1;
        }
        else if (millis() - previousMillis > 1000 && noLightChanger == 1)
        {
            digitalWrite(yellow_led, LOW);
            previousMillis = millis();
            noLightChanger = 0;
        }
    }
    else
    {
        Serial.println("its bright turn off light");
        Serial.println(ldrstatus);
        noLightChanger = 0;
        digitalWrite(yellow_led, LOW);

        if (LightIndex == 0)
        {
            digitalWrite(red_led, HIGH);
            delay(LightChanger[LightIndex]);
            digitalWrite(red_led, LOW);
            LightIndex++;
        }
        else if (LightIndex == 1)
        {
            digitalWrite(yellow_led, HIGH);
            delay(LightChanger[LightIndex]);
            digitalWrite(yellow_led, LOW);
            previousMillis = millis();
            LightIndex++;
        }
        else if (LightIndex == 2)
        {
            digitalWrite(green_led, HIGH);
            while (millis() - previousMillis < 5000)
            {
                
                digitalRead(buttonPin);
                if (buttonState == LOW)
                {
                    delay(1000);
                    LightIndex = 0;
                }
                
            }
            digitalWrite(green_led, LOW);
            LightIndex = 0;
        }
    }
}