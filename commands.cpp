#include "commands.h"

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

Table schemaJson;

int countOfTuples(hashTable dataBase) {
    string tablesName = schemaJson.tableName;
    hashTable tableOfColumns = schemaJson.tableColumns;
    int maxSize = 0;
    for ( int i = 0; i < tableOfColumns.sizeOfKey(tablesName); i ++ ) {
        if ( dataBase.sizeOfKey(tableOfColumns.findByIndex(tablesName, i)) > maxSize ) {
            maxSize = dataBase.sizeOfKey(tableOfColumns.findByIndex(tablesName, i));
        }
    }
    return maxSize;
}

void printTable(hashTable database) {
    for ( int i = 0; i < schemaJson.tableColumns.sizeOfKey(schemaJson.tableName); i ++ ) {
        for ( int j = 0; j < database.sizeOfKey(schemaJson.tableColumns.findByIndex(schemaJson.tableName, i)); j ++ ) {
            cout << database.findByIndex(schemaJson.tableColumns.findByIndex(schemaJson.tableName, i), j) << " ";
        }
        cout << '\n';
    }
}

void cleanLine(string &line) {
    string newLine;
    for ( auto i : line ) {
        if ( i == '(' or i == ')' or i == ',' or int(i) == 39 ) {
            continue;
        }
        else {
            newLine += i;
        }
    }
    line = newLine;
}

void parseJsonFile() {
    json objJson;
    fstream fout("awd.json");
    fout >> objJson;
    fout.close();
    schemaJson.tuplesLimit = objJson["tuples_limit"];
    schemaJson.nameOfSchema = objJson["name"];
    schemaJson.tableColumns.initTable();
    for ( auto& [tableName, columns] : objJson["structure"].items() ) {
        schemaJson.tableNames.addLastElement(tableName);
        for ( auto nameOfColumn : columns ) {
            schemaJson.tableColumns.addLastValue(nameOfColumn, tableName);
        }
        schemaJson.tableColumns.addLastValue(tableName + "_pk", tableName);
    }
}

void parseLineDataBase(hashTable& dataBase, string word) {
    string newLine = "";
    int index = 0;
    for ( int i = 0; i < word.size(); i ++ ) {
        if ( word[i] == ',' ) {
            dataBase.addLastValue(newLine, schemaJson.tableColumns.findByIndex(schemaJson.tableName, index++));
            if ( index == schemaJson.tableColumns.sizeOfKey(schemaJson.tableName) ) {
                index = 0;
            }
            newLine = "";
        }
        else {
            newLine += word[i];
        }
    }
    dataBase.addLastValue(newLine, schemaJson.tableColumns.findByIndex(schemaJson.tableName, index++));
}

void loadFromFileDataBase(hashTable &dataBase, string filePath) {
    string line, word;
    ifstream fileInput(filePath);
    while ( getline(fileInput, line) ) {
        parseLineDataBase(dataBase, line);
        /*int index = 0;
        replace(line.begin(), line.end(), ',', ' ');
        stringstream sstream(line);
        while ( sstream >> word ) {
            dataBase.addLastValue(word, schemaJson.tableColumns.findByIndex(schemaJson.tableName, index++));
            if ( index == schemaJson.tableColumns.sizeOfKey(schemaJson.tableName) ) {
                index = 0;
            }
        } */
    }
}

void saveToFileDataBase(hashTable dataBase, string filePath) {
    string tablesName = schemaJson.tableName;
    hashTable tableOfColumns = schemaJson.tableColumns;
    ofstream fileInput(filePath);
    for ( int i = 0; i < countOfTuples(dataBase); i ++ ) {
        for ( int j = 0; j < tableOfColumns.sizeOfKey(tablesName); j ++ ) {
            if ( dataBase.sizeOfKey(tableOfColumns.findByIndex(tablesName, j)) <= i ) {
                fileInput << ",";
            }
            else {
                fileInput << dataBase.findByIndex(tableOfColumns.findByIndex(tablesName, j), i) << ",";
            }
        }
        fileInput << '\n';
    } 
} 

bool insertElement(hashTable &dataBase, stringstream& sstream) {
    string word;
    sstream >> word;
    if ( word != "INTO" ) {
        cout << "wrong command" << endl;
        return false;
    }
    sstream >> schemaJson.tableName;
    if ( schemaJson.tableNames.findElementNode(schemaJson.tableName) == false ) {
        cout << "wrong table name" << endl;
        return false;
    }
    sstream >> word;
    if ( word != "VALUES" ) {
        cout << "wrong command" << endl;
        return false;
    }
    loadFromFileDataBase(dataBase, schemaJson.nameOfSchema + "/" + schemaJson.tableName + "/" + to_string(schemaJson.numberOfCSV ) + ".csv");
    while ( schemaJson.tuplesLimit <= countOfTuples(dataBase) ) {
        dataBase.initTable();
        loadFromFileDataBase(dataBase, schemaJson.nameOfSchema + "/" + schemaJson.tableName + "/" + to_string(schemaJson.numberOfCSV++ ) + ".csv");
    }
    if ( dataBase.sizeOfKey(schemaJson.tableName + "_pk") == 1 ) {
        dataBase.addLastValue(to_string(schemaJson.numberOfCSV), schemaJson.tableName + "_pk");
    }
    int index = 0;
    while ( sstream >> word ) {
        cleanLine(word);
        if ( word == "" ) {
            index += 1;
            continue;
        }
        dataBase.addLastValue(word, schemaJson.tableColumns.findByIndex(schemaJson.tableName, index++));
        if ( index == schemaJson.tableColumns.sizeOfKey(schemaJson.tableName) - 1 ) {
            index = 0;
        }
    }
    return true;
}

void selectWhere::takeNames(string word, string& tableName, string& columnName) {
    replace(word.begin(), word.end(), '.', ' ');
    stringstream takeName(word);
    takeName >> tableName;
    if ( nameOfKeys.findElementNode(tableName) == false ) {
        nameOfKeys.addLastElement(tableName);
    }
    takeName >> columnName;
}
void selectWhere::loadFromCSV(hashTable& table, string nameOfTable) {
    table.initTable();
    schemaJson.tableName = nameOfTable;
    schemaJson.numberOfCSV = 1;
    while ( fs::exists(schemaJson.nameOfSchema + "/" + nameOfTable + "/" + to_string(schemaJson.numberOfCSV) + ".csv") ) {
        loadFromFileDataBase(table, schemaJson.nameOfSchema + "/" + nameOfTable + "/" + to_string(schemaJson.numberOfCSV) + ".csv");
        schemaJson.numberOfCSV += 1;
    }
    schemaJson.numberOfCSV = 1;
}
void selectWhere::fillCrossJoin(singleList namesOfColumns) {
    crossJoin.initTable();
    for ( int i = 0; i < nameOfKeys.sizeOfList(); i ++ ) {
        loadFromCSV(dataBase, nameOfKeys.findElementByIndex(i));
        for ( int j = 1; j < dataBase.sizeOfKey(namesOfColumns.findElementByIndex(i)); j ++ ) {
            if ( resultOfWhere.findElement(schemaJson.tableName, to_string(j)) == false ) {
                continue;
            }
            string value = dataBase.findByIndex(namesOfColumns.findElementByIndex(i), j);
            crossJoin.addLastValue(value, schemaJson.tableName);
        }
        dataBase.initTable();
    }
}
void selectWhere::makeCrossJoin() {
    int index = 0;
    while ( index != nameOfKeys.sizeOfList() - 1 ) {
        keyOfCrossJoin += "a";
        for ( int i = 0; i < crossJoin.sizeOfKey(nameOfKeys.findElementByIndex(index)); i ++ ) {
            for ( int j = 0; j < crossJoin.sizeOfKey(nameOfKeys.findElementByIndex(index + 1)); j ++ ) {
                string firstValue = crossJoin.findByIndex(nameOfKeys.findElementByIndex(index), i);
                string secondValue = crossJoin.findByIndex(nameOfKeys.findElementByIndex(index + 1), j);
                crossJoin.addLastValue(firstValue + " " + secondValue, keyOfCrossJoin);
            }
        }
        index += 1;
        nameOfKeys.changeElementByIndex(keyOfCrossJoin, index);
    }
} 
void selectWhere::printCrossJoin() {
    for ( int i = 0; i < crossJoin.sizeOfKey(keyOfCrossJoin); i ++ ) {
        cout << crossJoin.findByIndex(keyOfCrossJoin, i) << '\n';
    }
}
void selectWhere::parseLine(stringstream& sstream, string word) {
    while ( sstream >> word ) {
        cleanLine(word);
        nameOfTables.addLastElement(word);
        sstream >> word;
        if ( word != "=" ) {
            cout << "wrong command";
            return;
        }
        sstream >> word;
        cleanLine(word);
        valueForDivide.addLastElement(word);
        sstream >> word;
        operations.addLastElement(word);
    }
}
void selectWhere::crossOfSets(hashTable &linesForCrossJoin) {
    string nameOfTable, nameOfColumn;
    singleList linesForSave;
    string value = valueForDivide.findElementByIndex(0);
    takeNames(nameOfTables.findElementByIndex(0), nameOfTable, nameOfColumn);
    loadFromCSV(dataBase, nameOfTable);
    for ( int i = 0; i < linesForCrossJoin.sizeOfKey(nameOfTable); i ++ ) {
        linesForSave.addLastElement(linesForCrossJoin.findByIndex(nameOfTable, i));
    }
    if ( linesForSave.sizeOfList() != 0 ) {
        for ( int i = 0; i < linesForCrossJoin.sizeOfKey(nameOfTable); i ++ ) {
            linesForCrossJoin.deleteElement(linesForCrossJoin.findByIndex(nameOfTable, i), nameOfTable);
        }
    }
    if ( value.find(".") == string::npos ) {
        for ( int i = 1; i < dataBase.sizeOfKey(nameOfColumn); i ++ ) {
            if ( linesForSave.findElementNode(to_string(i)) == false and linesForSave.sizeOfList() != 0  ) {
                continue;
            } 
            if ( dataBase.findByIndex(nameOfColumn, i) == value ) {
                linesForCrossJoin.addLastValue(to_string(i), nameOfTable);
            }
        }
    }
    else {
        string secondNameOfTable, secondNameOfColumn;
        hashTable secondTable;
        singleList secondLineForSave;
        takeNames(value, secondNameOfTable, secondNameOfColumn);
        loadFromCSV(secondTable, secondNameOfTable);
        for ( int i = 1; i < secondTable.sizeOfKey(secondNameOfColumn); i ++ ) {
            if ( dataBase.findByIndex(nameOfColumn, i) == secondTable.findByIndex(secondNameOfColumn, i) ) {
                if ( secondNameOfTable == nameOfTable ) {
                    linesForCrossJoin.addLastValue(to_string(i), nameOfTable);
                }
                else {
                    linesForCrossJoin.addLastValue(to_string(i), nameOfTable);
                    linesForCrossJoin.addLastValue(to_string(i), secondNameOfTable);
                }
            }
        }
    }
    nameOfTables.deleteFirstNode();
    valueForDivide.deleteFirstNode();
}
void selectWhere::operationsAtOR() {
    hashTable resultOfOR;
    resultOfOR.initTable();
    crossOfSets(resultOfOR);
    while ( operations.sizeOfList() != 0 ) {
        string operation = operations.findElementByIndex(0);
        if ( operation == "AND" ) {
            crossOfSets(resultOfOR);
            operations.deleteFirstNode();
        }
        if ( operation == "OR" ) {
            operations.deleteFirstNode();
            operationsAtOR();
        }
    }
    for ( int i = 0; i < nameOfKeys.sizeOfList(); i ++ ) {
        string key = nameOfKeys.findElementByIndex(i);
        for ( int j = 0; j < resultOfOR.sizeOfKey(key); j ++ ) {
            resultOfWhere.addLastValue(resultOfOR.findByIndex(key, j), key);
        }
    }
}
void selectWhere::makeWhere() {
    operations.deleteLastNode();
    crossOfSets(resultOfWhere);
    while ( operations.sizeOfList() != 0 ) {
        string operation = operations.findElementByIndex(0);
        if ( operation == "AND" ) {
            crossOfSets(resultOfWhere);
            operations.deleteFirstNode();
        }
        if ( operation == "OR" ) {
            operations.deleteFirstNode();
            operationsAtOR();
        }
    }
    /*for ( int i = 0; i < nameOfKeys.sizeOfList(); i ++ ) {
        string key = nameOfKeys.findElementByIndex(i);
        for ( int j = 0; j < resultOfWhere.sizeOfKey(key); j ++ ) {
            cout << resultOfWhere.findByIndex(key, j) << " ";
        }
        cout << '\n';
    } */
}

bool deleteElement(hashTable& dataBase, stringstream &sstream) {
    string word, tableName;
    sstream >> word;
    if ( word != "FROM" ) {
        cout << "wrong command" << endl;
        return false;
    }
    sstream >> tableName;
    if ( schemaJson.tableNames.findElementNode(tableName) == false ) {
        cout << "wrong table name" << endl;
        return false;
    }
    sstream >> word;
    if ( word != "WHERE" ) {
        cout << "wrong command" << endl;
        return false;
    }
    else {
        selectWhere DELETE;
        DELETE.resultOfWhere.initTable();
        DELETE.parseLine(sstream, word);
        DELETE.makeWhere();
        singleList linesForDelete;
        for ( int i = 0; i < DELETE.resultOfWhere.sizeOfKey(tableName); i ++ ) {
            linesForDelete.addLastElement(DELETE.resultOfWhere.findByIndex(tableName, i));
        }
        int reductionFactor = 0;
        while ( fs::exists(schemaJson.nameOfSchema + "/" + tableName + "/" + to_string( schemaJson.numberOfCSV ) + ".csv") ) {
            loadFromFileDataBase(dataBase, schemaJson.nameOfSchema + "/" + tableName + "/" + to_string( schemaJson.numberOfCSV ) + ".csv");
            int tuples = countOfTuples(dataBase);
            for ( int i = 1; i < tuples; i ++ ) {
                if ( linesForDelete.findElementNode(to_string(i)) == false ) {
                    continue;
                }
                for ( int j = 0; j < schemaJson.tableColumns.sizeOfKey(schemaJson.tableName) - 1; j ++ ) {
                    string key = schemaJson.tableColumns.findByIndex(tableName, j);
                    dataBase.deleteElement(dataBase.findByIndex(key, i - reductionFactor), key);
                }
                reductionFactor += 1;
            }
            saveToFileDataBase(dataBase, schemaJson.nameOfSchema + "/" + tableName + "/" + to_string( schemaJson.numberOfCSV++ ) + ".csv");
            dataBase.initTable();
        } 
    }
    return true;
}

bool findTableColumnName(string word, string& tableName, string& nameOfColumn) {
    replace(word.begin(), word.end(), '.', ' ');
    replace(word.begin(), word.end(), ',', ' ');
    stringstream sstream(word);
    sstream >> tableName;
    sstream >> nameOfColumn;
    if ( schemaJson.tableNames.findElementNode(tableName) == false ) {
        cout << "wrong table name";
        return false;
    }
    if ( schemaJson.tableColumns.findElement(tableName, nameOfColumn) == false ) {
        cout << "wrong column name";
        return false;
    }
    return true;
}

void selectElements(hashTable& dataBase, stringstream& sstream) {
    string word;
    singleList namesOfTables, namesOfColumns;
    sstream >> word;
    while ( word != "FROM" ) {
        string columnName, tableName;
        if ( findTableColumnName(word, tableName, columnName) == false ) {
            return;
        }
        namesOfTables.addLastElement(tableName);
        namesOfColumns.addLastElement(columnName);
        if ( !( sstream >> word ) ) {
            cout << "wrong command";
            return;
        }
    }
    for ( int i = 0; i < namesOfTables.sizeOfList(); i ++ ) {
        sstream >> word;
        cleanLine(word);
        if ( schemaJson.tableNames.findElementNode(word) == false ) {
            cout << "wrong table name";
            return;
        }
    }
    if ( !(sstream >> word) ) {
        selectWhere CJ;
        CJ.resultOfWhere.initTable();
        CJ.nameOfKeys = namesOfTables;
        for ( int i = 0; i < namesOfTables.sizeOfList(); i ++ ) {
            CJ.loadFromCSV(dataBase, namesOfTables.findElementByIndex(i));
            string key = namesOfTables.findElementByIndex(i);
            for ( int j = 1; j < dataBase.sizeOfKey(namesOfColumns.findElementByIndex(i)); j ++ ) {
                CJ.resultOfWhere.addLastValue(to_string(j), key);
            }
        }
        CJ.fillCrossJoin(namesOfColumns);
        CJ.makeCrossJoin();
        CJ.printCrossJoin(); 
        return;
    }
    if ( word != "WHERE" ) {
        cout << "wrong command";
        return;
    }
    if ( word == "WHERE" ) {
        selectWhere CJ;
        CJ.resultOfWhere.initTable();
        CJ.parseLine(sstream, word);
        CJ.makeWhere();
        CJ.fillCrossJoin(namesOfColumns);
        CJ.makeCrossJoin();
        CJ.printCrossJoin();
    } 
}

string SQLquery(string commandLine) {
    string command;
    stringstream sstream(commandLine);
    sstream >> command;
    hashTable dataBase;
    createDirectory();
    parseJsonFile();
    dataBase.initTable();
    if ( command == "INSERT" ) {
        if ( insertElement(dataBase, sstream) == false ) {
            return "unsuccesfully";
        }
        saveToFileDataBase(dataBase, schemaJson.nameOfSchema + "/" + schemaJson.tableName + "/" + to_string(schemaJson.numberOfCSV) + ".csv"); 
    }
    else if ( command == "DELETE" ) {
        if ( deleteElement(dataBase, sstream) == false ) {
            return "unsuccesfully";
        }
    }
    else if ( command == "SELECT" ) {
        selectElements(dataBase, sstream);
    }
    else {
        cout << "wrong command" << endl;
        return "unsuccesfully";
    }
    cin.sync();
    return "succesfully";
}