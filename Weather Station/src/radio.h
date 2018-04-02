#ifndef __RADIO__
#define __RADIO__

#include "main.h"
#include <RF24.h>

namespace radio {
RF24 radio(6, 10);

void begin();
}

#endif