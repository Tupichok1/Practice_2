#include "hashMap.h"
using namespace std;

int hashTable::hashFunc(string key) {
    int sum = 0;
    for ( auto i : key ) {
        sum += int(i);
    }
    return sum % SIZEHM;
}
void hashTable::initTable() {
    table = new hashNode*[SIZEHM];
    for ( int i = 0; i < SIZEHM; i ++ ) {
        table[i] = NULL;
    }
}
void hashTable::addLastValue(string value, string key) {
    if ( value == "" ) {
        return;
    }
    int hashed = hashFunc(key);
    hashNode* newNode = new hashNode{key, value, NULL};
    if ( table[hashed] == NULL ) {
        table[hashed] = newNode;
        return;
    }
    else {
        hashNode* last = table[hashed];
        while ( last -> next != NULL ) {
            last = last -> next;
        }
        last -> next = newNode;
    }
}
int hashTable::sizeOfKey(string key) {
    int size = 0;
    int hashed = hashFunc(key);
    hashNode* entry = table[hashed];
    while ( entry != NULL ) {
        size += 1;
        entry = entry -> next;
    }
    return size;
}
void hashTable::deleteElement(string value, string key) {
    if ( value == "NULL" ) {
        return;
    }
    int hashed = hashFunc(key);
    hashNode* deleteNode = table[hashed];
    if ( deleteNode -> value == value ) {
        table[hashed] = NULL;
        delete deleteNode;
        return;
    }
    deleteNode = table[hashed] -> next;
    hashNode* prev = table[hashed];
    while ( deleteNode -> value != value ) {
        prev = prev -> next;
        deleteNode = deleteNode -> next;
    }
    prev -> next = deleteNode -> next;
    delete deleteNode;
}
void hashTable::deleteElementByIndex(int index, string key) {
    if ( sizeOfKey(key) <= index ) {
        return;
    }
    int hashed = hashFunc(key);
    hashNode* deleteNode = table[hashed];
    deleteNode = table[hashed] -> next;
    hashNode* prev = table[hashed];
    int i = 0;
    while ( i != index ) {
        prev = prev -> next;
        deleteNode = deleteNode -> next;
        i += 1;
    }
    prev -> next = deleteNode -> next;
    delete deleteNode;
}
void hashTable::printElement(string key) {
    int hashed = hashFunc(key);
    hashNode* entry = table[hashed];
    while ( entry != NULL ) {
        cout << entry -> value << " ";
        entry = entry -> next;
    }
}
string hashTable::findByIndex(string key, int index) {
    if ( sizeOfKey(key) <= index ) {
        return "NULL";
    }
    int hashed = hashFunc(key);
    hashNode* entry = table[hashed];
    for ( int i = 0; i <= index; i ++ ) {
        if ( i == index ) {
            return entry -> value;
        }
        entry = entry -> next;
    }
    return "NULL";
}
bool hashTable::findElement(string key, string value) {
    int hashed = hashFunc(key);
    hashNode* newNode = table[hashed];
    if ( newNode == NULL ) {
        return false;
    }
    if ( newNode -> value == value ) {
        return true;
    }
    while ( newNode -> value != value ) {
        newNode = newNode -> next;
        if ( newNode == NULL ) {
            return false;
        }
    }
    return true;
}
void hashTable::parseHashMap(string line) {
    string newLine;
    string key, value;
    for ( size_t i = 0; i < line.size(); i ++ ) {
        if ( line[i] == '-' ) {
            key = newLine;
            newLine = "";
            continue;
        }
        if ( line[i] == ',' ) {
            continue;
        }
        if ( line[i] == ' ' ) {
            addLastValue(newLine, key);
            newLine = "";
            continue;
        }
        newLine += line[i];
    }
    addLastValue(newLine, key);
}
void hashTable::loadFromFileHash(string file) {
    string line;
    ifstream fileInput(file);
    while ( getline(fileInput, line) ) {
        parseHashMap(line);
    }
}