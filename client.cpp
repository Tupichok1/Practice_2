#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main() {
    int ssocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(7432);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if ( connect(ssocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0 ) {
        cout << "failed to connect" << endl;
        return 1;
    }
    else {
        cout << "connected" << endl;
    }
    char buffer[1000];
    string buff;
    while ( true ) {
        getline(cin, buff);
        if ( buff == "exit" ) {
            send(ssocket, buff.c_str(), buff.length(), 0);
            return 0;
        }
        send(ssocket, buff.c_str(), buff.length(), 0);
        int sizeOfMessage = recv(ssocket, buffer, sizeof(buffer), 0);
        buffer[sizeOfMessage] = '\0';
        cout << buffer << endl;
        buffer[0] = '\0';
    }
}