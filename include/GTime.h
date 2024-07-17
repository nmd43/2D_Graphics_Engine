/**
 *  Copyright 2013 Mike Reed
 */

#ifndef GTime_DEFINED
#define GTime_DEFINED

#include "GTypes.h"

using GMSec = unsigned long;

class GTime {
public:
    static GMSec GetMSec();
};

#endif
