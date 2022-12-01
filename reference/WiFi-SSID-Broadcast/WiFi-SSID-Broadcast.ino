// https://sharetext.me/c88zc9aswn
#include <Arduino.h>
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}

String apNames[] = {
  "习惯性近身平A",
  "他改变了中国,他又改回去了",
  "既要，又要",
  "他三连击了",
  "他为世界指明了方向",
  "通商宽衣",
  "萨格尔王",
  "岿（kui）然不动",
  "十里山路不换肩",
  "扛麦郎",
  "小学博士",
  "讲好中国故事",
  "顺差逆差",
  "颐使气指的教师爷",
  "绿水青山就是金山银山",
  "吃饱了没事干的外国人",
  "撒胡椒面",
  "功成不必在我",
  "香喷喷的豆包",
  "外国人还爱吃饺子",
};

const int apCount = sizeof(apNames) / sizeof(String);
byte apMACs[apCount][6];

int currentAP = 0;
int bPointer = 52;
char prefix = '#';

//Beacon Packet buffer
byte packet[128] = {
  /* IEEE 802.11 Beacon Frame */
  /*0*/   0x80, 0x00, 0x00, 0x00,  //type/subtype = beacon
  /*4*/   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, //dst addr = broadcast
  /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //src addr = random MAC
  /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //bss id = src addr
  /*22*/  0x90, 0x53, //seq number 1337 + frag number 0
  /*24*/  0xDE, 0xAD, 0x00, 0xBE, 0xEF, 0x00, 0xF0, 0x0D, //timestamp
  /*32*/  0x64, 0x00, //beacon interval (~100ms)
  /*34*/  0x01, 0x04, //capabilities info

  /* IEEE 802.11 wireless LAN managment frame */
  /*36*/  0x01, 0x08, //tag = Supported Rates, size
  0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,
  /*46*/  0x03, 0x01, 0xFF, //tag = Channel, size, random channel

  /*49*/  0x00, 0xFF, 0x23 //tag = SSID, size, prefix char
};


void fillMACs() {
  for (int j = 0; j < apCount; j++) {
    apMACs[j][0] = random(256);
    apMACs[j][1] = random(256);
    apMACs[j][2] = random(256);
    apMACs[j][3] = random(256);
    apMACs[j][4] = random(256);
    apMACs[j][5] = random(256);
  }
}


void setup() {
  delay(100);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);
  WiFi.setOutputPower(20.5);
  packet[51] = prefix;
  fillMACs();
}


void loop() {
  //Set random channel (pinning this variable wasn't giving me good results)
  byte channel = random(1, 11);
  wifi_set_channel(channel);
  packet[48] = channel;

  //Copying src addr and bssid to packet buffer
  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);

  //Copying SSID to packet buffer
  byte currLenght = apNames[currentAP].length();
  packet[50] = currLenght + 1;
  for (int i = bPointer; i < bPointer + currLenght; i++) {
    packet[i] = apNames[currentAP][i - bPointer];
  }

  //Sending beacon.
  wifi_send_pkt_freedom(packet, bPointer + currLenght, 0);
  wifi_send_pkt_freedom(packet, bPointer + currLenght, 0);
  wifi_send_pkt_freedom(packet, bPointer + currLenght, 0);
  wifi_send_pkt_freedom(packet, bPointer + currLenght, 0);
  delay(1);

  currentAP++;
  if (currentAP == apCount) {
    currentAP = 0;
    delay(1);
  }
}
