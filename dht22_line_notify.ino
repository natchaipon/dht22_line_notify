#include <TridentTD_LineNotify.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <time.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define SSID        "P"
#define PASSWORD    "12345678"
#define LINE_TOKEN  "9RYzfrmyTQS92nGDVRZAQmXb8KiYF2wdazEPPEZ4JF9"

bool state = false;
bool state_t_8 = false;
bool state_t_13 = false;

int timezone = 7 * 3600;
int dst = 0;

long previousMillis = 0;
long interval = 1000 * 60 * 1;
int i = 0;

float h = 0;
float t = 0;

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  LINE.setToken(LINE_TOKEN);
  dht.begin();
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
}


void loop() {
  h = dht.readHumidity();
  t = dht.readTemperature();

  if (isnan(t) || isnan(h)) {
    Serial.println("Error reading DHT!");
    return;
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("\t");
    Serial.print("Temp: ");
    Serial.println(t);
    Serial.println("...............................");

    configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    int t_h = p_tm->tm_hour;
    int t_m = p_tm->tm_min;

    if (t_h == 8 && t_m == 10 && state_t_8 == false) {
      line_alert();
      state_t_8 = true;
    }
    else if (t_h != 8 && t_m != 10 && state_t_8 == true) {
      state_t_8 = false;
    }

    if (t_h == 13 && t_m == 10 && state_t_13 == false) {
      line_alert();
      state_t_13 = true;
    }
    else if (t_h != 13 && t_m != 10 && state_t_13 == true) {
      state_t_13 = false;
    }

    if (t >= 28 && i <= 2) {
      if (i == 0) {
        for (int num = 0; num <= 2; num++)
          line_alert_danger();
          i++;
      }
      else {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis > interval) {
          previousMillis = currentMillis;
          Serial.println("line alert");
          for (int num = 0; num <= 2; num++) {
            line_alert_danger();
          }
          Serial.println("----------------------------------");
          i++;
          //Serial.println(i);
        }
      }
    }
    else if (t < 28) {
      i = 0;
    }
  }
}


void line_alert() {
  LINE.notify("อุณหภูมิ " + String(t) + " °\n" + "ความชื้น " + String(h) + " %");
}

void line_alert_danger() {
  LINE.notify("***อุณหภูมิสูงเกินกำหนด ผู้ดูแลควรตรวจสอบ***\nอุณหภูมิ " + String(t) + " °\n" + "ความชื้น " + String(h) + " %");
}
