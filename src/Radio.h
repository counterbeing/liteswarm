#pragma once

#include "DebugLog.h"
#include "MasterState.h"
#include "config.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <painlessMesh.h>
#include <stdint.h>

#define id "LiteSwarm"
#define pass "NotSoSecret"
const int port = 5555;

Scheduler myScheduler;
painlessMesh mesh;

#define LED D1

void sendCallback();
Task sendTask(TASK_SECOND * 1, TASK_FOREVER, &sendCallback);

class Radio {
 private:
  MasterState * masterState;

  void sendCallback() {
    const int knobValue = masterState->knob->getCorrectedValue();
    Serial.print("Knob Value = ");
    Serial.println(knobValue);

    String msg;
    DynamicJsonDocument doc(1024);
    doc["rotaryPosition"] = knobValue;
    serializeJson(doc, msg);
    mesh.sendBroadcast(msg);
    Serial.println("Message Sent");
  }

  void receivedCallback(uint32_t from, String msg) {
    Serial.print("Received = ");
    Serial.println(msg);
    String json = msg.c_str();
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
      Serial.print("Failed Deserializing ... ");
      Serial.println(error.c_str());
    }
    // LEDState = doc["Button"];
    // digitalWrite(LED, LEDState);
  }

 public:
  Radio(MasterState * masterState_)
      : masterState(masterState_) {}

  // uint8_t getLatestReceivedAnimationId() {
  //   return _incomingRadioPacket.animationId;
  // }

  // uint32_t getLatestReceivedRotaryPosition() {
  //   return _incomingRadioPacket.rotaryPosition;
  // }

  void setup() {
    // if (RADIO_DEBUG) {}
    Serial.begin(115200);
    // pinMode(LED, OUTPUT);

    mesh.init(id, pass, &myScheduler, port);
    mesh.onReceive(&receivedCallback);
    myScheduler.addTask(sendTask);
    sendTask.enable();
  }

  void loop() { mesh.update(); }
};
