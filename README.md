## Arduino Project for Bike Lights for Burning Man

Cheap simple hardware that can be put together in a robust and ad-hoc way to
keep you safe for days of partying.

Materials:
- Arduino Nano or equivalent
- RGB Led strip
- Rotary encoder
- Soldering kit

## Development
Things you might be missing.
- edit using VSCode with PlatformIO plugin
- OpenOCD some USB stuff `brew install openocd` (or else `dyld: Library not loaded: /usr/local/opt/libftdi/lib/libftdi1.2.dylib`)


## SOFTWARE TODO
- cory improving codebase, rotary switch
- using platform.io (compile/upload to arduino)
- TODO all
  - move bike find mode to SOS press - short long short - or something like it (it's in the middle)
  - 
- mac wants gait recognition mode
  - TODO 
---

## LOG
#### 6 Jun 18
- cory n mac join forces
- we both fail at trying to win with vscode config + platformio.... for now
- ...?


---

## OTHER NOTES

Super tall tail for big red
- material?

Homing system for big red
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