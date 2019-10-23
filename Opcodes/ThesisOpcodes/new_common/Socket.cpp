#include "Socket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <json/json.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
const int PORT = 2222;

using namespace std;
template <typename T>
string getJsonString(T* data, const char* label, size_t elementCount);

template <typename T>
void sendData(T* data, const char* label, size_t elementCount)
{
    string jsonString = getJsonString<T>(data, label, elementCount);
    int sock = initSocket();
    socketSend(sock, jsonString.c_str());
}

void sendDataFloat(float* data, const char* label, size_t elementCount)
{
    sendData<float>(data, label, elementCount);
}

void sendDataDouble(double* data, const char* label, size_t elementCount)
{
    sendData<double>(data, label, elementCount);
}

int initSocket()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    return sock;
}

template <typename T>
string getJsonString(T* data, const char* label, size_t elementCount)
{
    Json::Value root;
    Json::Value array;
    root["label"] = label;
    for (int i = 0; i < elementCount; ++i) {
        array[i] = data[i];
    }

    root["data"] = array;

    return root.toStyledString();
}

void socketSend(int sock, const char* str)
{
    string cString = str;
    string s = "start" + cString + "end";
    const char* output = s.c_str();

    send(sock, output, strlen(output), 0);
}

template void sendData(double* data, const char* label, size_t elementCount);
template void sendData(float* data, const char* label, size_t elementCount);
