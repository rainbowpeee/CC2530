#ifndef NEWPROCESSAPP_H
#define NEWPROCESSAPP_H

#include "ZComDef.h"

#define NEWPROCESSAPP_ENDPOINT           20

#define NEWPROCESSAPP_PROFID             0x0F08
#define NEWPROCESSAPP_DEVICEID           0x0001
#define NEWPROCESSAPP_DEVICE_VERSION     0
#define NEWPROCESSAPP_FLAGS              0

#define NEWPROCESSAPP_MAX_CLUSTERS       2
#define NEWPROCESSAPP_PERIODIC_CLUSTERID 1

extern void NewProcessApp_Init( uint8 task_id );
extern UINT16 NewProcessApp_ProcessEvent( uint8 task_id, uint16 events );

#endif