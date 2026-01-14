#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cstdint>
#include <termios.h>
#include <unistd.h>
#include "MQTTClient.h"
#include "Ascon128.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "C_Controller"
#define TOPIC       "robot/cmd"
#define QOS         1
#define TIMEOUT     10000L

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) perror("tcgetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}

int main() {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        return -1;
    }

    uint8_t key[16] = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
    };
    uint8_t iv[16] = {
        0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
        0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF
    };

    printf("C Controller ready. Press W/A/S/D/X to control the robot. Q to quit.\n");

    while (1) {
        char c = getch();
        const char* command = nullptr;

        switch (c) {
            case 'w': case 'W': command = "FORWARD"; break;
            case 's': case 'S': command = "BACK";    break;
            case 'a': case 'A': command = "LEFT";    break;
            case 'd': case 'D': command = "RIGHT";   break;
            case 'x': case 'X': command = "STOP";    break;
            case 'q': case 'Q': goto exit_loop;
            default: continue;
        }

        size_t plen = strlen(command);

        // ===== Create a new cipher object for every command =====
        Ascon128 cipher;
        cipher.setKey(key, sizeof(key));
        cipher.setIV(iv, sizeof(iv)); // reset IV for every message

        uint8_t ciphertext[plen];
        uint8_t tag[16];

        cipher.encrypt(ciphertext, (const uint8_t*)command, plen);
        cipher.computeTag(tag, sizeof(tag));

        uint8_t payload[plen + sizeof(tag)];
        memcpy(payload, ciphertext, plen);
        memcpy(payload + plen, tag, sizeof(tag));

        pubmsg.payload = (void*)payload;
        pubmsg.payloadlen = plen + sizeof(tag);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        MQTTClient_waitForCompletion(client, token, TIMEOUT);

        printf("Sent command: %s\n", command);
    }

exit_loop:
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    printf("Controller exited.\n");
    return 0;
}
