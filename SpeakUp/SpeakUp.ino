#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}

void setup() {
  delay(500);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);

}

void broadcast_ssid(char* ssid) {
  int pkt_offset_ssid = 36;
  byte wifi_channel = random(1, 11);
  wifi_set_channel(wifi_channel);

  uint8_t beacon_packet[128] = {

    /* --- MAC header, length:24 --- */
    /* 0000    */ 0x80, 0x00,                                     /* Frame control; type/subtype: 0x80 => Beacon Type */
    /* 0002    */ 0x00, 0x00,                                     /* Duration */
    /* 0004    */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,             /* (DA) Destination MAC Address: FF:FF:FF:FF:FF:FF for Broadcast */
    /* 000a,10 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,             /* (SA) Source MAC Address: any fake MAC address */
    /* 0010,16 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,             /* (BSS ID) usually same as SA */
    /* 0016,22 */ 0xc0, 0x6c,                                     /* Sequence Control */

    /* --- Frame Body, length: 0~2312 --- */
    /* 0018,24 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, /* Timestamp: the number of microseconds the AP has been active */
    /* 0020,32 */ 0xff, 0x00,                                     /* Beacon Interval */
    /* 0022,34 */ 0x01, 0x04,                                     /* Capability Info */

    /* --- SSID, length: 0~32 */
    /* 0024,36 */ 0x00                                            /* Element ID: 0 => SSID */
    /* 0025,37 */                                                 /* Length: length of SSID */
    /* 0026,38 */                                                 /* SSID */
  };

  uint8_t packet_post_SSID[13] = {
    /* --- Supported Rates --- */
    0x01,                                           /* Element ID: 1 => Supported Rates */
    0x08,                                           /* Length */
    0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, /* Supported Rates (8 bytes) */

    /* --- DS Parameter set --- */
    0x03,                                           /* Element ID: 3 => DSSS Parameter Set */
    0x01,                                           /* Length */
    0x06                                            /* Current Channel */
  };

  int ssid_len = strlen(ssid);
  beacon_packet[pkt_offset_ssid + 1] = ssid_len;

  for (int i = 0; i < ssid_len; i++ ) {
    beacon_packet[pkt_offset_ssid + 2 + i] = ssid[i];
  }

  for (int i = 0; i < 12; i++) { // append packet_post_SSID[] after SSID field except 'Current Channel'
    beacon_packet[pkt_offset_ssid + 2 + ssid_len + i] = packet_post_SSID[i];
  }

  beacon_packet[pkt_offset_ssid + 2 + 12 + ssid_len] = wifi_channel;

  beacon_packet[10] = beacon_packet[16] = random(256);
  beacon_packet[11] = beacon_packet[17] = random(256);
  beacon_packet[12] = beacon_packet[18] = random(256);
  beacon_packet[13] = beacon_packet[19] = random(256);
  beacon_packet[14] = beacon_packet[20] = random(256);
  beacon_packet[15] = beacon_packet[21] = random(256);

  wifi_send_pkt_freedom(beacon_packet, (pkt_offset_ssid + 2 + 13 + ssid_len), 0);
  wifi_send_pkt_freedom(beacon_packet, (pkt_offset_ssid + 2 + 13 + ssid_len), 0);
  wifi_send_pkt_freedom(beacon_packet, (pkt_offset_ssid + 2 + 13 + ssid_len), 0);
  delay(1);
}

void speak_up() {
  broadcast_ssid(" #FREEHONGKONG");
  broadcast_ssid(" #FREEEASTTURKESTAN");
  broadcast_ssid(" #FREETIBET");
  broadcast_ssid(" #STAYTAIWANFREE");
  broadcast_ssid(" #FREECHINA");
}

void loop() {
  speak_up();
}
