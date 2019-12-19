# 2019-12-17 solution to struct padding

```c
// Radio.h

// compiler will automatically pad struct differently depending on architecture
// atmega328 is 8bit; teensy is 32bit; (can be disabled with `#pragma pack(1)`)
// the padding happens if larger members come after smaller members. so quick fix
// is to organize members from largest to smallest.

// OLD code - caused padding problems when unpacking 
// struct RadioPacket          // Any packet up to 32 bytes can be sent.
// {                           // 0 - bit count (256 max); byte count
//   uint8_t SHARED_SECRET;    // 8  : 1
//   int32_t rotaryPosition;   // 40 : 5
//   int8_t animationId;       // 48 : 6
//   uint16_t senderId;        // 64 : 8
//                             // ... 256 : 32
// };

// fix
struct RadioPacket          // Any packet up to 32 bytes can be sent.
{                           // 0 - bit count (256 max); byte count
  int32_t rotaryPosition;   // 32 : 4
  uint16_t senderId;        // 48 : 6
  uint8_t SHARED_SECRET;    // 56 : 7
  int8_t animationId;       // 64 : 8
                            // ... 256 : 32
};
```


## other fixes

- animation selection loop causes device to reset/crash once `animation_index` is set to 0 again
  - fixed: sort of. have not figure out cause, but skipping the animation at `animation_index = 0` avoids the crash and cycles through other animation as expected.
  - the animation causing the problem is `crossfade`. or maybe its the index? not sure.
  - crossfade is diff from others in that it defines `setup()` as public method instead of private one.

- fixed: knobPosition outside `knob.confine()` interval causes endless loop of update packets until knob is returned to range. fixed w/ liberal sprinkling of `knob.confine()` in radio code. bad.

to monitor serial of two connected devices (add `~/.platformio/penv/bin` to path)
```
pio device monitor --environment teensy
pio device monitor --environment nano
```

---

# 2019-12-12 teensy dev & debugging

working on refactoring code from the 'master' branch for teensy_lc.

Hardest part was figuring out the correct pinouts for the radio. 
- `neopixel` style LED strip worked right off the bat
- so did rotary encoder
- radio is hard b/c nrf docs use terms like `csn`, & `ce`, but teensy uses `ss` and doesn't mention the other term as much. See the section in the `readme.md` for an overview.

problem initially presented itself as nrf radio module + NrfLite lib on teensy reporting via terminal that 1) `radio ok`  but 2) logged values for received packets that never changed. As if the packets weren't really getting decoded (although they were being received somehow).

Three theories:

1). wiring is screwed up connecting teensy to nrf, so it isn't able to fully read or correctly decode incoming packets.

2). there is a buffer overflow, typecasting/type mixing mistake in the code. the nrf is working, receiving packets, but decoding them is not; instead we get nonsense or zeor values from random memory locations. I was leaning towards this explanation because I found a ton of mixed types (uint8_t -> int; long -> int, etc). But after standardizing a bunch of them, still got communication problems.

3). the teensy has a much faster clock (100mhz?) than the arduino. According to the official documentation and forum/issue posts on nrflite repo, the crystal resonator in the nrf24l01 that generates the radio frequencies is unable to work effectively above some frequency range... perhaps 8khz at the high end? [Here is an issue on nrflite recommending limiting clock speed to 4khz](https://github.com/dparson55/NRFLite/issues/19). But based on reading the issue, it doesn't sound particularly straightforward to force the arduino + nrf libs running on the teensy to step down to 4khz. Gonna play with this next. [also a forum post on teensy forums](https://forum.pjrc.com/threads/40499-Need-to-change-SPI-clock-rate-for-RF24-radio-modules-w-TMRh20-library).

4). well one last theory: I skimmed the commit log for the master branch (counterbeing/bikelights) and didn't see anything too alarming, so thats what I adapted. But its totally possible it has subtle work-in-progress bits that are tripping up the logic of the current program. I've gone over everything a multiple times but nothing is obviously screwed up... could you guys have a look?

### changes on this branch
- added more preporcessor / compiler directives to config.h to simplofy switching between developing for nano vs teensy
- added teensy config to `platformio.ini`. this should make two `"env"` (board)-dependant sections appear in the platformio "project tasks" sidebar, one for nano, and one for teensy. Just plug whichever board in and click the appropriate build/compile/monitor - it figures it out on its own.
- standardized data types used to pack & upack the radio packet.

### symptoms
see console log below. its the serial output of a nano running on my desk right next to the teensy. its getting the correct 'shared_secret', and 'animationId' increments on the receiver if I spem the rotary over and over, but the system almost immediately races away from user input towards some minima or maxima; in particular, neither unit successfully synchronizes towards shared state with the other one, at least with the 'rotaryPosition' parameter. The other params seem to be more stable.

So I think whats happening is a bad logic around [line 90 of Radio.h](https://). The `stateChanged()` method always sees a different local rotaryPosition than the one in the incoming packet, so it spams updates at the other device, the recipient spams back, and both get locked in an unstable positive feedback loop.  (*note: maybe we should add a naieve 'back-off' algorithm to the radios to keep the packet density from spiraling out of control like this - that, and adding CRC / checksum code...)

**anyway, I can't figure out why currentRotary** isn't getting set correctly from the incoming packets. Am I trying to compare the literal value of the pointer to the local currentRotary with the proper value of the incoming packet (fail), or is the memory space of the struct in which the packet is stored is getting overriden (yes? can't tell), or is it unintended behavior from the interplay of code that sets the `rotaryPosition` after it changes (due to incoming packet or user input) and the code the animations call (` `MyKnob.h` line 442: confine()`) to scale and loop the rotary values over a particular range?

Well, I've tried a bunch of little experiments and came up with nada. I finished 2/3rds of the pcb of the teensy yesterday and I need to finish it and submit it soon to have a chance at getting it fabbed for xmas. I would love to have fully validated the software side before comitting to particular pins on the teensy but I'm stuck - can you help?  Tell me if I'm using an out of date branch (gaaaahhh), or perhaps you'll see the data type error if there is one, or merge my config.h changes into your/Tony's remastered branch.

---

see how `rotaryPosition` changes (or doesn't) in response to me playing with the knob on either the teensy (remote) or nano (logged to serial).

some interesting values:

`rotaryPosition`:

| incoming value | local value |
| -------------- | ----------- |
| `-939524096`   | `5`         |
| `0`            | `5`         |
| ...later       | ...         |
| '0'            | '255'       |

- senderId sometimes shows the correct value
- shared secret was `0` for a long time. not sure how I fixed it.


## control test
testing this codebase with two vanilla nano scarves, both neopixel

#### test 1: 
- nano_a has code from this branch (teensy) with latest commit
- nano_b has whatever code I put on it over thanksgiving. (it was working then).
-  rotary on nano_b 
  - clicking: nano_b flashes quickly - probably switching to new animation then instantly switching back. won't stay switched. similar to teensy.
  - twisting - unsure if this does anything
  - nothing shows up in serial
  - nothing appears to happen to nano_a
-  rotary on nano_a:
  - clicking: switched to next animation for approx 1 sec, then back. no effect on nano_a. fast clicking can advance through several animations
  - twisting: seems to modulate animation as expected

#### test 2: 
- nano_a & nano_b have code from this branch (teensy) with latest commit
- **AW FUCK** both scarves perform more or less as expected. no crazy values, no runaway feedback loops....
- **so its probably not code... but bad wiring or the clock rate on the teensy**.

## new theory: compiler pads memory space of packet struct differently on nano (8-bit) vs teensy (32-bit)

I think this is what's going on! will test soon.

apparently the c compiler tries optimize the memory layout of *struct*ure members according to constraints / rules that partially depend on system architecture (word size?). If the struct's members are of different sizes, the compiler may elect to pad the smaller ones such that the whole struct lays out more regularly in memory.

Our code currently packs 4 numbers into a 48-byte `struct` that is passed to the radio for transmission, or conversely received and unpacked.

The preferred memory layout of the stuct in memory space is probably different on teensy vs nano - thus leading to what appear to be buffer overrun type bugs when one platform gets a packet with that was packed and padded for a different architecture.

apparently the `#pragma pack(n)` compiler directive can control the compilers packing strategy, where n = how many padding bytes can be added around each member to make the struct's overall memory footprint "square". `n = 1` should make it nice and "1-dimensional" and resolve the problem if we're lucky (hopefully its not more complicated than this).


**thanks be to @SadatD for the following explaination, originally posted to https://stackoverflow.com/questions/3318410/pragma-pack-effect#3318475**: 

>  `#pragma pack` instructs the compiler to pack structure members with particular alignment.  >  Most compilers, when you declare a struct, will insert padding between members to ensure that they are aligned to appropriate addresses in memory (usually a multiple of the type's size).  This avoids the performance penalty (or outright error) on some architectures associated with accessing variables that are not aligned properly.  For example, given 4-byte integers and the following struct:
>
>      struct Test
>      {
>         char AA;
>         int BB;
>         char CC;
>      };
>  
>  The compiler could choose to lay the struct out in memory like this:
>  
>      |   1   |   2   |   3   |   4   |  
>  
>      | AA(1) | pad.................. |
>      | BB(1) | BB(2) | BB(3) | BB(4) | 
>      | CC(1) | pad.................. |
>  
>  and `sizeof(Test)` would be 4 &times; 3 = 12, even though it only contains 6 bytes of data.  >  The most common use case for the `#pragma` (to my knowledge) is when working with hardware devices where you need to ensure that the compiler does not insert padding into the data and each member follows the previous one.  With `#pragma pack(1)`, the struct above would be laid out like this:
>
>      |   1   |
>  
>      | AA(1) |
>      | BB(1) |
>      | BB(2) |
>      | BB(3) |
>      | BB(4) |
>      | CC(1) |
>  
>  
>  
>  And `sizeof(Test)` would be 1 &times; 6 = 6.
>  
>  With `#pragma pack(2)`, the struct above would be laid out like this:
>  
>      |   1   |   2   | 
>  
>      | AA(1) | pad.. |
>      | BB(1) | BB(2) |
>      | BB(3) | BB(4) |
>      | CC(1) | pad.. |
>  
>  And `sizeof(Test)` would be 2 &times; 4 =  8.
>  
>  Order of variables in struct is also important. With variables ordered like following:
>  
>      struct Test
>      {
>         char AA;
>         char CC;
>         int BB;
>      };
>  
>  and with `#pragma pack(2)`, the struct would be laid out like this:
>  
>  
>      |   1   |   2   | 
>  
>      | AA(1) | CC(1) |
>      | BB(1) | BB(2) |
>      | BB(3) | BB(4) |
>  
>  and `sizeOf(Test)` would be 3 × 2 = 6.
>   .



references:
- https://forum.pjrc.com/threads/24940-Teensy-3-1-nrf24l01-issue
- https://docs.microsoft.com/en-us/cpp/preprocessor/pack?view=vs-2019



---

### nano communicating with teensy v2 (teensy branch latest)
``` log
> Executing task: platformio run --target upload --target monitor --environment nano <

Processing nano (platform: atmelavr; board: nanoatmega328new; framework: arduino)
--------------------------------------------------------------------------------------------------
Verbose mode can be enabled via `-v, --verbose` option
CONFIGURATION: https://docs.platformio.org/page/boards/atmelavr/nanoatmega328new.html
PLATFORM: Atmel AVR 2.0.0 > Arduino Nano ATmega328 (New Bootloader)
HARDWARE: ATMEGA328P 16MHz, 2KB RAM, 30KB Flash
PACKAGES: toolchain-atmelavr 1.50400.190710 (5.4.0), framework-arduino-avr 5.0.0, tool-avrdude 1.60300.190628 (6.3.0)
LDF: Library Dependency Finder -> http://bit.ly/configure-pio-ldf
LDF Modes: Finder ~ chain, Compatibility ~ soft
Found 11 compatible libraries
Scanning dependencies...
Dependency Graph
|-- <FastLED> 3.3.2
|   |-- <SPI> 1.0
|   |-- <SoftwareSerial> 1.0
|-- <Bounce2> 2.52
|-- <Encoder> #4c4ec3a
|-- <SPI> 1.0
|-- <NRFLite> 2.2.2
|   |-- <SPI> 1.0
Building in release mode
Checking size .pio/build/nano/firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
DATA:    [=======   ]  68.9% (used 1412 bytes from 2048 bytes)
PROGRAM: [=====     ]  46.6% (used 14306 bytes from 30720 bytes)
Configuring upload protocol...
AVAILABLE: arduino
CURRENT: upload_protocol = arduino
Looking for upload port...
Auto-detected: /dev/cu.usbserial-1460
Uploading .pio/build/nano/firmware.hex

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e950f (probably m328p)
avrdude: reading input file ".pio/build/nano/firmware.hex"
avrdude: writing flash (14306 bytes):

Writing | ################################################## | 100% 2.51s

avrdude: 14306 bytes of flash written
avrdude: verifying flash memory against .pio/build/nano/firmware.hex:
avrdude: load data flash data from input file .pio/build/nano/firmware.hex:
avrdude: input file .pio/build/nano/firmware.hex contains 14306 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 1.87s

avrdude: verifying ...
avrdude: 14306 bytes of flash verified

avrdude: safemode: Fuses OK (E:00, H:00, L:00)

avrdude done.  Thank you.

================================== [SUCCESS] Took 6.97 seconds ==================================
--- Miniterm on /dev/cu.usbserial-1460  115200,8,N,1 ---
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
0Picking random radio id: 17831
------CE & CSN---------
PIN_RADIO_CE (chip enable): 7
PIN_RADIO_CSN (chip select / slave select)?: 6
radio ok
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: -939524096
         (local rotaryPosition: 40)
animationId: 0  (local animationId: 0)
senderId: 0
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: -939524096
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 5)
animationId: 0  (local animationId: 0)
senderId: 0

--- exit ---

Environment    Status    Duration
-------------  --------  ------------
nano           SUCCESS   00:00:06.972
teensy         IGNORED
================================== 1 succeeded in 00:00:06.972 ==================================

Terminal will be reused by tasks, press any key to close it.
```


### nano communicating with teensy part 2 (teensy branch latest)
```log
 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
         (local rotaryPosition: 255)
animationId: 4  (local animationId: 4)
senderId: 0
--- Sending Data
------INCOMING---------
SHARED_SECRET: 42
rotaryPosition: 0
```
