#include <stdint.h>
#include <NRFLite.h>
#include <SPI.h>

const static uint8_t RADIO_ID = 1;       // Our radio's id.
const static uint8_t PIN_RADIO_CE = 9;   // hardware pins
const static uint8_t PIN_RADIO_CSN = 10; // hardware pins

#define TAGID 0x426C6973 // tag code is 'Blis'
#define TAGID_ALT 0x426c697a

NRFLite _radio;

char STROBEPACKET = 0;

// macros for setting and getting the packet type and device IDs
#define SENDPERIOD 2000000 // broadcast period in microseconds
#define MUTEPERIOD (SENDPERIOD * 3)
#define getTag(data) get32(data + 0)
#define getID(data) get16(data + 4)

#define putTag(data, value) put32(data + 0, value)
#define putID(data, value) put16(data + 4, value)

uint8_t current[32];      // anim state variables
bool radioAlive = false;  // true if radio booted up
unsigned long timeToSend; // how long till next send;
unsigned long muteTimer;
int brightness;
int program = 0;        // which application is running in loop
unsigned long lastTime; // for calculation delta time
char radioEvent = 0;    // what the radio

void setupRadio()
{
    pinMode(14, INPUT_PULLUP);
    putID(current, 65535);
    putID(current, random(65536));
    putTag(current, TAGID);
    SPI.begin();
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        radioAlive = false;
        Serial.println("radio fail");
    }
    else
    {
        radioAlive = true;
        Serial.println("radio ok");
    }
    timeToSend = 0;
    muteTimer = 0; // lets listen before we start talking
}
void checkRadioReceive()
{
    uint8_t incoming[sizeof(current)];
    // check for incoming data
    // Serial.println('check for date')
    while (_radio.hasData())
    {
        _radio.readData(&incoming);

        // mac
        // incoming is array of uint8_t

        Serial.println("getTag: ");
        Serial.println(getTag(incoming));
        Serial.println("-----------------------");

        // if (getTag(incoming) == TAGID_ALT) {
        // FIXME what is this tag value? 'incoming' includes front-padding?
        if (getTag(incoming) == 0x6f740064)
        { // 'otd' in ascii
            Serial.print("got strobe packet");
            STROBEPACKET = 1;
            continue;
        }
        // endmac

        unsigned long sum = crc(incoming, 30);
        if (sum != (incoming[30] + (incoming[31] * 256))) // is the packet corrupt?
            continue;

        uint32_t tag = getTag(incoming);
        Serial.println("got");

        if (tag != TAGID) // is it our packet type?
            continue;

        uint16_t unitId = getID(incoming);
        uint16_t currentId = getID(current);

        Serial.print("currentId: ");
        Serial.println(currentId);
        Serial.print("from unitID: ");
        Serial.println(unitId);

#ifndef SUPERFOLLOWER
        if (unitId < currentId) // it was lower rank than me FOLLOWER, ignore it
            continue;
        if (unitId > currentId) // it was higher rank than me, get the data
#endif
        {
            radioEvent = 1;

            // we dont copy the 6 byte header, or the 2 byte checksum
            memcpy(current + 6, incoming + 6, sizeof(current) - 8); // copy the incomming data into my data
            muteTimer = 0;                                          // im listening, so I keep quiet
            continue;
        }

        putID(current, random(65536)); // we had the same ID, pick a new one
    }
}

void checkRadioSend(int deltaTime)
{
#ifdef SUPERFOLLOWER // superFOLLOWER is always mute
    return;
#endif
    // check mute timer
    if (muteTimer < MUTEPERIOD)
    {
        muteTimer += deltaTime;
        return;
    }
    // run periodic timer
    timeToSend += deltaTime;
    if (timeToSend < SENDPERIOD)
        return;
    timeToSend = 0;

    unsigned long sum = crc(current, 30);
    current[30] = sum & 255;
    current[31] = sum >> 8;

    if (_radio.send(RADIO_ID, &current, sizeof(current), NRFLite::NO_ACK))
    {
        radioEvent = 2; // for display debug
    }
    else
    {
        timeToSend += getID(current) & 255; // might have been a collision, wait a little longer, based on my ID
    }
}

unsigned long crc(const byte *data, byte len)
{

    const unsigned long crc_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c};

    unsigned long crc = ~0L;

    for (unsigned int index = 0; index < len; ++index)
    {
        crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
        crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
        crc = ~crc;
    }
    return crc & 65535;
}

uint32_t get32(uint8_t *data)
{
    uint32_t v;
    v = data[0];
    v <<= 8;
    v += data[1];
    v <<= 8;
    v += data[2];
    v <<= 8;
    v += data[3];
    return v;
}
void put32(uint8_t *data, uint32_t value)
{
    data[3] = (value >> 0) & 255;
    data[2] = (value >> 8) & 255;
    data[1] = (value >> 16) & 255;
    data[0] = (value >> 24) & 255;
}
uint16_t get16(uint8_t *data)
{
    uint32_t v;
    v = data[0];
    v <<= 8;
    v += data[1];
    return v;
}
void put16(uint8_t *data, uint16_t value)
{
    data[1] = (value >> 0) & 255;
    data[0] = (value >> 8) & 255;
}