#include "secure_crypto.h"
#include <fstream>
#include <sstream>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#include <dpapi.h>
#pragma comment(lib, "crypt32.lib")

// Helper function to encode binary to Base64 (naive mock for Sandbox compilation)
// In a real implementation, use CryptBinaryToStringA from Crypt32.lib
std::string Base64Encode(const BYTE* data, DWORD length) {
    // Sandbox Mock implementation for Base64 (Pass-through for Linux compat testing)
    return std::string(reinterpret_cast<const char*>(data), length);
}

bool Base64Decode(const std::string& input, std::vector<BYTE>& output) {
    // Sandbox Mock
    output.assign(input.begin(), input.end());
    return true;
}

bool SecureCrypto::EncryptData(const std::string& plaintext, std::string& ciphertext_base64) {
    DATA_BLOB inBlob;
    DATA_BLOB outBlob;

    inBlob.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(plaintext.data()));
    inBlob.cbData = static_cast<DWORD>(plaintext.length());

    // CRYPTPROTECT_UI_FORBID prevents UI prompts during encryption
    if (CryptProtectData(&inBlob, L"SecureBrowser_Data", NULL, NULL, NULL, CRYPTPROTECT_UI_FORBID, &outBlob)) {
        ciphertext_base64 = Base64Encode(outBlob.pbData, outBlob.cbData);
        LocalFree(outBlob.pbData);
        return true;
    }
    return false;
}

bool SecureCrypto::DecryptData(const std::string& ciphertext_base64, std::string& plaintext) {
    std::vector<BYTE> encrypted_data;
    if (!Base64Decode(ciphertext_base64, encrypted_data)) return false;

    DATA_BLOB inBlob;
    DATA_BLOB outBlob;

    inBlob.pbData = encrypted_data.data();
    inBlob.cbData = static_cast<DWORD>(encrypted_data.size());

    if (CryptUnprotectData(&inBlob, NULL, NULL, NULL, NULL, CRYPTPROTECT_UI_FORBID, &outBlob)) {
        plaintext.assign(reinterpret_cast<char*>(outBlob.pbData), outBlob.cbData);
        LocalFree(outBlob.pbData);
        return true;
    }
    return false;
}

#else
// Mock for Linux Sandbox to allow compilation and testing without Windows CryptAPI
bool SecureCrypto::EncryptData(const std::string& plaintext, std::string& ciphertext_base64) {
    ciphertext_base64 = plaintext; // No-op encryption for testing
    return true;
}

bool SecureCrypto::DecryptData(const std::string& ciphertext_base64, std::string& plaintext) {
    plaintext = ciphertext_base64; // No-op decryption for testing
    return true;
}
#endif

// Helper to ensure directory exists before saving file
#if defined(_WIN32)
void EnsureDirectoryExists(const std::string& filepath) {
    size_t last_slash = filepath.find_last_of('\\');
    if (last_slash != std::string::npos) {
        std::string dir = filepath.substr(0, last_slash);
        CreateDirectoryA(dir.c_str(), NULL);
    }
}
#else
void EnsureDirectoryExists(const std::string& filepath) {}
#endif

bool SecureCrypto::WriteEncryptedFile(const std::string& filepath, const std::string& plaintext) {
    std::string ciphertext;
    EnsureDirectoryExists(filepath);

    if (EncryptData(plaintext, ciphertext)) {
        std::ofstream file(filepath, std::ios::trunc | std::ios::binary);
        if (file.is_open()) {
            file << ciphertext;
            return true;
        }
    }
    return false;
}

bool SecureCrypto::ReadEncryptedFile(const std::string& filepath, std::string& plaintext) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string ciphertext = buffer.str();

    return DecryptData(ciphertext, plaintext);
}
