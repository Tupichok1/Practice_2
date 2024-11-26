#include <sstream>
#include "singleLinkedList.h"
#include "hashMap.h"
#include "createDirectory.h"

struct Table {
    int numberOfCSV = 1;
    int tuplesLimit;
    std::string tableName;
    std::string nameOfSchema;
    singleList tableNames;
    hashTable tableColumns;
};

int countOfTuples(hashTable dataBase);

void printTable(hashTable database);

void cleanLine(std::string &line);
void parseJsonFile();

void loadFromFileDataBase(hashTable &dataBase, std::string filePath);

void saveToFileDataBase(hashTable dataBase, std::string filePath);

bool insertElement(hashTable &dataBase, std::stringstream& sstream);

struct selectWhere {
    std::string keyOfCrossJoin = "";
    singleList nameOfKeys, nameOfTables, valueForDivide, operations;
    hashTable resultOfWhere;
    hashTable crossJoin, dataBase;
    void takeNames(std::string word, std::string& tableName, std::string& columnName);
    void loadFromCSV(hashTable& table, std::string nameOfTable);
    void fillCrossJoin(singleList namesOfColumns);
    void makeCrossJoin();
    void printCrossJoin();
    void parseLine(std::stringstream& sstream, std::string word);
    void crossOfSets(hashTable &linesForCrossJoin);
    void operationsAtOR();
    void makeWhere();
};

bool deleteElement(hashTable& dataBase, std::stringstream &sstream);

bool findTableColumnName(std::string word, std::string& tableName, std::string& nameOfColumn);

void selectElements(hashTable& dataBase, std::stringstream& sstream);

std::string SQLquery(std::string commandLine);