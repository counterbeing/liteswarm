## Arduino Project for Bike Lights for Burning Man

Cheap simple hardware that can be put together in a robust and ad-hoc way to
keep you safe for days of partying.

### Materials / Shopping List
- [portable battery packs](https://smile.amazon.com/Smallest-Jackery-3350mAh-Portable-Charger/dp/B00L9F95RO/ref=sr_1_8?keywords=jackery+battery+pack&qid=1559954560&s=gateway&sr=8-8), not a bad idea to have two. These are small enough to fit inside of a fuzzy scarf, and they don't turn off on you. You can get a few hours out of one of these... Depends on the animation you run.
- [fabric](https://www.joann.com/luxury-faux-husky-fur-fabric--white/14387674.html)
- [arduino nanos](https://smile.amazon.com/gp/product/B07HF5RMHV/ref=crt_ewc_title_dp_2?ie=UTF8&psc=1&smid=A278BT9THV444Y)
- [led strips](https://www.aliexpress.com/item/5m-5V-WS2812B-WS2812-Led-Strip-light-Individually-Addressable-Smart-RGB-Led-pixel-strips-Black-White/32995915537.html?spm=a2g0o.cart.0.0.69a03c00Io4CfT)
- [usb cables](https://www.amazon.com/HTTX-Straight-Soldering-Connector-Replacement/dp/B07D1HDJM8/ref=sr_1_10?keywords=usb-a+connector+male&qid=1559956539&s=gateway&sr=8-10)
- [rotary encoder](https://smile.amazon.com/DIYhz-Rotary-Encoder-Digital-Potentiometer/dp/B07D3D64X7/ref=sr_1_10?keywords=rotary+encoder&qid=1559955080&s=industrial&sr=1-10)
- [wire](https://www.amazon.com/StrivedayTM-Flexible-Silicone-electronic-electrics/dp/B01KQ2JNLI/ref=sr_1_8?keywords=soldering+wire&qid=1559956794&s=gateway&sr=8-8)

## Development
Things you might be missing.
- edit using VSCode with PlatformIO plugin
- OpenOCD some USB stuff `brew install openocd` (or else `dyld: Library not loaded: /usr/local/opt/libftdi/lib/libftdi1.2.dylib`)





## SOFTWARE TODO

#### last updated 22 aug 18
  - [ ] add `ifdefs` to `config.h` to set options for ws2811, ws2815, sk9822
  - [x] `config.h` `KNOBDEBUG` & `RADIODEBUG` enable/disable serial print lines
    - [ ] debug flag that enables the above and sets `NUMLEDS` to ~10 (trade leds  for serial print strings in memory)
  - [ ] save memory by casting `long`s to something else (`uint_16`?) i.e. in MyKnob.h
  - [x] wrap press duration logic in descriptive variables
  - [x] swap order of cmdMode pattern from short-med to med-short
  - [ ] animation fx return `FastLed[]`, then main fx optionally combines overlay `led[]` before writing to strip
  - [ ] add visual feedback (use amimationCombinator w/ strobe)
  - [x] lowpower mode triggered by 3000 ms long press
  - [ ] buffer rapid presses?


## PCB TODO

#### last updated 22 aug 18
  - [ ] switch 6/7 labels on silkscreen
  - [ ] switch clock & data on silkscreen
  - [ ] move position of version label on silkscreen so visible after wires added
  - [ ] increase inside diameter of power holes, radio holes
  - [ ] add holes to the corners, esp opposite radio end; use ziptie thru holes to secure cables
  - [ ] switch radio pins layout from 1x7 to 2x8 compatibile with female ribbon cable header (we'll just solder 2 rows of 1x4 header pins to those holes then push the ribbon cable on)

  
---

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

#### 6 Jun 19
- cory n mac join forces
- we both fail at trying to win with vscode config + platformio.... for now

---

## OTHER NOTES

Super tall tail for big red
- material?

Homing system for big red
- [like this maybe](http://www.migratoryconnectivityproject.org/vhf-radios/)
- audio / visual 
- TODO CORY: beep... beep... beep... beep beep beep BEEEEEEP homing missile
    - thoughts: zigbee? wifi signal strength dongle? bluetooth beacon 
- TODO MAC: tubing - bilge pump tubing 

wiring: TODO MAC
- amplify logic line 
- figure out ideal gauge power wires
TEST: 
- 2m, then 5m
- try injencting power at 0, 2.5, 5m
- make better power connectors using ground power supply (ATX)
- then switch to lithium battery + buck converter