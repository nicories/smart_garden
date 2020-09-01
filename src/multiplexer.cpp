#include <ESP8266WiFi.h>
#include <multiplexer.h>

int Multiplexer::read(int index) {
    digitalWrite(controls[0], index & 0b01);
    digitalWrite(controls[1], index & 0b10);

    return analogRead(input);
}
