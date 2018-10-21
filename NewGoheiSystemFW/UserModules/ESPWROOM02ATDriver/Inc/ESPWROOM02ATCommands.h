/*
 * ESPWROOM02ATCommands.h
 *
 *  Created on: 2018/10/20
 *      Author: Shuji
 */

#ifndef ESPWROOM02ATDRIVER_INC_ESPWROOM02ATCOMMANDS_H_
#define ESPWROOM02ATDRIVER_INC_ESPWROOM02ATCOMMANDS_H_

//応答
const char REP_READY[]               = "ready";
const char REP_WIFI_CONNECTED[]      = "WIFI CONNECTED";
const char REP_WIFI_GOT_IP[]         = "WIFI GOT IP";
const char REP_OK[]                  = "OK";
const char REP_CONNECTED[]           = ",CONNECT";
const char REP_SEND_OK[]             = "SEND OK";
const char REP_RECEIVE_FROM_SERVER[] = "+IPD,";

//コマンド
const char CMD_CONNECTION_START[] = "AT+CIPSTART=";
const char CMD_SENDCOMMAND_NICT[] = "AT+CIPSEND=";
const char CMD_NEWLINE[]          = "\r\n";
const char CMD_CONNECTION_CLOSE[] = "AT+CIPCLOSE";
const char CMD_CONNECT_AP[] = "AT+CWJAP_CUR";

#endif /* ESPWROOM02ATDRIVER_INC_ESPWROOM02ATCOMMANDS_H_ */
