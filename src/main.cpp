#include <Arduino.h>
#include <array>

#include "ring.hpp"
#include "ecg.hpp"


#define MS_FACTOR 1000ULL
#define TIME_TO_SLEEP 1000

#define LED_BUILTIN 19


const auto max_rtc_size = 8 * 1024;
const auto max_ssid_size = 32;
const auto max_pass_size = 64;
const auto ring_size = 2048;


struct AppState {
  uint64_t boot_count;    // счетчик пробуждений.
  uint64_t last_write_at; // когда 
  std::array<char, max_ssid_size> wifi_ssid; // zero padded name
  std::array<char, max_pass_size> wifi_pass; // zero padded password
};


// Up to 8 KB RTC Slow Memory
RTC_DATA_ATTR struct {
    AppState state;
    RingBuffer<ECGSample, ring_size> samples;
} memory {
  0, /* boot_count */
};

void go_to_sleep() {
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * MS_FACTOR);
  esp_deep_sleep_start();
}

void do_init() {
}

void toggle_led() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // will switched off by deep sleep
}

void do_work() {
  toggle_led();
  Serial.begin(115200);
  delay(10);
  Serial.printf("boot #%lu\n", memory.state.boot_count);
}

void setup() {
  if (memory.state.boot_count++ == 0) do_init();
  do_work();
  go_to_sleep();
}

void loop() {
  // put your main code here, to run repeatedly:
}