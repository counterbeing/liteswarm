///////////////////////////////////////////////////////////////////////////////////
// trying to use `map` implementation from `ustd` lib for arduino / atmega, but...
// it compiles OK but intellisense shows errors
//
// lightweight map implementations:
// - this one https://github.com/muwerk/ustd
// - another implementation https://attractivechaos.wordpress.com/2009/09/29/khash-h/
//
// posted the following on `ustd` issue tracker: https://github.com/muwerk/ustd/issues/2:
//
// vscode c/c++ intellisense is/was throwing errors claiming it cant find deps 
// included by `ustd` package... but the include preprocessor directives causing 
// problems are conditionally dependant on __APPLE__ or __LINUX__ being defined. 
// not sure why they are allowed to be by platformio since we are compiling for atmel
// 
// first tried to fix by setting DEFINE (-Dxxxx) and UNDEFINE (-Uxxxx) build flags
// in `platformio.ini`, which auto-generates `.vscode/c_cpp_properties.json` used
// by intellisense. example:
//
//   ; platformio.ini [env:nano]
//   build_flags = -D__ATMEGA__=1 -U__APPLE__ -U__linux__
// 
// intellisense complains:
//
//   #include errors detected. Please update your includePath. Squiggles are disabled for this translation unit (/Users/100ideas/dev/y_bm/bikelights/src/main.cpp).
//   cannot open source file "cassert" (dependency of "platform.h")
//
//   PERHAPS this is bc these are the  WRONG FLAGS for the atmelavr `avr-gcc`? See
//     https://gcc.gnu.org/onlinedocs/gcc-2.95.3/cpp_1.html#SEC42
//    
//
//  refs:
//    https://docs.platformio.org/en/latest/projectconf/section_env_build.html
//    https://docs.microsoft.com/en-us/cpp/preprocessor/macros-c-cpp?view=vs-2019
//    https://docs.microsoft.com/en-us/cpp/preprocessor/preprocessor-directives?view=vs-2019
//    https://clang.llvm.org/docs/JSONCompilationDatabase.html
//    http://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Preprocessor-Options.html#Preprocessor-Options
//    https://github.com/Microsoft/vscode-cpptools/issues?q=include++squiggle
//    https://github.com/microsoft/vscode-cpptools/issues/899
//
// the following seems to save intellisense
// https://gcc.gnu.org/onlinedocs/gcc-2.95.3/cpp_1.html#SEC18
//
#undef __APPLE__
#ifndef __ATMEGA__
#define __ATMEGA__
#endif
///////////////////////////////////////////////////////////////////////////////////


// the following documentation is from `ustd/map.h`

/*! \brief Lightweight c++11 dictionary map implementation.

map.h is a minimal, yet highly portable dictionary map type implementation
that runs well on architectures with very limited resources such as attiny 8kb
avr.

The map class either:

* * allocates memory dynamically on map defintions, or
* * work in fully static mode without any dynamic allocation once the map
object has been created.

The library header-only.

Make sure to provide the <a
href="https://github.com/muwerk/ustd/blob/master/README.md">required platform
define</a> before including ustd headers.

## An example for dynamic mode:

~~~{.cpp}
#define __ATTINY__ 1  // Appropriate platform define required
#include <map.h>

ustd::map<int,double> myMap;

myMap[0] = 13.3; // Memory for map is allocated, if necessary
double p = myMap[0];
~~~

## An example for static mode

~~~{.cpp}
#define __ATTINY__ 1  // Appropriate platform define required
#include <map.h>

// map size is fixed 5 (startSize==maxSize), no dynamic extensions:
ustd::map<int, float> mayMap = ustd::map<int,float>(5, 5, 0, false);
~~~
 */


#include <platform.h>
#include <array.h>
#include <map.h>
#include <stdint.h>
#include <printf.h> // local printf wrapper that sends to serial

typedef struct {
  uint32_t firstSeen;
  uint32_t lastSeen;
  int score;
} RadioStats;

/////////////////////////////////////////////////
//  SwarmMap - tracks active devices nearby over time
//  
// scoreThreshold * debounceWindow = minimum time (ms) before device can be considered active
// device must transmit at elast 1 packet per timeoutWindow to remain under consieration
//

class SwarmMap {
 private:
  uint32_t timeoutWindow = 10000;
  uint32_t debounceWindow = 2000;
  int scoreThreshold = 4;
  uint32_t lastIntervalTime = millis();
  ustd::map<uint16_t, RadioStats> seenMap;
  // uint16_t ids [20];

  bool runAfterInterval(int interval) {
    long nextRunTime = lastIntervalTime + interval;
    long current = millis();
    if (current > nextRunTime) {
      lastIntervalTime = current;
      return true;
    }
    return false;
  }

 public:
  SwarmMap(){
    printfn("SwarmMap initialized");
  }
    
  // track device activity over time
  void logPacketFrom(uint16_t id) { 
    // not in map, create new struct for it
    if (RADIO_DEBUG){
      printfn("\n+++logPacketFrom %d", id);
    }
    if (seenMap.find(id) == -1) { 
      RadioStats _stat = {millis(), millis(), 0};
      seenMap[id] = _stat;
      printfn("\ncreate RadioStat struct %u", seenMap[id].firstSeen);
    } else {
      // its in the map, but hasnt been active enough yet
      // has it been longer than debounceWindow since last update? increase score //
      if(
        (seenMap[id].lastSeen != 0) &&
        (seenMap[id].score < scoreThreshold) &&
        ((millis() - seenMap[id].firstSeen) > debounceWindow)
        ){ 
          seenMap[id].score += 1;
          printfn("increase score for %d to %d", id, seenMap[id].score);
          seenMap[id].firstSeen = millis();
      }
      seenMap[id].lastSeen = millis();
    }
  }

  // calc time diffs, activity counts in time windows; call in main loop
  int getSwarmSize() {
    int count = 0;
    // if(seenMap.length() > 0) {  // these don't work bc they return allocated size of array not # of elemenets
    // if(!seenMap.isEmpty()) {
    if(seenMap.keys.length() > 0) {
      for (int i = 0; i < seenMap.keys.length(); i++) {
        // hasn't been seen for 10 sec, remove from map
        if ((millis() - seenMap[seenMap.keys[i]].lastSeen) > timeoutWindow){
          uint16_t _id = seenMap.keys[i];
          if(RADIO_DEBUG){
            Serial.print("\nid: ");
            Serial.print(_id);
            Serial.print("\tseen ago: ");
            Serial.print(millis() - seenMap[_id].lastSeen);
            Serial.print("\terase?: ");
            Serial.print((millis() - seenMap[_id].lastSeen) > timeoutWindow);
            Serial.print("\tscore++?: ");
            Serial.print((millis() - seenMap[_id].lastSeen) > debounceWindow);
            Serial.print("\tscore?: ");
            Serial.print(seenMap[_id].score);
            Serial.print("\nseenMap.length() ");
            Serial.print(seenMap.length());
            Serial.print(";\tseenMap.keys.length() ");
            Serial.print(seenMap.keys.length());
            Serial.print(";\tdeleting seenMap[");
            Serial.print(_id);
            Serial.print("]\tseenMap.values[i].firstSeen ");
            Serial.print(seenMap.values[i].firstSeen);
          }
          int eraseStatus = seenMap.erase(_id);
          if(RADIO_DEBUG){
            Serial.print("\nerased? ");
            Serial.print(eraseStatus);
            Serial.print(". new seenMap.keys.length() ");
            Serial.print(seenMap.keys.length());
            Serial.print("\tseenMap.keys: ");
            for (int j = 0; j < seenMap.keys.length(); j++) {
              Serial.print(seenMap.keys[j]);
              Serial.print(", ");
            }
            Serial.print("\n");
          }
        // we've seen enough packets over time to be confident it is an active neighbor
        } else if (seenMap[seenMap.keys[i]].score >= scoreThreshold) {
          if(RADIO_DEBUG && runAfterInterval(debounceWindow)){
            Serial.print("\nactive id: ");
            Serial.print(seenMap.keys[i]);
            Serial.print(", last seen ");
            Serial.println(millis() - seenMap[seenMap.keys[i]].lastSeen);
          }
          count++;
        }
      }
    }
    return count;
  }

};


/////////////////////////////////////////////////
//  test function
//
//  constructor params: 
//    ustd::map(unsigned int startSize=ARRAY_INIT_SIZE, unsigned int maxSize=ARRAY_MAX_SIZE, unsigned int incSize=ARRAY_INC_SIZE, bool shrink=true)
//    https://muwerk.github.io/ustd/docs/classustd_1_1map.html#a1b0670916b74ab7628fb3da556589d98 


// ustd::map<int, uint16_t> mp;

// uint16_t testsize = 33220;
// // https://github.com/muwerk/ustd/blob/master/Examples/mac-linux/ustd-test.cpp
// static int MapTest() {
//   printfn("\n\nsizeof uint_16 (bits): %d", sizeof (uint16_t)*8);
//   printfn("UINT16_MAX: %u\n", UINT16_MAX);
//   printfn("sizeof long (bits): %d", sizeof (long)*8);

//   for (int i = 0; i < 20; i++) {
//     printf("%d ", i);
//     // mp[i] = i*random()*7;
//     mp[i] = random(10,65532);
//     printf(" - ");
//     printfn("%u", mp[i]);
//   }
//   printfn("mp len: %d\n", mp.length());
//   printfn("Done ustd.");
//   return 0;
// }

// static void ArrTest() {
//   ustd::array<int> intArray = ustd::array<int>(5, 5, 0, false);
  
//   int foo = 26;
//   // intArray[0] = 13; // Memory for array[0] is allocated
//   // intArray.add(3);  // the array is extended, if necessary
//   intArray.add(foo);
//   int p = intArray[0];
 
//   printf("[0]:%d [1]:%d length=%d\n", intArray[0], intArray[1], intArray.length());
//   intArray.erase(0);
//   printf("[0]:%d [1]:%d length=%d\n", intArray[0], intArray[1], intArray.length());
// }