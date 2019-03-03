#include "pxt.h"
#include "MicroBitFile.h"
#include "MicroBitFileSystem.h"

using namespace pxt;

// v0 backward compat support
#ifndef PXT_BUFFER_DATA
#define PXT_BUFFER_DATA(buffer) buffer->payload
#endif

/**
* File seek offset modifier
*/
enum FileSystemSeekFlags {
    //% block=set
    Set = MB_SEEK_SET,
    //% block=current
    Current = MB_SEEK_CUR,
    //% block=end
    End = MB_SEEK_END
};

/**
* File system operations
*/
//% weight=5 color=#002050 icon="\uf0a0"
namespace files
{    
// Initializes file system. Must be called before any FS operation.
// built-in size computation for file system
// does not take into account size changes
// for compiled code
void initFileSystem()
{
    if (MicroBitFileSystem::defaultFileSystem == NULL)
    {
        new MicroBitFileSystem(pxt::afterProgramPage());
    }
}

/**
    * Appends text and a new line to a file
    * @param filename file name, eg: "output.txt"
    * @param text the string to append to the end of the file
    */
//% blockId="files_append_line" block="file %filename|append line %text"
//% blockExternalInputs=1 weight=90 blockGap=8
void appendLine(String filename, String text)
{
    int res = 0;
    initFileSystem();
    MicroBitFile f(ManagedString("test.t"));//MSTR(filename));
    
    res = f.append(MSTR(text));
    f.append("\r\n");
    f.close();

    uBit.serial.send(ManagedString(res), SYNC_SPINWAIT);
}

/**
    * Appends text to a file
    * @param filename file name, eg: "output.txt"
    * @param text the string to append to the end of the file
    */
//% blockId="fs_append_string" block="file %filename|append string %text"
//% blockExternalInputs=1 weight=86 blockGap=8
void appendString(String filename, String text)
{
    initFileSystem();
    MicroBitFile f(MSTR(filename));
    f.append(MSTR(text));
    f.close();
}

/**
* Reads the content of the file to send it to serial
* @param filename file name, eg: "output.txt"
*/
//% blockId="fs_write_to_serial" block="file %filename|read to serial"
//% weight=80
void readToSerial(String filename) {

    // MicroBitFileSystem* fs = MicroBitFileSystem::defaultFileSystem;//MSTR(filename).toCharArray()
    // int fd = fs->open("test.t", MB_READ);
    // if(fd<0) {
    //     uBit.serial.send("file open error:", SYNC_SPINWAIT);
    //     uBit.serial.send(ManagedString(fd), SYNC_SPINWAIT);
    //     return;
    // }

    // uint8_t buf[32];
    // int read = 0;
    // while((read = fs->read(fd, buf, sizeof(buf) * sizeof(uint8_t))) > 0) {
    //      uBit.serial.send(buf, read * sizeof(uint8_t), SYNC_SPINWAIT);
    // }  

    // uBit.serial.send(ManagedString(read), SYNC_SPINWAIT);

    // fs->close(fd);


    initFileSystem();
    MicroBitFile f(MSTR(filename));
    char buf[32];
    int read = 0;
    while((read = f.read(buf, sizeof(buf) * sizeof(char))) > 0) {
         uBit.serial.send((uint8_t*)buf, read * sizeof(char), SYNC_SPINWAIT);
    }
    uBit.serial.send(ManagedString(read), SYNC_SPINWAIT);
    f.close();    
}

/**
    * Removes the file. There is no undo for this operation.
    * @param filename name of the file to remove, eg: "output.txt"
    */
//% blockId="fs_remove" block="file remove %filename"
//% weight=80 advanced=true blockGap=8
void remove(String filename)
{
    initFileSystem();
    MicroBitFileSystem::defaultFileSystem->remove(MSTR(filename).toCharArray());
}

/**
* Creates a directory
* @param name full qualified path to the new directory
*/
//% advanced=true weight=10
//% blockId=files_create_directory block="files create directory %name"
void createDirectory(String name) {
    initFileSystem();
    MicroBitFileSystem::defaultFileSystem->createDirectory(MSTR(name).toCharArray());
}

/**
* Reads a number settings, -1 if not found.
* @param name name of the settings, must be filename compatible, e.g.: setting
*/
//% blockId=settings_read_number block="settings read number %name"
//% weight=19
int settingsReadNumber(String name) {
    initFileSystem();
    MicroBitFileSystem::defaultFileSystem->createDirectory("settings");
    MicroBitFile f("settings/" + MSTR(name), MB_READ);
    if (!f.isValid()) 
        return -1;
    ManagedString v;
    ManagedString buff;
    do {
        buff = f.read(32);        
        v = v + buff;
    } while(buff.length() > 0);
    return atoi(v.toCharArray());
}

/**
*
*/
//% weight=0 advanced=true
int fsOpen(String path) {
    initFileSystem();
    return MicroBitFileSystem::defaultFileSystem->open(MSTR(path).toCharArray(), MB_READ|MB_WRITE|MB_CREAT);
}

/**
*
*/
//% weight=0 advanced=true
int fsFlush(int fd) {
    if (fd < 0) return MICROBIT_NOT_SUPPORTED;

    initFileSystem();
    return MicroBitFileSystem::defaultFileSystem->flush(fd);
}

/**
*
*/
//% weight=0 advanced=true
int fsClose(int fd) {
    if (fd < 0) return MICROBIT_NOT_SUPPORTED;

    initFileSystem();
    return MicroBitFileSystem::defaultFileSystem->close(fd);
}

/**
*
*/
//% weight=0 advanced=true
int fsRemove(String name) {
    initFileSystem();
    return MicroBitFileSystem::defaultFileSystem->remove(MSTR(name).toCharArray());
}

/**
*
*/
//% weight=0 advanced=true
int fsSeek(int fd, int offset, int flags) {
    if (fd < 0) return MICROBIT_NOT_SUPPORTED;
    if (offset < 0) return MICROBIT_INVALID_PARAMETER;

    initFileSystem();
    return MicroBitFileSystem::defaultFileSystem->seek(fd, offset, flags);
}

/**
*
*/
//% weight=0 advanced=true
int fsWriteString(int fd, String text) {
    if (fd < 0) return MICROBIT_NOT_SUPPORTED;

    initFileSystem();
    ManagedString t = MSTR(text);
    return MicroBitFileSystem::defaultFileSystem->write(fd, (uint8_t*)t.toCharArray(), t.length());
}

/**
*
*/
//% weight=0 advanced=true
int fsWriteBuffer(int fd, Buffer buffer) {
    if (fd < 0) return MICROBIT_NOT_SUPPORTED;

    initFileSystem();
    return MicroBitFileSystem::defaultFileSystem->write(fd, PXT_BUFFER_DATA(buffer), buffer->length);
}

/**
*/
//% weight=0 advanced=true
Buffer fsReadBuffer(int fd, int length) {
    if (fd < 0 || length < 0) 
        return mkBuffer(NULL, 0);

    initFileSystem();
    Buffer buf = mkBuffer(NULL, length);

    int ret = MicroBitFileSystem::defaultFileSystem->read(fd, PXT_BUFFER_DATA(buf), buf->length);

    if (ret < 0) return mkBuffer(NULL, 0);
    else if (ret != length) {
        auto sbuf = mkBuffer(PXT_BUFFER_DATA(buf), ret);
        decrRC(buf);
        return sbuf;
    }
    else return buf;
}

/**
*
*/
//% weight=0 advanced=true
int fsRead(int fd) {
    if (fd < 0) return MICROBIT_NOT_SUPPORTED;
    initFileSystem();

    char c[1];    
    int ret = MicroBitFileSystem::defaultFileSystem->read(fd, (uint8_t*)&c, 1);
    if (ret != 1) return ret;
    else return c[0];
}

}
