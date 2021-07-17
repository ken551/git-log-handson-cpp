#include <ctime>
#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
using namespace std;

void myTrim(unsigned char* str);
int myStrLen(unsigned char* str);
void setGitRoot(char* buf, char* argv);
void myCompress(const char* filename, unsigned char* dst);
void myUncompress(const char* filename, unsigned char* dst);
void printLog(char *commitID, char *str);

const uLongf BUF_SIZE = 1024;

int main(int argc, char** argv) {
    char commitID[41];
    unsigned char* str;
    str = (unsigned char*)malloc(sizeof(unsigned char) * BUF_SIZE);
    char buf[100];
    setGitRoot(buf, argv[1]);
    //get HEAD branch
    FILE* head = fopen(strcat(buf, "HEAD"), "r");
    if(head == NULL) {
        cout << "error: HEAD ref not found" << endl;
        return -1;
    }
    fgets((char*)str, BUF_SIZE, head);
    if(strcmp(strtok((char *)str, " "), "ref:")){
        cout << "error: invalid HEAD file" << endl;
    }
    fclose(head);    

    //HEAD branch
    setGitRoot(buf, argv[1]);
    strcat(buf, strtok(NULL,"\n")); 
    FILE* headBranch = fopen(buf, "r");
    if(headBranch == NULL) {
        cout << "error: branch file not found" << endl;
    }
    memset(str, 0, BUF_SIZE * sizeof(unsigned char));
    fgets((char *)str, BUF_SIZE, headBranch);
    fclose(headBranch);
    myTrim(str); //latest commit ID
    for(int i=0;i<40;i++){
        commitID[i] = str[i];
    }
    commitID[40]= 0;

    //get commit object
    char folderName[3] = {(char)str[0], (char)str[1], 0};
    setGitRoot(buf, argv[1]);
    strcat(buf, "objects/");
    strcat(buf, folderName);
    strcat(buf, "/");
    char fileName[myStrLen(str)-1];
    strncpy(fileName, (const char*)&str[2], myStrLen(str)-1);
    fileName[myStrLen(str)] = 0;
    strcat(buf, fileName);

    memset(str, 1, BUF_SIZE * sizeof(unsigned char));
    myUncompress(buf, str);
    int i=0;
    while(str[i] != 1){
        if(str[i] == 0){
            str[i] = ' ';
        }
        i ++;
    }
    printLog(commitID, (char*)str);
    return 0;
}

void myTrim(unsigned char* str){
    int i = 0;
    while(str[i] != 0){
        i++;
    }
    str[i-1] = 0;
}

int myStrLen(unsigned char* str){
    int i = 0;
    while(str[i] != 0){
        i++;
    }
    return i-1;
}


void setGitRoot(char* buf, char* argv){
    memset(buf, 0, 100*sizeof(char));
    strcat(buf, argv);
    strcat(buf, "/.git/");
}

void printLog(char *commitID, char *str){
    //get commit message
    char* commitMessage;
    commitMessage = strstr((char*)str, "\n\n");
    commitMessage += 2;

    //get time
    char* commitTime;
    commitTime = strstr((char*)str, "> ");
    commitTime += 2;
    time_t hoge = strtol(strtok(commitTime, " "), nullptr, 10);
    char* timeZone = strtok(NULL, "\n");

    char* author;
    author = strstr((char*)str, "author ");
    author += 7;

    //output commit info
    cout << "commit " << commitID << endl;
    cout << "Author: " << author << endl;
    cout << "Date: " << strtok(ctime(&hoge), "\n") << " " << timeZone << endl;
    cout << endl;
    cout << commitMessage << endl;
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