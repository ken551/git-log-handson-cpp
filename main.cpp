#include <zlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
using namespace std;

void myCompress(const char* filename, unsigned char* dst);
void myUncompress(const char* filename, unsigned char* dst);
const uLongf BUF_SIZE = 1024;

int main(int argc, char** argv) {
    unsigned char* str;
    str = (unsigned char*)malloc(sizeof(unsigned char) * BUF_SIZE);
    myCompress(argv[1], str);

    cout << str << endl; //print result

    //output result to a file
    FILE* write = fopen("result.txt", "w");
    fprintf(write, "%s", str);
    fclose(write);
    free(str);
    str = (unsigned char*)malloc(sizeof(unsigned char) * BUF_SIZE);
    myUncompress("result.txt", str);
    cout << str << endl;
    return 0;
}

void myCompress(const char* filename, unsigned char* dst){
    struct stat filestat;
    unsigned char* sourceptr;
    uLongf destlen;

    // compressing file
    FILE* stream = fopen(filename, "r");
    if(stream == NULL) {
        cout << "error: file not found" << endl;
        return;
    }
    int fd = fileno(stream);
    if(fstat(fd, &filestat) == 0) {
        sourceptr = (unsigned char*)mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
        destlen = BUF_SIZE;
        compress(dst, &destlen, sourceptr, filestat.st_size);
    }
    fclose(stream);
}

void myUncompress(const char* filename, unsigned char* dst){
    struct stat filestat;
    unsigned char* sourceptr;
    uLongf bufSize = BUF_SIZE;
    FILE* stream = fopen(filename, "r");
    if(stream == NULL) {
        cout << "error: file not found" << endl;
        return;
    }
    int fd = fileno(stream);
    if(fstat(fd, &filestat) == 0) {
        sourceptr = (unsigned char*)mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
        uncompress(dst, &bufSize, sourceptr, filestat.st_size);
    }
    fclose(stream);
}