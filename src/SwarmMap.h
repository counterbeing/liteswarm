///////////////////////////////////////////////////////////////////////////////////
// trying to use `map` implementation from `ustd` lib for arduino / atmega, but...
// it compiles OK but intellisense shows errors
//
// lightweight map implementations:
// - https://github.com/muwerk/ustd
// - another implementation https://attractivechaos.wordpress.com/2009/09/29/khash-h/
//
//
// vscode c/c++ intellisense is/was throwing errors claiming it cant find deps 
// included by `ustd` package... but the include preprocessor directives causing 
// are problems conditionally dependant on __APPLE__ or __LINUX__ being defined. 
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
#include <map.h>

ustd::map<int, float> mayMap = ustd::map<int,float>(5, 5, 0, false);