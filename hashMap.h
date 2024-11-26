#include <string>
#include <iostream>
#include <fstream>
#include "singleLinkedList.h"
#define SIZEHM 80

struct hashNode {
    std::string key;
    std::string value;
    hashNode* next;
};
struct hashTable {
    hashNode** table;
    int hashFunc(std::string key);
    void initTable();
    void addLastValue(std::string value, std::string key);
    int sizeOfKey(std::string key);
    void deleteElement(std::string value, std::string key);
    void deleteElementByIndex(int index, std::string key);
    void printElement(std::string key);
    std::string findByIndex(std::string key, int index);
    bool findElement(std::string key, std::string value);
    void parseHashMap(std::string line);
    void loadFromFileHash(std::string file);
};
