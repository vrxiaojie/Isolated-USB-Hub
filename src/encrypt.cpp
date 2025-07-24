#include "encrypt.h"
#include <Arduino.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>
const byte key[16] = {'V', 'R', 'x', 'i', 'a', 'o', 'j', 'i', 'e', '-', 'U', 'S', 'B', 'H', 'u', 'b'};
uint8_t mac[6];

bool encrypt_mac()
{
    // test 加密
    byte aes_input[16]; // AES 输入大小必须是16字节
    esp_efuse_mac_get_default(mac);
    Serial.print("MAC:");
    for (int i = 0; i < 6; i++)
    {
        Serial.printf("%X", mac[i]);
        aes_input[i] = mac[i]; // 将 MAC 地址前6个字节填入输入
    }
    Serial.println();
    // 填充其余字节（AES 输入必须是16字节）
    for (int i = 6; i < 16; i++)
    {
        aes_input[i] = 0; // 填充为 0
    }

    // AES 加密
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 128); // 设置密钥和加密模式
    byte encrypted_mac[16];
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, aes_input, encrypted_mac); // 执行 AES 加密
    mbedtls_aes_free(&aes);
    Serial.print("AES加密后的MAC:");
    for (int i = 0; i < 16; i++)
    {
        Serial.print(encrypted_mac[i], HEX);
    }
    Serial.println();
    // 使用 SHA256 生成身份码
    unsigned char hash[32]; // SHA256 输出长度为 32 字节
    mbedtls_sha256_context sha256_ctx;
    mbedtls_sha256_init(&sha256_ctx);
    mbedtls_sha256_starts(&sha256_ctx, 0);                 // 0 表示无补码
    mbedtls_sha256_update(&sha256_ctx, encrypted_mac, 16); // 使用加密后的 MAC 地址
    mbedtls_sha256_finish(&sha256_ctx, hash);              // 完成 SHA256 计算
    mbedtls_sha256_free(&sha256_ctx);

    // 打印生成的身份码
    Serial.print("Generated Unique ID: ");
    for (int i = 0; i < 32; i++)
    {
        Serial.print(hash[i], HEX);
    }
    Serial.println();
}
