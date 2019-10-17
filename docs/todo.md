## SOFTWARE TODO

#### last updated 22 aug 18
  - [ ] add visual feedback (use amimationCombinator w/ strobe)
  - [ ] buffer rapid presses?
  - [ ] debug flag that enables the above and sets `NUMLEDS` to ~10 (trade leds  for serial print strings in memory)
  - [ ] save memory by casting `long`s to something else (`uint_16`?) i.e. in MyKnob.h
  - [x] Ensure radio is muted for first second or so of operation. This will ensure that a newly booted radio doesn't alter the group's animation sequence.
  - [x] Ensure that non-matching packet secrets ignore packet.
  - [x] `config.h` `KNOB_DEBUG` & `RADIO_DEBUG` enable/disable serial print lines
  - [x] add `ifdefs` to `config.h` to set options for ws2811, ws2815, sk9822
  - [x] lowpower mode triggered by 3000 ms long press
  - [x] swap order of cmdMode pattern from short-med to med-short
  - [x] wrap press duration logic in descriptive variables



## PCB TODO

#### last updated 22 aug 18
  - [ ] switch 6/7 labels on silkscreen
  - [ ] switch clock & data on silkscreen
  - [ ] move position of version label on silkscreen so visible after wires added
  - [ ] increase inside diameter of power holes, radio holes
  - [ ] add holes to the corners, esp opposite radio end; use ziptie thru holes to secure cables
  - [ ] switch radio pins layout from 1x7 to 2x8 compatibile with female ribbon cable header (we'll just solder 2 rows of 1x4 header pins to those holes then push the ribbon cable on)

  