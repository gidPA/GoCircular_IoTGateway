#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include <time.h>

int testNTPClient();
void getCurrentTimeObject(tm *timeObject);
void createSQLTimestamp(tm *timeObject, char* dateStringBuffer, byte dateStringSize);


#endif // TIME_HANDLER_H