#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "System.h"
#include "Bluetooth.h"
#include "ProcessSystemData.h"
#include "ProcessInterrupterData.h"

void stopConnectEventTimer();
void cleareConterConnectTimer();

void processConnection();
void processConnectionEventTimer();

#endif // _CONNECTION_H