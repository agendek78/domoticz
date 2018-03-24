#pragma once

#include <stdint.h>

#define ZDO_EP  0   // reserved for ZDO
#define NM_EP   1   // reserved for NWK Server
#define GW_EP   2   // reserved for Gateway Server
#define APP_EP  3   // defines the first endpoint for the application
#define APP_MAX_EP  254 // maximum endpoint ID

typedef uint16_t  cId_t;

// Simple Description Format Structure
typedef struct
{
	uint8_t        EndPoint;
	uint16_t       AppProfId;
	uint16_t       AppDeviceId;
	uint8_t        AppDevVer;
	uint8_t        AppNumInClusters;
	cId_t          *pAppInClusterList;
	uint8_t        AppNumOutClusters;
	cId_t          *pAppOutClusterList;
} SimpleDescriptionFormat_t;

typedef enum
{
	noLatencyReqs,
	fastBeacons,
	slowBeacons
} afNetworkLatencyReq_t;

typedef struct
{
	SimpleDescriptionFormat_t *simpleDesc;
	afNetworkLatencyReq_t latencyReq;
} endPointDesc_t;
