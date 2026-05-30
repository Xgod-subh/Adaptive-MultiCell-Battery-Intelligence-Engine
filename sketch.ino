#define BLYNK_TEMPLATE_ID "TMPL3H8YcLZYX"
#define BLYNK_TEMPLATE_NAME "Adaptive MultiCell Battery Intelligence Engine"
#define BLYNK_AUTH_TOKEN "TCn5ngMGoNvS8ud6aBVCNJwNhPkVw6ci"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

BlynkTimer timer;

#define CELL1_PIN 34
#define CELL2_PIN 35
#define CELL3_PIN 32
#define CELL4_PIN 33

void sendBatteryData() {

  float cell1 = analogRead(CELL1_PIN) * 4.2 / 4095.0;
  float cell2 = analogRead(CELL2_PIN) * 4.2 / 4095.0;
  float cell3 = analogRead(CELL3_PIN) * 4.2 / 4095.0;
  float cell4 = analogRead(CELL4_PIN) * 4.2 / 4095.0;

  float cells[4] = {cell1, cell2, cell3, cell4};

  float packVoltage = cell1 + cell2 + cell3 + cell4;
  float avgVoltage = packVoltage / 4.0;

  float minCell = cells[0];
  float maxCell = cells[0];

  int weakIndex = 1;
  int strongIndex = 1;

  for (int i = 1; i < 4; i++) {
    if (cells[i] < minCell) {
      minCell = cells[i];
      weakIndex = i + 1;
    }

    if (cells[i] > maxCell) {
      maxCell = cells[i];
      strongIndex = i + 1;
    }
  }

  float imbalance = ((maxCell - minCell) / maxCell) * 100.0;

  String status;

  if (packVoltage < 8.0) {
    status = "Pack Failure";
  }
  else if (imbalance < 5) {
    status = "Healthy";
  }
  else if (imbalance < 15) {
    status = "Minor Imbalance";
  }
  else {
    status = "Critical Imbalance";
  }

  Blynk.virtualWrite(V0, cell1);
  Blynk.virtualWrite(V1, cell2);
  Blynk.virtualWrite(V2, cell3);
  Blynk.virtualWrite(V3, cell4);

  Blynk.virtualWrite(V4, packVoltage);
  Blynk.virtualWrite(V5, avgVoltage);
  Blynk.virtualWrite(V6, imbalance);

  Blynk.virtualWrite(V7, status);
  Blynk.virtualWrite(V8, "Cell " + String(weakIndex));
  Blynk.virtualWrite(V9, "Cell " + String(strongIndex));

  Serial.println("Battery Data Updated");
}

void setup() {
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(1000L, sendBatteryData);
}

void loop() {
  Blynk.run();
  timer.run();
}