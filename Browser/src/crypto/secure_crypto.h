#ifndef SECURE_BROWSER_CRYPTO_H_
#define SECURE_BROWSER_CRYPTO_H_

#include <string>

// Stage 12: DPAPI based transparent encryption/decryption for sensitive files.
class SecureCrypto {
public:
    static bool EncryptData(const std::string& plaintext, std::string& ciphertext_base64);
    static bool DecryptData(const std::string& ciphertext_base64, std::string& plaintext);

    // File I/O Helpers
    static bool WriteEncryptedFile(const std::string& filepath, const std::string& plaintext);
    static bool ReadEncryptedFile(const std::string& filepath, std::string& plaintext);
};

#endif // SECURE_BROWSER_CRYPTO_H_
