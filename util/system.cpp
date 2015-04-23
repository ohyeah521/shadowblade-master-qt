#include "system.h"

time_t Util::System::getTime()
{
    return ::time(NULL)*1000;
}

