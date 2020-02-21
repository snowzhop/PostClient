#ifndef MMIVAS_BASE64UTIL_H
#define MMIVAS_BASE64UTIL_H

char hashNumber(const int& symbolNumber);
char hashSymbol(const char& symbol);

char* encodeBase64(const char* originalText);
char* decodeBase64(const char* originalText);

#endif //MMIVAS_BASE64UTIL_H
