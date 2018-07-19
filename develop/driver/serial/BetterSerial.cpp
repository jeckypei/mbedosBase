/*
* added jecky
*/

#include "BetterSerial.h"


BetterSerial::BetterSerial(PinName tx, PinName rx, const char *name, int baud) : Serial(tx,rx,name,baud) {
}

BetterSerial::BetterSerial(PinName tx, PinName rx, int baud): Serial(tx,rx,baud) {
}


void BetterSerial::readLock() {
    readMutex.lock();
}

void BetterSerial::readUnlock() {
    readMutex.unlock();
}


void BetterSerial::writeLock() {
    writeMutex.lock();
}

void BetterSerial::writeUnlock() {
    writeMutex.unlock();
}


int BetterSerial::putc(int c) {
    writeLock();
    fflush(_file);
    int ret = std::fputc(c, _file);
    writeUnlock();
    return ret;
}
int BetterSerial::puts(const char *s) {
    writeLock();
    fflush(_file);
    int ret = std::fputs(s, _file);
    writeUnlock();
    return ret;
}
int BetterSerial::getc() {
    readLock();
    fflush(_file);
    int ret = mbed_getc(_file);
    readUnlock();
    return ret;
}
char* BetterSerial::gets(char *s, int size) {
    readLock();
    fflush(_file);
    char *ret = mbed_gets(s,size,_file);
    readUnlock();
    return ret;
}

ssize_t BetterSerial::write(const void* buffer, size_t length) {
    const char* ptr = (const char*)buffer;
    const char* end = ptr + length;

    writeLock();
    while (ptr != end) {
        if (_putc(*ptr++) == EOF) {
            break;
        }
    }
    writeUnlock();

    return ptr - (const char*)buffer;
}

ssize_t BetterSerial::read(void* buffer, size_t length) {
    char* ptr = (char*)buffer;
    char* end = ptr + length;

    readLock();
    while (ptr != end) {
        int c = _getc();
        if (c==EOF) break;
        *ptr++ = c;
    }
    readUnlock();

    return ptr - (const char*)buffer;
}

int BetterSerial::printf(const char* format, ...) {
    writeLock();
    std::va_list arg;
    va_start(arg, format);
    fflush(_file);
    int r = vfprintf(_file, format, arg);
    va_end(arg);
    writeUnlock();
    return r;
}

int BetterSerial::scanf(const char* format, ...) {
    readLock();
    std::va_list arg;
    va_start(arg, format);
    fflush(_file);
    int r = vfscanf(_file, format, arg);
    va_end(arg);
    readUnlock();
    return r;
}

int BetterSerial::vprintf(const char* format, std::va_list args) {
    writeLock();
    fflush(_file);
    int r = vfprintf(_file, format, args);
    writeUnlock();
    return r;
}

int BetterSerial::vscanf(const char* format, std::va_list args) {
    readLock();
    fflush(_file);
    int r = vfscanf(_file, format, args);
    writeUnlock();
    return r;
}


