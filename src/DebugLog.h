#include <Arduino.h>

#ifndef DebugLog_H
#define DebugLog_H

template <typename T>
void debugLog(T t) {
  Serial.println(t);
}
template <typename T1, typename T2>
void debugLog(T1 t1, T2 t2) {
  Serial.print(t1);
  Serial.println(t2);
}
template <typename T>
void debugLo(T t) {
  Serial.print(t);
}
#endif // DebugLog_H