#include "readFile.h"
#include <cstdio>
void readFile(const char* file, size_t& pSize, char*& data) {
    size_t read;
    char *result;
    FILE *fp;
    fp = fopen(file, "rb");
    if (fp == nullptr) {
        data = nullptr;
        return;
    }
    fseek(fp, 0, SEEK_END);
    pSize = (size_t) (ftell(fp));
    if (pSize == 0) {
        fclose(fp);
        data = nullptr;
        return;
    }
    result = (char *) malloc(pSize + 1);
    if (result == nullptr) {
        data = nullptr;
        return;
    }
    fseek(fp, 0, SEEK_SET);
    read = fread(result, 1, pSize, fp);
    fclose(fp);
    if (read != pSize) {
        free(result);
        data = nullptr;
        return;
    }
    result[pSize] = 0;
    data = result;
}
