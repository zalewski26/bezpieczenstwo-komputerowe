#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

double occurance_polish[] = {8.91, 1.47, 3.96, 3.25, 7.66, 0.3, 1.42, 1.08, 8.21, 2.28, 3.51, 2.1, 2.8, 5.52, 7.75, 
3.13, 0.14, 4.69, 4.32, 3.98, 2.5, 0.04, 4.65, 0.02, 3.76, 5.64};

double computeFittingQuotient(std::vector<int> v){
    double result = 0.0;
    double columns;
    for (int i = 0; i < 26; i++){
        double counter = 0;
        for (int j = 0; j < v.size(); j++){
            if (v[j] == i + 65 || v[j] == i + 97)
                counter += 1.0;
        }
        result += std::abs((counter / (double)v.size()) - (occurance_polish[i] / 100));
    }
    for (int i = 0; i < v.size(); i++){
        if (!((v[i] >= 65 && v[i] <= 90) || (v[i] >= 97 && v[i] <= 122))){
            result += 2 / (double)v.size();
        }
    }
    return result;
}

std::vector<int>* readFile(std::string filename, int size)
{
    std::vector<int>* arr = new std::vector<int>[size];
    std::ifstream file;
    file.open (filename);
    if (!file.is_open()) 
        return nullptr;

    for (int i = 0; i < size; i++){
        std::string word;
        while (file >> word){
            if (word.find(":") != std::string::npos){
                break;
            }
        }
        while (file >> word){
            if (word == "kryptogramm")
                break;
            arr[i].push_back(std::stoi(word, 0, 2));
        }
    }
    return arr;
}

void getResult(std::string filename, std::vector<int>& v){
    std::ifstream file;
    file.open (filename);
    if (!file.is_open()) 
        return;
    std::string word;
    while (file >> word){
        v.push_back(std::stoi(word, 0, 2));
    }
}

int main(int argc, char** argv){
    int numOfCrypt = std::stoi(argv[1]);
    std::vector<int>* arr = readFile(argv[2], numOfCrypt);
    std::vector<int> result;
    getResult(argv[3], result);

    int lenOfCrypt = result.size();
    std::vector<int>* columns = new std::vector<int>[lenOfCrypt];
    
    for (int k = 0; k < lenOfCrypt; k++){
        int* specialCounter = new int[numOfCrypt];
        for (int i = 0; i < numOfCrypt; i++){
            if (k < arr[i].size())
                columns[k].push_back(arr[i][k]);
            else 
                columns[k].push_back(1000);
            specialCounter[i] = 0;
        }
        for (int i = 0; i < numOfCrypt; i++){
            for (int j = 0; j < numOfCrypt; j++){
                if (i != j && columns[k][i] != 1000 && columns[k][j] != 1000){
                    if ((( columns[k][i] ^ columns[k][j]) & 0b1000000) > 0){
                        specialCounter[i]++;
                    }
                }
            }
        }
        int best = 0;
        for (int i = 1; i < numOfCrypt; i++){
            if (specialCounter[i] > specialCounter[best]){
                best = i;
            }
        }
        int key;
        double prev = __DBL_MAX__;
        std::vector<int> columnsVec;
        if (specialCounter[best] > 0){
            for (int x = 0; x < numOfCrypt; x++){
                if (columns[k][x] == 1000)
                    continue;
                if (specialCounter[x] == specialCounter[best]){
                    for (int p = 32; p < 64; p++){
                     columnsVec.clear();
                        int columnsKey = columns[k][x] ^ p;
                        for (auto i = columns[k].begin(); i != columns[k].end(); i++){
                            if (*i != 1000)
                             columnsVec.push_back(*i ^ columnsKey);
                        }
                        double val = computeFittingQuotient(columnsVec);
                        if (val < prev){
                            prev = val;
                            key = columnsKey;
                        }
                    }
                }
            }
        }
        else {
            for (int x = 0; x < numOfCrypt; x++){
                if  (columns[k][x] == 1000)
                    continue;
                for (int p = 65; p < 122; p++){
                 columnsVec.clear();
                    int columnsKey = columns[k][0] ^ p;
                    for (auto i = columns[k].begin(); i != columns[k].end(); i++){
                        if (*i != 1000)
                         columnsVec.push_back(*i ^ columnsKey);
                    }
                    double val = computeFittingQuotient(columnsVec);
                    if (val < prev){
                        prev = val;
                        key = columnsKey;
                    }
                } 
            }
        }
        
        for (auto i = columns[k].begin(); i != columns[k].end(); i++)
            *i = (*i ^ key);
        if (k < result.size())
            result[k] = result[k] ^ key;
    }
    for (int i = 0; i < numOfCrypt; i++){
        for (int j = 0; j < lenOfCrypt; j++){
            std::cout << char (columns[j][i]);
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
    for (int i = 0; i < result.size(); i++){
        std::cout << char(result[i]);
    }
    std::cout << std::endl;
    delete[] columns;
    delete[] arr;
}