#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}

const char *ssid_list[] = {
  // UTF-8 encoder/decoder: https://mothereff.in/utf-8
  " 1 \xE4\xB8\x8D\xE8\xA6\x81\xE6\xA0\xB8\xE9\x85\xB8\xE8\xA6\x81\xE5\x90\x83\xE9\xA3\xAF", /* 不要核酸要吃飯 */
  " 2 \xE4\xB8\x8D\xE8\xA6\x81\xE5\xB0\x81\xE6\x8E\xA7\xE8\xA6\x81\xE8\x87\xAA\xE7\x94\xB1", /* 不要封控要自由 */
  " 3 \xE4\xB8\x8D\xE8\xA6\x81\xE8\xAC\x8A\xE8\xA8\x80\xE8\xA6\x81\xE5\xB0\x8A\xE5\x9A\xB4", /* 不要謊言要尊嚴 */
  " 4 \xE4\xB8\x8D\xE8\xA6\x81\xE6\x96\x87\xE9\x9D\xA9\xE8\xA6\x81\xE6\x94\xB9\xE9\x9D\xA9", /* 不要文革要改革 */
  " 5 \xE4\xB8\x8D\xE8\xA6\x81\xE9\xA0\x98\xE8\xA2\x96\xE8\xA6\x81\xE9\x81\xB8\xE7\xA5\xA8", /* 不要領袖要選票 */
  " 6 \xE4\xB8\x8D\xE5\x81\x9A\xE5\xA5\xB4\xE6\x89\x8D\xE5\x81\x9A\xE5\x85\xAC\xE6\xB0\x91", /* 不做奴才做公民 */
  //  " #FREEHONGKONG",
  //  " #FREEEASTTURKESTAN",
  //  " #FREETIBET",
  //  " #STAYTAIWANFREE",
  //  " #FREECHINA",
};

void setup() {
  delay(500);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);
}

void broadcast_ssid(const char* ssid) {
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
    0x00                                            /* Current Channel */
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
  int ssid_list_len = sizeof ssid_list / sizeof ssid_list[0];
  for (int i = 0; i < ssid_list_len; i++) {
    broadcast_ssid(ssid_list[i]);
  }
}

void loop() {
  speak_up();
}
