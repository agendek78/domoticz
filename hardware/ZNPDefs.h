#pragma once

/***************************************************************************************************
* CONSTANTS
***************************************************************************************************/
//Special definitions for ZTOOL (Zigbee 0.7 release)
#define ZTEST_DEFAULT_PARAM_LEN              0x10  //( 16 Bytes)
#define ZTEST_DEFAULT_ADDR_LEN               0x08  //(  8 Bytes)
#define ZTEST_DEFAULT_DATA_LEN               0x75  //(117 Bytes)
#define ZTEST_DEFAULT_AF_DATA_LEN            0x20  //( 32 Bytes) - AF
#define ZTEST_DEFAULT_SEC_LEN                0x0B
#define ZTEST_DEFAULT_SEC_PARAM_LEN          0x1C  //( 28 Bytes)

/***************************************************************************************************
* SYS COMMANDS
***************************************************************************************************/

/* AREQ from host */
#define MT_SYS_RESET_REQ                     0x00

/* SREQ/SRSP */
#define MT_SYS_PING                          0x01
#define MT_SYS_VERSION                       0x02
#define MT_SYS_SET_EXTADDR                   0x03
#define MT_SYS_GET_EXTADDR                   0x04
#define MT_SYS_RAM_READ                      0x05
#define MT_SYS_RAM_WRITE                     0x06
#define MT_SYS_OSAL_NV_ITEM_INIT             0x07
#define MT_SYS_OSAL_NV_READ                  0x08
#define MT_SYS_OSAL_NV_WRITE                 0x09
#define MT_SYS_OSAL_START_TIMER              0x0A
#define MT_SYS_OSAL_STOP_TIMER               0x0B
#define MT_SYS_RANDOM                        0x0C
#define MT_SYS_ADC_READ                      0x0D
#define MT_SYS_GPIO                          0x0E
#define MT_SYS_STACK_TUNE                    0x0F
#define MT_SYS_SET_TIME                      0x10
#define MT_SYS_GET_TIME                      0x11
#define MT_SYS_OSAL_NV_DELETE                0x12
#define MT_SYS_OSAL_NV_LENGTH                0x13
#define MT_SYS_SET_TX_POWER                  0x14
#define MT_SYS_JAMMER_PARAMETERS             0x15
#define MT_SYS_SNIFFER_PARAMETERS            0x16
#define MT_SYS_ZDIAGS_INIT_STATS             0x17
#define MT_SYS_ZDIAGS_CLEAR_STATS            0x18
#define MT_SYS_ZDIAGS_GET_STATS              0x19
#define MT_SYS_ZDIAGS_RESTORE_STATS_NV       0x1A
#define MT_SYS_ZDIAGS_SAVE_STATS_TO_NV       0x1B
#define MT_SYS_OSAL_NV_READ_EXT              0x1C
#define MT_SYS_OSAL_NV_WRITE_EXT             0x1D

/* Extended Non-Vloatile Memory */
#define MT_SYS_NV_CREATE                     0x30
#define MT_SYS_NV_DELETE                     0x31
#define MT_SYS_NV_LENGTH                     0x32
#define MT_SYS_NV_READ                       0x33
#define MT_SYS_NV_WRITE                      0x34
#define MT_SYS_NV_UPDATE                     0x35
#define MT_SYS_NV_COMPACT                    0x36

/* AREQ to host */
#define MT_SYS_RESET_IND                     0x80
#define MT_SYS_OSAL_TIMER_EXPIRED            0x81
#define MT_SYS_JAMMER_IND                    0x82


#define MT_SYS_RESET_HARD     0
#define MT_SYS_RESET_SOFT     1
#define MT_SYS_RESET_SHUTDOWN 2

#define MT_SYS_SNIFFER_DISABLE       0
#define MT_SYS_SNIFFER_ENABLE        1
#define MT_SYS_SNIFFER_GET_SETTING   2

/***************************************************************************************************
* MAC COMMANDS
***************************************************************************************************/
/* SREQ/SRSP */
#define MT_MAC_RESET_REQ                     0x01
#define MT_MAC_INIT                          0x02
#define MT_MAC_START_REQ                     0x03
#define MT_MAC_SYNC_REQ                      0x04
#define MT_MAC_DATA_REQ                      0x05
#define MT_MAC_ASSOCIATE_REQ                 0x06
#define MT_MAC_DISASSOCIATE_REQ              0x07
#define MT_MAC_GET_REQ                       0x08
#define MT_MAC_SET_REQ                       0x09
#define MT_MAC_GTS_REQ                       0x0a
#define MT_MAC_RX_ENABLE_REQ                 0x0b
#define MT_MAC_SCAN_REQ                      0x0c
#define MT_MAC_POLL_REQ                      0x0d
#define MT_MAC_PURGE_REQ                     0x0e
#define MT_MAC_SET_RX_GAIN_REQ               0x0f

/* Enhanced Active Scan request */
#define MT_MAC_ENHANCED_ACTIVE_SCAN_REQ      0x12
#define MT_MAC_ENHANCED_ACTIVE_SCAN_RSP      0x13

/* Enhanced MAC interface SREQ/SRSP */
#define MT_MAC_SRC_MATCH_ENABLE              0x14
#define MT_MAC_SRC_MATCH_ADD_ENTRY           0x15
#define MT_MAC_SRC_MATCH_DELETE_ENTRY        0x16
#define MT_MAC_SRC_MATCH_ACK_ALL             0x17
#define MT_MAC_SRC_CHECK_ALL                 0x18

/* Security PIB SREQ/SRSP */
#define MT_MAC_SECURITY_GET_REQ              0x30
#define MT_MAC_SECURITY_SET_REQ              0x31
#define MT_MAC_DELETE_DEVICE_REQ             0x32
#define MT_MAC_READ_KEY_WITH_ID_REQ          0x33
#define MT_MAC_WRITE_KEY_WITH_ID_REQ         0x34
#define MT_MAC_ADD_DEVICE_REQ                0x35
#define MT_MAC_DELETE_ALL_DEVICES_REQ        0x36
#define MT_MAC_UPDATE_PAN_ID                 0x37

/* AREQ from Host */
#define MT_MAC_ASSOCIATE_RSP                 0x50
#define MT_MAC_ORPHAN_RSP                    0x51

/* AREQ to host */
#define MT_MAC_SYNC_LOSS_IND                 0x80
#define MT_MAC_ASSOCIATE_IND                 0x81
#define MT_MAC_ASSOCIATE_CNF                 0x82
#define MT_MAC_BEACON_NOTIFY_IND             0x83
#define MT_MAC_DATA_CNF                      0x84
#define MT_MAC_DATA_IND                      0x85
#define MT_MAC_DISASSOCIATE_IND              0x86
#define MT_MAC_DISASSOCIATE_CNF              0x87
#define MT_MAC_GTS_CNF                       0x88
#define MT_MAC_GTS_IND                       0x89
#define MT_MAC_ORPHAN_IND                    0x8a
#define MT_MAC_POLL_CNF                      0x8b
#define MT_MAC_SCAN_CNF                      0x8c
#define MT_MAC_COMM_STATUS_IND               0x8d
#define MT_MAC_START_CNF                     0x8e
#define MT_MAC_RX_ENABLE_CNF                 0x8f
#define MT_MAC_PURGE_CNF                     0x90
#define MT_MAC_POLL_IND                      0x91

/***************************************************************************************************
* NWK COMMANDS
***************************************************************************************************/

/* AREQ from host */
#define MT_NWK_INIT                          0x00

/* SREQ/SRSP */
#define MT_NLDE_DATA_REQ                     0x01
#define MT_NLME_NETWORK_FORMATION_REQ        0x02
#define MT_NLME_PERMIT_JOINING_REQ           0x03
#define MT_NLME_JOIN_REQ                     0x04
#define MT_NLME_LEAVE_REQ                    0x05
#define MT_NLME_RESET_REQ                    0x06
#define MT_NLME_GET_REQ                      0x07
#define MT_NLME_SET_REQ                      0x08
#define MT_NLME_NETWORK_DISCOVERY_REQ        0x09
#define MT_NLME_ROUTE_DISCOVERY_REQ          0x0A
#define MT_NLME_DIRECT_JOIN_REQ              0x0B
#define MT_NLME_ORPHAN_JOIN_REQ              0x0C
#define MT_NLME_START_ROUTER_REQ             0x0D

/* AREQ to host */
#define MT_NLDE_DATA_CONF                    0x80
#define MT_NLDE_DATA_IND                     0x81
#define MT_NLME_NETWORK_FORMATION_CONF       0x82
#define MT_NLME_JOIN_CONF                    0x83
#define MT_NLME_JOIN_IND                     0x84
#define MT_NLME_LEAVE_CONF                   0x85
#define MT_NLME_LEAVE_IND                    0x86
#define MT_NLME_POLL_CONF                    0x87
#define MT_NLME_SYNC_IND                     0x88
#define MT_NLME_NETWORK_DISCOVERY_CONF       0x89
#define MT_NLME_START_ROUTER_CONF            0x8A

/***************************************************************************************************
* AF COMMANDS
***************************************************************************************************/

/* SREQ/SRSP */
#define MT_AF_REGISTER                       0x00
#define MT_AF_DATA_REQUEST                   0x01  /* AREQ optional, but no AREQ response. */
#define MT_AF_DATA_REQUEST_EXT               0x02  /* AREQ optional, but no AREQ response. */
#define MT_AF_DATA_REQUEST_SRCRTG            0x03
#define MT_AF_DELETE                         0x04

#define MT_AF_INTER_PAN_CTL                  0x10
#define MT_AF_DATA_STORE                     0x11
#define MT_AF_DATA_RETRIEVE                  0x12
#define MT_AF_APSF_CONFIG_SET                0x13
#define MT_AF_APSF_CONFIG_GET                0x14

/* AREQ to host */
#define MT_AF_DATA_CONFIRM                   0x80
#define MT_AF_INCOMING_MSG                   0x81
#define MT_AF_INCOMING_MSG_EXT               0x82
#define MT_AF_REFLECT_ERROR                  0x83

/***************************************************************************************************
* ZDO COMMANDS
***************************************************************************************************/

/* SREQ/SRSP */
#define MT_ZDO_NWK_ADDR_REQ                  0x00
#define MT_ZDO_IEEE_ADDR_REQ                 0x01
#define MT_ZDO_NODE_DESC_REQ                 0x02
#define MT_ZDO_POWER_DESC_REQ                0x03
#define MT_ZDO_SIMPLE_DESC_REQ               0x04
#define MT_ZDO_ACTIVE_EP_REQ                 0x05
#define MT_ZDO_MATCH_DESC_REQ                0x06
#define MT_ZDO_COMPLEX_DESC_REQ              0x07
#define MT_ZDO_USER_DESC_REQ                 0x08
#define MT_ZDO_END_DEV_ANNCE                 0x0A
#define MT_ZDO_USER_DESC_SET                 0x0B
#define MT_ZDO_SERVICE_DISC_REQ              0x0C
#define MT_ZDO_END_DEV_BIND_REQ              0x20
#define MT_ZDO_BIND_REQ                      0x21
#define MT_ZDO_UNBIND_REQ                    0x22

#define MT_ZDO_SET_LINK_KEY                  0x23
#define MT_ZDO_REMOVE_LINK_KEY               0x24
#define MT_ZDO_GET_LINK_KEY                  0x25
#define MT_ZDO_NWK_DISCOVERY_REQ             0x26
#define MT_ZDO_JOIN_REQ                      0x27
#define MT_ZDO_SEND_DATA                     0x28
#define MT_ZDO_NWK_ADDR_OF_INTEREST_REQ      0x29

#define MT_ZDO_MGMT_NWKDISC_REQ              0x30
#define MT_ZDO_MGMT_LQI_REQ                  0x31
#define MT_ZDO_MGMT_RTG_REQ                  0x32
#define MT_ZDO_MGMT_BIND_REQ                 0x33
#define MT_ZDO_MGMT_LEAVE_REQ                0x34
#define MT_ZDO_MGMT_DIRECT_JOIN_REQ          0x35
#define MT_ZDO_MGMT_PERMIT_JOIN_REQ          0x36
#define MT_ZDO_MGMT_NWK_UPDATE_REQ           0x37

/* AREQ optional, but no AREQ response. */
#define MT_ZDO_MSG_CB_REGISTER               0x3E
#define MT_ZDO_MSG_CB_REMOVE                 0x3F
#define MT_ZDO_STARTUP_FROM_APP              0x40

/* AREQ from host */
#define MT_ZDO_AUTO_FIND_DESTINATION_REQ     0x41
#define MT_ZDO_SEC_ADD_LINK_KEY              0x42
#define MT_ZDO_SEC_ENTRY_LOOKUP_EXT          0x43
#define MT_ZDO_SEC_DEVICE_REMOVE             0x44
#define MT_ZDO_EXT_ROUTE_DISC                0x45
#define MT_ZDO_EXT_ROUTE_CHECK               0x46
#define MT_ZDO_EXT_REMOVE_GROUP              0x47
#define MT_ZDO_EXT_REMOVE_ALL_GROUP          0x48
#define MT_ZDO_EXT_FIND_ALL_GROUPS_ENDPOINT  0x49
#define MT_ZDO_EXT_FIND_GROUP                0x4A
#define MT_ZDO_EXT_ADD_GROUP                 0x4B
#define MT_ZDO_EXT_COUNT_ALL_GROUPS          0x4C
#define MT_ZDO_EXT_RX_IDLE                   0x4D
#define MT_ZDO_EXT_UPDATE_NWK_KEY            0x4E
#define MT_ZDO_EXT_SWITCH_NWK_KEY            0x4F
#define MT_ZDO_EXT_NWK_INFO                  0x50
#define MT_ZDO_EXT_SEC_APS_REMOVE_REQ        0x51
#define MT_ZDO_FORCE_CONCENTRATOR_CHANGE     0x52
#define MT_ZDO_EXT_SET_PARAMS                0x53
#define MT_ZDO_STARTUP_FROM_APP_EX           0x54

/* AREQ to host */
#define MT_ZDO_AREQ_TO_HOST                  0x80 /* Mark the start of the ZDO CId AREQs to host. */
#define MT_ZDO_NWK_ADDR_RSP               /* 0x80 */ ((uint8_t)NWK_addr_req | 0x80)
#define MT_ZDO_IEEE_ADDR_RSP              /* 0x81 */ ((uint8_t)IEEE_addr_req | 0x80)
#define MT_ZDO_NODE_DESC_RSP              /* 0x82 */ ((uint8_t)Node_Desc_req | 0x80)
#define MT_ZDO_POWER_DESC_RSP             /* 0x83 */ ((uint8_t)Power_Desc_req | 0x80)
#define MT_ZDO_SIMPLE_DESC_RSP            /* 0x84 */ ((uint8_t)Simple_Desc_req | 0x80)
#define MT_ZDO_ACTIVE_EP_RSP              /* 0x85 */ ((uint8_t)Active_EP_req | 0x80)
#define MT_ZDO_MATCH_DESC_RSP             /* 0x86 */ ((uint8_t)Match_Desc_req | 0x80)

#define MT_ZDO_COMPLEX_DESC_RSP              0x87
#define MT_ZDO_USER_DESC_RSP                 0x88
//                                        /* 0x92 */ ((uint8_t)Discovery_Cache_req | 0x80)
#define MT_ZDO_USER_DESC_CONF                0x89
#define MT_ZDO_SERVER_DISC_RSP               0x8A

#define MT_ZDO_END_DEVICE_BIND_RSP        /* 0xA0 */ ((uint8_t)End_Device_Bind_req | 0x80)
#define MT_ZDO_BIND_RSP                   /* 0xA1 */ ((uint8_t)Bind_req | 0x80)
#define MT_ZDO_UNBIND_RSP                 /* 0xA2 */ ((uint8_t)Unbind_req | 0x80)

#define MT_ZDO_MGMT_NWK_DISC_RSP          /* 0xB0 */ ((uint8_t)Mgmt_NWK_Disc_req | 0x80)
#define MT_ZDO_MGMT_LQI_RSP               /* 0xB1 */ ((uint8_t)Mgmt_Lqi_req | 0x80)
#define MT_ZDO_MGMT_RTG_RSP               /* 0xB2 */ ((uint8_t)Mgmt_Rtg_req | 0x80)
#define MT_ZDO_MGMT_BIND_RSP              /* 0xB3 */ ((uint8_t)Mgmt_Bind_req | 0x80)
#define MT_ZDO_MGMT_LEAVE_RSP             /* 0xB4 */ ((uint8_t)Mgmt_Leave_req | 0x80)
#define MT_ZDO_MGMT_DIRECT_JOIN_RSP       /* 0xB5 */ ((uint8_t)Mgmt_Direct_Join_req | 0x80)
#define MT_ZDO_MGMT_PERMIT_JOIN_RSP       /* 0xB6 */ ((uint8_t)Mgmt_Permit_Join_req | 0x80)

//                                        /* 0xB8 */ ((uint8_t)Mgmt_NWK_Update_req | 0x80)

#define MT_ZDO_STATE_CHANGE_IND              0xC0
#define MT_ZDO_END_DEVICE_ANNCE_IND          0xC1
#define MT_ZDO_MATCH_DESC_RSP_SENT           0xC2
#define MT_ZDO_STATUS_ERROR_RSP              0xC3
#define MT_ZDO_SRC_RTG_IND                   0xC4
#define MT_ZDO_BEACON_NOTIFY_IND             0xC5
#define MT_ZDO_JOIN_CNF                      0xC6
#define MT_ZDO_NWK_DISCOVERY_CNF             0xC7
#define MT_ZDO_CONCENTRATOR_IND_CB           0xC8
#define MT_ZDO_LEAVE_IND                     0xC9
#define MT_ZDO_TC_DEVICE_IND                 0xCA
#define MT_ZDO_PERMIT_JOIN_IND               0xCB
#define MT_ZDO_SET_REJOIN_PARAMS             0xCC

#define MT_ZDO_MSG_CB_INCOMING               0xFF

// Some arbitrarily chosen value for a default error status msg.
#define MtZdoDef_rsp                         0x0040

/***************************************************************************************************
* SAPI COMMANDS
***************************************************************************************************/

// SAPI MT Command Identifiers
/* AREQ from Host */
#define MT_SAPI_SYS_RESET                   0x09

/* SREQ/SRSP */
#define MT_SAPI_START_REQ                   0x00
#define MT_SAPI_BIND_DEVICE_REQ             0x01
#define MT_SAPI_ALLOW_BIND_REQ              0x02
#define MT_SAPI_SEND_DATA_REQ               0x03
#define MT_SAPI_READ_CFG_REQ                0x04
#define MT_SAPI_WRITE_CFG_REQ               0x05
#define MT_SAPI_GET_DEV_INFO_REQ            0x06
#define MT_SAPI_FIND_DEV_REQ                0x07
#define MT_SAPI_PMT_JOIN_REQ                0x08
#define MT_SAPI_APP_REGISTER_REQ            0x0a

/* AREQ to host */
#define MT_SAPI_START_CNF                   0x80
#define MT_SAPI_BIND_CNF                    0x81
#define MT_SAPI_ALLOW_BIND_CNF              0x82
#define MT_SAPI_SEND_DATA_CNF               0x83
#define MT_SAPI_READ_CFG_RSP                0x84
#define MT_SAPI_FIND_DEV_CNF                0x85
#define MT_SAPI_DEV_INFO_RSP                0x86
#define MT_SAPI_RCV_DATA_IND                0x87

/***************************************************************************************************
* UTIL COMMANDS
***************************************************************************************************/

/* SREQ/SRSP: */
#define MT_UTIL_GET_DEVICE_INFO              0x00
#define MT_UTIL_GET_NV_INFO                  0x01
#define MT_UTIL_SET_PANID                    0x02
#define MT_UTIL_SET_CHANNELS                 0x03
#define MT_UTIL_SET_SECLEVEL                 0x04
#define MT_UTIL_SET_PRECFGKEY                0x05
#define MT_UTIL_CALLBACK_SUB_CMD             0x06
#define MT_UTIL_KEY_EVENT                    0x07
#define MT_UTIL_TIME_ALIVE                   0x09
#define MT_UTIL_LED_CONTROL                  0x0A

#define MT_UTIL_TEST_LOOPBACK                0x10
#define MT_UTIL_DATA_REQ                     0x11

#define MT_UTIL_GPIO_SET_DIRECTION           0x14
#define MT_UTIL_GPIO_READ                    0x15
#define MT_UTIL_GPIO_WRITE                   0x16

#define MT_UTIL_SRC_MATCH_ENABLE             0x20
#define MT_UTIL_SRC_MATCH_ADD_ENTRY          0x21
#define MT_UTIL_SRC_MATCH_DEL_ENTRY          0x22
#define MT_UTIL_SRC_MATCH_CHECK_SRC_ADDR     0x23
#define MT_UTIL_SRC_MATCH_ACK_ALL_PENDING    0x24
#define MT_UTIL_SRC_MATCH_CHECK_ALL_PENDING  0x25

#define MT_UTIL_ADDRMGR_EXT_ADDR_LOOKUP      0x40
#define MT_UTIL_ADDRMGR_NWK_ADDR_LOOKUP      0x41
#define MT_UTIL_APSME_LINK_KEY_DATA_GET      0x44
#define MT_UTIL_APSME_LINK_KEY_NV_ID_GET     0x45
#define MT_UTIL_ASSOC_COUNT                  0x48
#define MT_UTIL_ASSOC_FIND_DEVICE            0x49
#define MT_UTIL_ASSOC_GET_WITH_ADDRESS       0x4A
#define MT_UTIL_APSME_REQUEST_KEY_CMD        0x4B
#ifdef MT_SRNG
#define MT_UTIL_SRNG_GENERATE                0x4C
#endif
#define MT_UTIL_BIND_ADD_ENTRY               0x4D

#define MT_UTIL_ZCL_KEY_EST_INIT_EST         0x80
#define MT_UTIL_ZCL_KEY_EST_SIGN             0x81

/* AREQ from/to host */
#define MT_UTIL_SYNC_REQ                     0xE0
#define MT_UTIL_ZCL_KEY_ESTABLISH_IND        0xE1
#ifdef FEATURE_GET_PRIMARY_IEEE
#define MT_UTIL_GET_PRIMARY_IEEE             0xEF
#endif

/***************************************************************************************************
* DEBUG COMMANDS
***************************************************************************************************/

/* SREQ/SRSP: */
#define MT_DEBUG_SET_THRESHOLD               0x00
#define MT_DEBUG_MAC_DATA_DUMP               0x10

//TP2 commands  
#define MT_DEBUG_TP2_ENABLEAPSSECURITY       0x01
#define MT_DEBUG_TP2_SET_NODE_R20            0x02

/* AREQ */
#define MT_DEBUG_MSG                         0x80



/***************************************************************************************************
* APP CONFIG COMMANDS
***************************************************************************************************/

#define MT_APP_CNF_SET_DEFAULT_REMOTE_ENDDEVICE_TIMEOUT    0x01
#define MT_APP_CNF_SET_ENDDEVICETIMEOUT                    0x02
#define MT_APP_CNF_SET_ALLOWREJOIN_TC_POLICY               0x03
#define MT_APP_CNF_BDB_ADD_INSTALLCODE                     0x04
#define MT_APP_CNF_BDB_START_COMMISSIONING                 0x05
#define MT_APP_CNF_BDB_SET_JOINUSESINSTALLCODEKEY          0x06
#define MT_APP_CNF_BDB_SET_ACTIVE_DEFAULT_CENTRALIZED_KEY  0x07
#define MT_APP_CNF_BDB_SET_CHANNEL                         0x08
#define MT_APP_CNF_BDB_SET_TC_REQUIRE_KEY_EXCHANGE         0x09    
#define MT_APP_CNF_BDB_ZED_ATTEMPT_RECOVER_NWK             0x0A

#define MT_APP_CNF_BDB_COMMISSIONING_NOTIFICATION          0x80    
//Application debug commands
#define MT_APP_CNF_SET_NWK_FRAME_COUNTER                   0xFF  


/***************************************************************************************************
* GP COMMANDS
***************************************************************************************************/
//From Host to ZNP
#define MT_GP_DATA_REQ                       0x01
#define MT_GP_SEC_RSP                        0x02

//From ZNP to Host
#define MT_GP_SEC_REQ                        0x03
#define MT_GP_DATA_IND                       0x04
#define MT_GP_DATA_CNF                       0x05



/***************************************************************************************************
* APP COMMANDS
***************************************************************************************************/

/* SREQ/SRSP: */
#define MT_APP_MSG                           0x00
#define MT_APP_USER_TEST                     0x01
#define MT_APP_PB_ZCL_MSG                    0x02
#define MT_APP_PB_ZCL_CFG                    0x03

/* SRSP */
#define MT_APP_RSP                           0x80
#define MT_APP_TOUCHLINK_TL_IND              0x81
#define MT_APP_PB_ZCL_IND                    0x82

/***************************************************************************************************
* FILE SYSTEM COMMANDS
***************************************************************************************************/
#define MT_OTA_FILE_READ_REQ                  0x00
#define MT_OTA_NEXT_IMG_REQ                   0x01

#define MT_OTA_FILE_READ_RSP                  0x80
#define MT_OTA_NEXT_IMG_RSP                   0x81
#define MT_OTA_STATUS_IND                     0x82

/***************************************************************************************************
* UBL COMMANDS
***************************************************************************************************/

/* SREQ/SRSP: */

//efine SB_TGT_BOOTLOAD                       0x10 // Optional command in Target Application.
#define MT_UBL_ERASE                          0x10

#define MT_UBL_WRITE                          0x11
#define MT_UBL_READ                           0x12
#define MT_UBL_ENABLE                         0x13
#define MT_UBL_HANDSHAKE                      0x14
#define MT_UBL_FORCE                          0x15

/***************************************************************************************************
* ZNP COMMANDS
***************************************************************************************************/

#define MT_ZNP_BASIC_CFG                      0x00
#define MT_ZNP_ZCL_CFG                        0x10
#define MT_ZNP_SE_CFG                         0x20

/***************************************************************************************************
* ZNP RESPONSES
***************************************************************************************************/

#define MT_ZNP_BASIC_RSP                      0x80
#define MT_ZNP_ZCL_RSP                        0x90
#define MT_ZNP_SE_RSP                         0xA0


// RPC Command Field Type
#define RPC_CMD_POLL        0x00
#define RPC_CMD_SREQ        0x20
#define RPC_CMD_AREQ        0x40
#define RPC_CMD_SRSP        0x60
#define RPC_CMD_RES4        0x80
#define RPC_CMD_RES5        0xA0
#define RPC_CMD_RES6        0xC0
#define RPC_CMD_RES7        0xE0

// RPC Command Field Subsystem
#define RPC_SYS_RES0        0
#define RPC_SYS_SYS         1
#define RPC_SYS_MAC         2
#define RPC_SYS_NWK         3
#define RPC_SYS_AF          4
#define RPC_SYS_ZDO         5
#define RPC_SYS_SAPI        6    // Simple API
#define RPC_SYS_UTIL        7
#define RPC_SYS_DBG         8
#define RPC_SYS_APP         9
#define RPC_SYS_RCAF        10   // Remote Control Application Framework
#define RPC_SYS_RCN         11   // Remote Control Network Layer
#define RPC_SYS_RCN_CLIENT  12   // Remote Control Network Layer Client
#define RPC_SYS_BOOT        13   // Serial Bootloader
#define RPC_SYS_DEBUG	    14   // Debug Interface for Flash Programming
#define RPC_SYS_SRV_CTRL	15   // Interface to control Server
#define RPC_SYS_CAPSENSE    16   // CapSense callback
#define RPC_SYS_NFC    		17   // NFC
#define RPC_SYS_MAX         18   // Maximum value, must be last

/* The 3 MSB's of the 1st command field byte are for command type. */
#define RPC_CMD_TYPE_MASK   0xE0

/* The 5 LSB's of the 1st command field byte are for the subsystem. */
#define RPC_SUBSYSTEM_MASK  0x1F

/* Error codes */
#define RPC_SUCCESS         0     /* success */
#define RPC_ERR_SUBSYSTEM   1     /* invalid subsystem */
#define RPC_ERR_COMMAND_ID  2     /* invalid command ID */
#define RPC_ERR_PARAMETER   3     /* invalid parameter */
#define RPC_ERR_LENGTH      4     /* invalid length */

// Build Device Types - Used during compilation
//   These are the types of devices to build
//   Bit masked into ZSTACK_DEVICE_BUILD
#define DEVICE_BUILD_COORDINATOR    0x01
#define DEVICE_BUILD_ROUTER         0x02
#define DEVICE_BUILD_ENDDEVICE      0x04

// OSAL NV item IDs
#define ZCD_NV_EXTADDR                    0x0001
#define ZCD_NV_BOOTCOUNTER                0x0002
#define ZCD_NV_STARTUP_OPTION             0x0003
#define ZCD_NV_START_DELAY                0x0004

// NWK Layer NV item IDs
#define ZCD_NV_NIB                        0x0021
#define ZCD_NV_DEVICE_LIST                0x0022
#define ZCD_NV_ADDRMGR                    0x0023
#define ZCD_NV_POLL_RATE                  0x0024
#define ZCD_NV_QUEUED_POLL_RATE           0x0025
#define ZCD_NV_RESPONSE_POLL_RATE         0x0026
#define ZCD_NV_REJOIN_POLL_RATE           0x0027
#define ZCD_NV_DATA_RETRIES               0x0028
#define ZCD_NV_POLL_FAILURE_RETRIES       0x0029
#define ZCD_NV_STACK_PROFILE              0x002A
#define ZCD_NV_INDIRECT_MSG_TIMEOUT       0x002B
#define ZCD_NV_ROUTE_EXPIRY_TIME          0x002C
#define ZCD_NV_EXTENDED_PAN_ID            0x002D
#define ZCD_NV_BCAST_RETRIES              0x002E
#define ZCD_NV_PASSIVE_ACK_TIMEOUT        0x002F
#define ZCD_NV_BCAST_DELIVERY_TIME        0x0030
#define ZCD_NV_NWK_MODE                   0x0031
#define ZCD_NV_CONCENTRATOR_ENABLE        0x0032
#define ZCD_NV_CONCENTRATOR_DISCOVERY     0x0033
#define ZCD_NV_CONCENTRATOR_RADIUS        0x0034
#define ZCD_NV_CONCENTRATOR_RC            0x0036
#define ZCD_NV_NWK_MGR_MODE               0x0037
#define ZCD_NV_SRC_RTG_EXPIRY_TIME        0x0038
#define ZCD_NV_ROUTE_DISCOVERY_TIME       0x0039
#define ZCD_NV_NWK_ACTIVE_KEY_INFO        0x003A
#define ZCD_NV_NWK_ALTERN_KEY_INFO        0x003B
#define ZCD_NV_ROUTER_OFF_ASSOC_CLEANUP   0x003C
#define ZCD_NV_NWK_LEAVE_REQ_ALLOWED      0x003D
#define ZCD_NV_NWK_CHILD_AGE_ENABLE       0x003E
#define ZCD_NV_DEVICE_LIST_KA_TIMEOUT     0x003F

// APS Layer NV item IDs
#define ZCD_NV_BINDING_TABLE              0x0041
#define ZCD_NV_GROUP_TABLE                0x0042
#define ZCD_NV_APS_FRAME_RETRIES          0x0043
#define ZCD_NV_APS_ACK_WAIT_DURATION      0x0044
#define ZCD_NV_APS_ACK_WAIT_MULTIPLIER    0x0045
#define ZCD_NV_BINDING_TIME               0x0046
#define ZCD_NV_APS_USE_EXT_PANID          0x0047
#define ZCD_NV_APS_USE_INSECURE_JOIN      0x0048
#define ZCD_NV_COMMISSIONED_NWK_ADDR      0x0049

#define ZCD_NV_APS_NONMEMBER_RADIUS       0x004B     // Multicast non_member radius
#define ZCD_NV_APS_LINK_KEY_TABLE         0x004C
#define ZCD_NV_APS_DUPREJ_TIMEOUT_INC     0x004D
#define ZCD_NV_APS_DUPREJ_TIMEOUT_COUNT   0x004E
#define ZCD_NV_APS_DUPREJ_TABLE_SIZE      0x004F

// Security NV Item IDs
#define ZCD_NV_SECURITY_LEVEL             0x0061
#define ZCD_NV_PRECFGKEY                  0x0062
#define ZCD_NV_PRECFGKEYS_ENABLE          0x0063
#define ZCD_NV_SECURITY_MODE              0x0064
#define ZCD_NV_SECURE_PERMIT_JOIN         0x0065
#define ZCD_NV_APS_LINK_KEY_TYPE          0x0066
#define ZCD_NV_APS_ALLOW_R19_SECURITY     0x0067

#define ZCD_NV_IMPLICIT_CERTIFICATE       0x0069
#define ZCD_NV_DEVICE_PRIVATE_KEY         0x006A
#define ZCD_NV_CA_PUBLIC_KEY              0x006B

#define ZCD_NV_USE_DEFAULT_TCLK           0x006D
#define ZCD_NV_TRUSTCENTER_ADDR           0x006E
#define ZCD_NV_RNG_COUNTER                0x006F
#define ZCD_NV_RANDOM_SEED                0x0070

// ZDO NV Item IDs
#define ZCD_NV_USERDESC                   0x0081
#define ZCD_NV_NWKKEY                     0x0082
#define ZCD_NV_PANID                      0x0083
#define ZCD_NV_CHANLIST                   0x0084
#define ZCD_NV_LEAVE_CTRL                 0x0085
#define ZCD_NV_SCAN_DURATION              0x0086
#define ZCD_NV_LOGICAL_TYPE               0x0087
#define ZCD_NV_NWKMGR_MIN_TX              0x0088
#define ZCD_NV_NWKMGR_ADDR                0x0089

#define ZCD_NV_ZDO_DIRECT_CB              0x008F

// ZCL NV item IDs
#define ZCD_NV_SCENE_TABLE                0x0091
#define ZCD_NV_MIN_FREE_NWK_ADDR          0x0092
#define ZCD_NV_MAX_FREE_NWK_ADDR          0x0093
#define ZCD_NV_MIN_FREE_GRP_ID            0x0094
#define ZCD_NV_MAX_FREE_GRP_ID            0x0095
#define ZCD_NV_MIN_GRP_IDS                0x0096
#define ZCD_NV_MAX_GRP_IDS                0x0097

// Non-standard NV item IDs
#define ZCD_NV_SAPI_ENDPOINT              0x00A1

// NV Items Reserved for Commissioning Cluster Startup Attribute Set (SAS):
// 0x00B1 - 0x00BF: Parameters related to APS and NWK layers
// 0x00C1 - 0x00CF: Parameters related to Security
// 0x00D1 - 0x00DF: Current key parameters
#define ZCD_NV_SAS_SHORT_ADDR             0x00B1
#define ZCD_NV_SAS_EXT_PANID              0x00B2
#define ZCD_NV_SAS_PANID                  0x00B3
#define ZCD_NV_SAS_CHANNEL_MASK           0x00B4
#define ZCD_NV_SAS_PROTOCOL_VER           0x00B5
#define ZCD_NV_SAS_STACK_PROFILE          0x00B6
#define ZCD_NV_SAS_STARTUP_CTRL           0x00B7

#define ZCD_NV_SAS_TC_ADDR                0x00C1
#define ZCD_NV_SAS_TC_MASTER_KEY          0x00C2
#define ZCD_NV_SAS_NWK_KEY                0x00C3
#define ZCD_NV_SAS_USE_INSEC_JOIN         0x00C4
#define ZCD_NV_SAS_PRECFG_LINK_KEY        0x00C5
#define ZCD_NV_SAS_NWK_KEY_SEQ_NUM        0x00C6
#define ZCD_NV_SAS_NWK_KEY_TYPE           0x00C7
#define ZCD_NV_SAS_NWK_MGR_ADDR           0x00C8

#define ZCD_NV_SAS_CURR_TC_MASTER_KEY     0x00D1
#define ZCD_NV_SAS_CURR_NWK_KEY           0x00D2
#define ZCD_NV_SAS_CURR_PRECFG_LINK_KEY   0x00D3

// NV Items Reserved for Trust Center Link Key Table entries
// 0x0101 - 0x01FF
#define ZCD_NV_TCLK_TABLE_START           0x0101
#define ZCD_NV_TCLK_TABLE_END             0x01FF

// NV Items Reserved for APS Link Key Table entries
// 0x0201 - 0x02FF
#define ZCD_NV_APS_LINK_KEY_DATA_START    0x0201     // APS key data
#define ZCD_NV_APS_LINK_KEY_DATA_END      0x02FF

// NV Items Reserved for applications (user applications)
// 0x0401 - 0x0FFF

/*********************************************************************
* Message/Cluster IDS
*/

// ZDO Cluster IDs
#define ZDO_RESPONSE_BIT_V1_0   ((uint8_t)0x80)
#define ZDO_RESPONSE_BIT        ((uint16_t)0x8000)

#define NWK_addr_req            ((uint16_t)0x0000)
#define IEEE_addr_req           ((uint16_t)0x0001)
#define Node_Desc_req           ((uint16_t)0x0002)
#define Power_Desc_req          ((uint16_t)0x0003)
#define Simple_Desc_req         ((uint16_t)0x0004)
#define Active_EP_req           ((uint16_t)0x0005)
#define Match_Desc_req          ((uint16_t)0x0006)
#define NWK_addr_rsp            (NWK_addr_req | ZDO_RESPONSE_BIT)
#define IEEE_addr_rsp           (IEEE_addr_req | ZDO_RESPONSE_BIT)
#define Node_Desc_rsp           (Node_Desc_req | ZDO_RESPONSE_BIT)
#define Power_Desc_rsp          (Power_Desc_req | ZDO_RESPONSE_BIT)
#define Simple_Desc_rsp         (Simple_Desc_req | ZDO_RESPONSE_BIT)
#define Active_EP_rsp           (Active_EP_req | ZDO_RESPONSE_BIT)
#define Match_Desc_rsp          (Match_Desc_req | ZDO_RESPONSE_BIT)

#define Complex_Desc_req        ((uint16_t)0x0010)
#define User_Desc_req           ((uint16_t)0x0011)
#define Discovery_Cache_req     ((uint16_t)0x0012)
#define Device_annce            ((uint16_t)0x0013)
#define User_Desc_set           ((uint16_t)0x0014)
#define Server_Discovery_req    ((uint16_t)0x0015)
#define End_Device_Timeout_req  ((uint16_t)0x001f)
#define Complex_Desc_rsp        (Complex_Desc_req | ZDO_RESPONSE_BIT)
#define User_Desc_rsp           (User_Desc_req | ZDO_RESPONSE_BIT)
#define Discovery_Cache_rsp     (Discovery_Cache_req | ZDO_RESPONSE_BIT)
#define User_Desc_conf          (User_Desc_set | ZDO_RESPONSE_BIT)
#define Server_Discovery_rsp    (Server_Discovery_req | ZDO_RESPONSE_BIT)
#define End_Device_Timeout_rsp  (End_Device_Timeout_req | ZDO_RESPONSE_BIT)

#define End_Device_Bind_req     ((uint16_t)0x0020)
#define Bind_req                ((uint16_t)0x0021)
#define Unbind_req              ((uint16_t)0x0022)
#define Bind_rsp                (Bind_req | ZDO_RESPONSE_BIT)
#define End_Device_Bind_rsp     (End_Device_Bind_req | ZDO_RESPONSE_BIT)
#define Unbind_rsp              (Unbind_req | ZDO_RESPONSE_BIT)

#define Mgmt_NWK_Disc_req       ((uint16_t)0x0030)
#define Mgmt_Lqi_req            ((uint16_t)0x0031)
#define Mgmt_Rtg_req            ((uint16_t)0x0032)
#define Mgmt_Bind_req           ((uint16_t)0x0033)
#define Mgmt_Leave_req          ((uint16_t)0x0034)
#define Mgmt_Direct_Join_req    ((uint16_t)0x0035)
#define Mgmt_Permit_Join_req    ((uint16_t)0x0036)
#define Mgmt_NWK_Update_req     ((uint16_t)0x0038)
#define Mgmt_NWK_Disc_rsp       (Mgmt_NWK_Disc_req | ZDO_RESPONSE_BIT)
#define Mgmt_Lqi_rsp            (Mgmt_Lqi_req | ZDO_RESPONSE_BIT)
#define Mgmt_Rtg_rsp            (Mgmt_Rtg_req | ZDO_RESPONSE_BIT)
#define Mgmt_Bind_rsp           (Mgmt_Bind_req | ZDO_RESPONSE_BIT)
#define Mgmt_Leave_rsp          (Mgmt_Leave_req | ZDO_RESPONSE_BIT)
#define Mgmt_Direct_Join_rsp    (Mgmt_Direct_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_Permit_Join_rsp    (Mgmt_Permit_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_NWK_Update_notify  (Mgmt_NWK_Update_req | ZDO_RESPONSE_BIT)
