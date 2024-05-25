#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cctype> // For tolower

using namespace std;


struct LowercaseChar {
    int position;
    int length;
};

struct CharInfo {
    int position;
    int length;
};

struct SpecialChar {
    int position;
    char c;
};

struct Entity {
    int position;
    int length;
    string misMatched;
};

// Citanje podataka iz kompresirane datoteke
vector<int> readEncodedDataFromFile(const string &filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cerr << "Error opening file for reading: " << filename << endl;
        exit(1);
    }

    vector<int> encodedData;
    int data;
    while (inFile.read(reinterpret_cast<char*>(&data), sizeof(int))) {
        encodedData.push_back(data);
    }

    inFile.close();
    return encodedData;
}

// Funkcija za dekodiranje sekvence u kompresiranu sekvencu
void decodeSequenceInformation(const vector<int> &encodedData, vector<Entity> &matchedEntities) {
    int previousPosition = 0;
    for (size_t i = 0; i < encodedData.size(); i += 2) {
        Entity entity;
        entity.position = previousPosition + encodedData[i];
        previousPosition = entity.position;

        entity.length = encodedData[i + 1];
        entity.misMatched = ""; 

        matchedEntities.push_back(entity);
    }
}

// Funkcija za dekompresiranje podataka
void decompressData(const string &encodedFilename, string &decompressedSequence, const vector<LowercaseChar> &r_lowercaseList, const vector<SpecialChar> &specialList, const vector<CharInfo> &nList) {
    vector<int> encodedData = readEncodedDataFromFile(encodedFilename);

    vector<Entity> matchedEntities;
    decodeSequenceInformation(encodedData, matchedEntities);

    int maxLength = 0;
    for (const auto &entity : matchedEntities) {
        maxLength = max(maxLength, entity.position + entity.length);
    }
    decompressedSequence.resize(maxLength, 'N');

    for (const auto &entity : matchedEntities) {
        for (int i = 0; i < entity.length; ++i) {
            decompressedSequence[entity.position + i] = 'A'; 
        }
    }

    for (const auto &lowercase : r_lowercaseList) {
        for (int i = 0; i < lowercase.length; ++i) {
            decompressedSequence[lowercase.position + i] = tolower(decompressedSequence[lowercase.position + i]);
        }
    }

    for (const auto &nChar : nList) {
        for (int i = 0; i < nChar.length; ++i) {
            decompressedSequence[nChar.position + i] = 'N';
        }
    }

    for (const auto &special : specialList) {
        decompressedSequence[special.position] = special.c;
    }
}

// Zapisati dekompresiranu sekvencu u datoteku
void writeDecompressedSequenceToFile(const string &filename, const string &decompressedSequence) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error opening file for writing: " << filename << endl;
        exit(1);
    }

    outFile << decompressedSequence << endl;

    outFile.close();
    cout << "Decompressed sequence written to " << filename << endl;
}

int main() {
    const char *encodedFilename = "compressed_data.bin";
    const char *outputFilename = "decompressed_sequence.fa";
    
    vector<LowercaseChar> r_lowercaseList = { {12, 4}, {18, 3}, {21, 3}, {24, 7} }; // Example data
    vector<CharInfo> nList = { {19, 3}, {32, 3} }; // Example data
    vector<SpecialChar> specialList = { {22, 'X'}, {23, 'X'}, {24, 'X'} }; // Example data

    string decompressedSequence;
    decompressData(encodedFilename, decompressedSequence, r_lowercaseList, specialList, nList);
    writeDecompressedSequenceToFile(outputFilename, decompressedSequence);

    cout << "Decompressed Sequence: " << decompressedSequence << endl;

    return 0;
}
