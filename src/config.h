#ifndef CONFIG_H
#define CONFIG_H

// All that matters to a 3pin WS* strand
#define DATAPIN 4
// Not applicable to 4 wire APA102 type strands
#define CLOCKPIN 5

#define STRIP WS2811
const int NUMPIXELS = 180;

#endif 