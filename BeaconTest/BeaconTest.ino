
/*
 * MAC Header:
 *
 * frame control – control bits (figure-2 and figure-3),
 * DA – destination MAC address; we use ethernet broadcast so the address is FF:FF:FF:FF:FF:FF,
 * SA – source MAC address; we should set here some fake MAC address,
 * BSS ID – Basic Service Set Identifier; this field usually represents access point MAC address so we can use SA address,
 * seq-ctl – sequence control; we can use zero value here.
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}

// Beacon Packet buffer
uint8_t packet[128] = {

  /* --- MAC header, length:24 --- */
  /* 0000    */ 0x80, 0x00,                                     /* Frame control; type/subtype: 0x80 => Beacon Type */
  /* 0002    */ 0x00, 0x00,                                     /* Duration */
  /* 0004    */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,             /* (DA) Destination MAC Address: FF:FF:FF:FF:FF:FF for Broadcast */
  /* 000a,10 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,             /* (SA) Source MAC Address: any fake MAC address */
  /* 0010,16 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,             /* (BSS ID) usually same as SA */
  /* 0016,22 */ 0xc0, 0x6c,                                     /* Sequence Control */

  /* --- Frame Body, length: 0~2312 --- */
  /* 0018,24 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, /* Timestamp: the number of microseconds the AP has been active */
  /* 0020,32 */ 0x64, 0x00,                                     /* Beacon Interval */
  /* 0022,34 */ 0x01, 0x04,                                     /* Capability Info */

  /* --- SSID, length: 0~32 */
  /* 0024,36 */ 0x00,                                           /* Element ID: 0 => SSID */
  /* 0025,37 */ 0x0d,                                           /* Length: length of SSID */
  /* 0026,38 */ 0x23, 0x41, 0x34, 0x52, 0x65,                   /* SSID: "#A4Revolution" */
                0x76, 0x6F, 0x6C, 0x75, 0x74,
                0x69, 0x6F, 0x6E,

  /* --- Supported Rates --- */
                0x01,                                           /* Element ID: 1 => Supported Rates */
                0x08,                                           /* Length */
                0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, /* Supported Rates (8 bytes) */

  /* --- DS Parameter set --- */
                0x03,                                           /* Element ID: 3 => DSSS Parameter Set */
                0x01,                                           /* Length */
                0x06                                            /* Current Channel */
};

void setup() {
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);
    
}

void loop() {
  wifi_set_channel(6);

  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);

  wifi_send_pkt_freedom(packet, 64, 0);
  wifi_send_pkt_freedom(packet, 64, 0);
  wifi_send_pkt_freedom(packet, 64, 0);
  delay(1);
}
