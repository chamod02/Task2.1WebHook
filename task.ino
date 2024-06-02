#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include "secrets.h" // Include the secrets.h file

// DHT settings
#define DHTPIN 4 // GPIO pin where the DHT22 data pin is connected
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 60000; // Send readings every 60 seconds

WiFiClient client;

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Wait 60 seconds to update
  if ((millis() - lastTime) > timerDelay) {
    // Get temperature and humidity values
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    // Check if any reads failed and exit early (to try again)
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Print values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("%  Temperature: ");
    Serial.print(t);
    Serial.println("°C");

    // Write data to ThingSpeak
    ThingSpeak.setField(1, t);
    ThingSpeak.setField(2, h);

    // Write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    
    // Update last time
    lastTime = millis();
  }
}
