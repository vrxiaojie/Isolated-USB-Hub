#ifndef ENCRYPT_H_
#define ENCRYPT_H_
#include <Arduino.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>
bool check_encrypted_mac(String received_data);
void send_mac();
void encrypt_mac();
#endif