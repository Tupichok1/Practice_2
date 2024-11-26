#include "createDirectory.h"

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

void createDirectory() {
    json objJson;
    fstream fileInput("awd.json");
    fileInput >> objJson;
    fileInput.close();
    string nameOfTable = objJson["name"];
    if ( fs::exists(nameOfTable) ) {
        return;
    }
    fs::create_directory(nameOfTable);
    for ( auto& [tableName, columns] : objJson["structure"].items() ) {
        string path = nameOfTable + "/" + tableName;
        string CSVPath = path + "/" + "1.csv";
        string pk_sequence = tableName + "_pk";
        fs::create_directory(path);
        ofstream csv(CSVPath);
        for ( auto column : columns ) {
            csv << string(column) << ",";
        }
        csv << string(pk_sequence);
    }
}