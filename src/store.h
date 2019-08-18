#include "cedux.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "printf.h"

void printer(){
  printf("Generic Subscriber.  Num leaves %d! Data: %d:\n", 2, 200);
}

// HEADER DETAILS *************************************************************************************
// Normally this code would be in a header file. For the sake of having a concise demo, I'm putting it
// all in one file.
//
struct helicopter_state {
  uint32_t motor_speed;
  uint32_t desired_elevation;
  uint32_t current_elevation;
};

enum helicopter_action_type {
  NEW_ELEVATION,
  INCREASE_ELEVATION,
  DECREASE_ELEVATION,
};

struct absolute_elevation {
  uint32_t elevation;
};

struct elevation_change {
  int16_t delta;
};

// Tagged union action type definition
struct helicopter_action {
  enum helicopter_action_type type;
  union {
    struct absolute_elevation new_elevation;
    struct elevation_change increase_elevation;
    struct elevation_change decrease_elevation;
  };
};

// This macro declares the store data structure as well as all of the functions which are needed
// to register reducers, and subscribers, and dispatch actions. It also declares the cedux_run_my_store
// function.
CEDUX_DECLARE_STORE(struct helicopter_state, struct helicopter_action, my_store);

// This macro defines the implementation of the functions which are used to register reducers,
// subscribers, and dispatch actions. It also defines the cedux_run_my_store function
CEDUX_DEFINE_STORE(struct helicopter_state, struct helicopter_action, my_store);


// struct my_store_handle my_store; //for subscribers?

// uint32_t determine_new_speed();

  my_store = cedux_init_my_store(); // Initialize the internals of the store (list, queue)
  my_store.motor_speed = 1;        // set initial state
  my_store.current_elevation = 0;

  // register reducers
  cedux_register_reducer_my_store(&my_store, handle_new_elevation);
  cedux_register_reducer_my_store(&my_store, handle_new_desired_elevation);

/////////////////////////////////////////////////////////////////////////////
// define reducers
void handle_new_elevation(struct helicopter_state * p_state, struct helicopter_action action)
{
  if (action.type == NEW_ELEVATION) {
    p_state->current_elevation = action.new_elevation.elevation;
    // uint32_t new_speed = determine_new_speed(p_state->desired_elevation, p_state->current_elevation);
    uint32_t new_speed = 10201;
    p_state->motor_speed = new_speed;
  }
}

void handle_new_desired_elevation(struct helicopter_state * p_state, struct helicopter_action action)
{
  if (action.type == INCREASE_ELEVATION) {
    // Do some bounds checking of course...
    p_state->desired_elevation += action.increase_elevation.delta;
  } else if (action.type == DECREASE_ELEVATION) {
    // Do some bounds checking of course...
    p_state->desired_elevation -= action.decrease_elevation.delta;
  } else {
    return;
  }
  uint32_t new_speed = determine_new_speed(p_state->desired_elevation, p_state->current_elevation);
  p_state->motor_speed = new_speed;
}
//===========================================================================


/////////////////////////////////////////////////////////////////////////////
// define action dispatchers
void altimeter_isr() {
  printf("altimeter_isr reducer called\n");
  // uint32_t new_elevation = read_elevation();
  uint32_t new_elevation = my_store.current_elevation + 10;
  cedux_dispatch_my_store(&my_store, (struct helicopter_action){
    .type = NEW_ELEVATION,
    .elevation = new_elevation
  });
}

void up_button_isr() {
  printf("up_button_isr called");
  cedux_dispatch_action(&my_store, (struct helicopter_action){
    .type = INCREASE_ELEVATION,
    .increase_elevation = {
      .delta = 10
    }
  });
}
//===========================================================================


void _setup(){
  // my_store = cedux_init_my_store(); // Initialize the internals of the store (list, queue)
  // my_store.motor_speed = 1;        // set initial state
  // my_store.current_elevation = 0;

  // // register reducers
  // cedux_register_reducer_my_store(&my_store, handle_new_elevation);
  // cedux_register_reducer_my_store(&my_store, handle_new_desired_elevation);
}

void _runStore(){
  bool did_work = cedux_run_my_store(&my_store); // REQUIRED. You must call this every run of your main loop. This is where all the magic happens.
  if (did_work) {
    printf("A reducer did work.\n");
  }
}

int runs = 0;
void _main() {
  _runStore();

  if (runs < 6 && runs < 3) {
    altimeter_isr();
  }
  if (runs < 6 && runs > 3) {
    up_button_isr();
  }
  runs++;
}



/* running out of space when including 

NOTHING
Linking .pio/build/nanoatmega328/firmware.elf
Checking size .pio/build/nanoatmega328/firmware.elf
Building .pio/build/nanoatmega328/firmware.hex
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [========= ]  89.3% (used 1828 bytes from 2048 bytes)
PROGRAM: [=====     ]  46.9% (used 14404 bytes from 30720 bytes)


#include <stdio.h>
#include "printf.h"
Building .pio/build/nanoatmega328/firmware.hex
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [========= ]  94.6% (used 1938 bytes from 2048 bytes)
PROGRAM: [=====     ]  52.2% (used 16028 bytes from 30720 bytes)
Configuring upload protocol...
AVAILABLE: arduino


// #include "cedux.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
// #include <Arduino.h>
#include "printf.h"
...
Linking .pio/build/nanoatmega328/firmware.elf
Checking size .pio/build/nanoatmega328/firmware.elf
Building .pio/build/nanoatmega328/firmware.hex
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [========= ]  94.7% (used 1939 bytes from 2048 bytes)
PROGRAM: [=====     ]  52.2% (used 16028 bytes from 30720 bytes)


#include "cedux.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "printf.h"
...
and all src above: 

Linking .pio/build/nanoatmega328/firmware.elf
Checking size .pio/build/nanoatmega328/firmware.elf
Building .pio/build/nanoatmega328/firmware.hex
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [==========]  114.7% (used 2349 bytes from 2048 bytes)
PROGRAM: [=====     ]  54.9% (used 16880 bytes from 30720 bytes)
...
uploaded 16880 bytes

*/
