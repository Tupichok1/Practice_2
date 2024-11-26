#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <string.h>
#include "commands.h"

using namespace std;

void client(int ssocket) {
    string result;
    mutex lock;
    char message[1000];
    int sizeOfMessage;
    while ( true ) {
        lock_guard<mutex> guard(lock);
        string line = "";
        sizeOfMessage = recv(ssocket, message, sizeof(message), 0);
        for ( auto i : message ) {
            line += i;
        }
        if ( line == "exit" or sizeOfMessage <= 0 ) {
            cout << "disconnected" << endl;
            return;
        }
        message[sizeOfMessage] = '\0';
        result = SQLquery(message);
        message[0] = '\0';
        send(ssocket, result.c_str(), result.length(), 0);
    }
    close(ssocket);
}

int main() {
    int ssocket = socket(AF_INET, SOCK_STREAM, 0);
    if ( ssocket == -1 ) {
        cout << "socket creation failed" << endl;
        return 1;
    }
    struct sockaddr_in adr;
    adr.sin_family = AF_INET;
    adr.sin_port = htons(7432);
    adr.sin_addr.s_addr = INADDR_ANY;
    if ( bind(ssocket, (struct sockaddr * ) & adr, sizeof(adr)) < 0 ) {
        cout << "error calling bind" << endl;
        return 1;
    }
    if ( listen(ssocket, 3) < 0 ) {
        cout << "error calling listen" << endl;
        return 1;
    }
    else {
        cout << "connected" << endl;
    }
    while ( true ) {
        int clientSocket = accept(ssocket, NULL, NULL);
        if ( clientSocket <= 0 ) {
            cout << "error calling accept" << endl;
            return 1;
        }
        thread newThread(client, clientSocket);
        newThread.detach(); 
    }
    close(ssocket);
}