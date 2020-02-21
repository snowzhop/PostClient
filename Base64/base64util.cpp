#include <cstring>
#include <algorithm>
#include "base64util.h"

char hashNumber(const int& symbolNumber) {
    if (symbolNumber >= 0 && symbolNumber <= 25) {
        return char(symbolNumber + 'A');
    }
    if (symbolNumber >= 26 && symbolNumber <= 51) {
        return char('a' + symbolNumber - 26);
    }
    if (symbolNumber >= 52 && symbolNumber <= 61) {
        return char('0' + symbolNumber - 26 * 2);
    }
    if (symbolNumber == 62) {
        return '+';
    }
    if (symbolNumber == 63) {
        return '/';
    }
    
    return '$';
}

char hashSymbol(const char& symbol) {
    if (symbol >= 'A' && symbol <= 'Z') {
        return char(symbol - 'A');
    }
    if (symbol >= 'a' && symbol <= 'z') {
        return char(symbol + 26 - 'a');
    }
    if (symbol >= '0' && symbol <= '9') {
        return char(symbol + 26 * 2 - '0');
    }
    if (symbol == '+') {
        return char(62);
    }
    if (symbol == '/') {
        return char(63);
    }
    
    return -1;
}

char* encodeBase64(const char* originalText) {
    int textSize = std::strlen(originalText);
    int resultSize = 4 * (textSize / 3 + (textSize % 3 != 0));
    
    int tmpTextSize = textSize % 3 == 0 ? textSize : (textSize + textSize % 3);
    char tmpText[tmpTextSize];
    memset(tmpText, 0, tmpTextSize);
    
    strcpy(tmpText, originalText);
    
    char* result = new char[resultSize + 1];
    memset(result, -1, resultSize);
    
    int i = 0;
    while (i * 3 < textSize) {
        int j = i * 4;
        result[j] = hashNumber((int)(tmpText[i * 3] >> 2));
        result[j + 1] = hashNumber((int)(
                ((3 & tmpText[i * 3]) << 4) | (tmpText[i * 3 + 1] >> 4)
        ));
        result[j + 2] = hashNumber((int)(
                ((15 & tmpText[i * 3 + 1]) << 2) | (tmpText[i * 3 + 2] >> 6)
        ));
        result[j + 3] = hashNumber((int)(
                63 & tmpText[i * 3 + 2]
        ));
        i++;
    }
    
    if (i * 3 != textSize) {
        int j = i * 4;
        result[j] = hashNumber((int)(tmpText[i * 3] >> 2));
        result[j + 1] = hashNumber((int)(
                ((3 & tmpText[i * 3]) << 4) | (tmpText[i * 3 + 1] >> 4)
        ));
        result[j + 2] = hashNumber((int)(
                ((15 & tmpText[i * 3 + 1]) << 2) | (tmpText[i * 3 + 2] >> 6)
        ));
        result[j + 3] = hashNumber((int)(
                63 & tmpText[i * 3 + 2]
        ));
        
        for(j = 0; j < i * 3 - textSize; j++) {
            result[resultSize - 1 - j] = '=';
        }
    }
    
    result[resultSize] = '\0';
    return result;
}

char* decodeBase64(const char* originalText) {
    int textSize = std::strlen(originalText);
    int resultSize = 3 * textSize / 4 - [&originalText, &textSize]() -> int {
        int signs = 0;
        for(int i = textSize - 1; i > textSize - 5 ; i--) {
            if (originalText[i] == '=') {
                signs++;
            }
        }
        return signs;
    }();
    
    char* tmpStr = new char[textSize];
    strcpy(tmpStr, originalText);
    
    char* result = new char[resultSize + 1];
    
    int i = 0;
    while (i * 4 < textSize) {
        int j = i * 3;
        result[j]       =
                hashSymbol(tmpStr[i * 4]) << 2 |
                hashSymbol(tmpStr[i * 4 + 1]) >> 4;
        result[j + 1]   =
                hashSymbol(tmpStr[i * 4 + 1]) << 4 |
                hashSymbol(tmpStr[i * 4 + 2]) >> 2;
        result[j + 2]   =
                hashSymbol(tmpStr[i * 4 + 2]) << 6 |
                hashSymbol(tmpStr[i * 4 + 3]);
        
        i++;
    }
    
    if (i * 4 != textSize) {
        int j = i * 3;
        result[j]       =
                hashSymbol(tmpStr[i * 4]) << 2 |
                hashSymbol(tmpStr[i * 4 + 1]) >> 4;
        result[j + 1]   =
                hashSymbol(tmpStr[i * 4 + 1]) << 4 |
                hashSymbol(tmpStr[i * 4 + 2]) >> 2;
        result[j + 2]   =
                hashSymbol(tmpStr[i * 4 + 2]) << 6 |
                hashSymbol(tmpStr[i * 4 + 3]);
    }
    
    result[resultSize] = '\0';
    
    return result;
}