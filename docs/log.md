## LOG

#### 22 aug 19
- got WS2815s working
- discovered that `NUMLEDS = 375` uses 120% of available memory
  - removing all serial.print statements with static strings saved 406 bytes
  - that was enough space for all animations @ 375 leds
- measured current draw for WS2815 for each animation @ 375 leds (12.5 m)
  - ranges between 0.8-4.25 amps @ 12v
  - so for 25m of strip on big red max power draw ~ 100 watts
  - see comments in `config.h` for details

#### 14 aug 19
- set `"C_Cpp.clang_format_style": "Google",` in `.vscode/settings.json` to get javascriptish formatting rules
- added `include/printf.h`: if included, intercepts output of printf and sends to `serial.print()`. not currently used, but should be.

#### 13 aug 19
- switch push-pattern cmd dispatcher
- FIX pcb v1.3 swapped CS<->CSM pins in `Radio.h`
  ```c
    const static uint8_t PIN_RADIO_CE = 7;  // 7 on PCBs 1.3, was 6 on 1.1
    const static uint8_t PIN_RADIO_CSN = 6; // 6 on PCBs 1.3, was 7 on 1.1
  ```
- determined color channel order for SK9822: BGR

- c experiments
  - https://repl.it/@100ideas/hashmap-vs-switch
  - https://repl.it/@100ideas/knobreducer-binary-sandbox
  - https://repl.it/@100ideas/c-string-and-array-practice
  - https://www.programiz.com/cpp-programming/strings
  - https://en.cppreference.com/w/cpp/language/operator_arithmetic

- buffer rapid presses TODO pseudocode
  ```
  // TODO buffer rapid presses
    } else { ....
      IF rapid presses (where difference < shortPress):
        skip (*_aiIndex)++;
        instead increment rapidPressCount++, once per rapid press
      IF rapid presses over (buttonState == 1 && rapidPressCount > 0)
        (*_aiIndex) += rapidPressCount;
        rapidPressCount == 0;
  ```