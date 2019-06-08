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
- cory improving codebase, rotary switch
- using platform.io (compile/upload to arduino)
- TODO all
  - move bike find mode to SOS press - short long short - or something like it (it's in the middle)
- mac wants gait recognition mode
  - TODO 
---

## LOG
#### 6 Jun 18
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