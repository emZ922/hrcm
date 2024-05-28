#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdlib>
#include <algorithm>

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

// Function to read the data from a text file
void readDataFromFile(const string &filename, vector<Entity> &matchedEntities, vector<Entity> &mismatchedEntities, vector<CharInfo> &nList, vector<LowercaseChar> &lowercaseList, vector<SpecialChar> &specialList, string &referenceSequence) {
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error opening file for reading: " << filename << endl;
        exit(1);
    }

    string line;

    // Read matched entities
    getline(inFile, line); // Skip the "Matched Entities:" line
    while (getline(inFile, line) && line != "Mismatched Entities:") {
        if (line.empty()) continue;
        Entity entity;
        size_t pos1 = line.find("Position: ") + 10;
        size_t pos2 = line.find(", Length: ");
        entity.position = stoi(line.substr(pos1, pos2 - pos1));
        pos1 = line.find("Length: ") + 8;
        pos2 = line.find(", Mismatched: ");
        entity.length = stoi(line.substr(pos1, pos2 - pos1));
        pos1 = line.find("Mismatched: ") + 12;
        entity.misMatched = line.substr(pos1);
        matchedEntities.push_back(entity);
    }

    // Read mismatched entities
    while (getline(inFile, line) && line != "N List:") {
        if (line.empty()) continue;
        Entity entity;
        size_t pos1 = line.find("Position: ") + 10;
        size_t pos2 = line.find(", Length: ");
        entity.position = stoi(line.substr(pos1, pos2 - pos1));
        pos1 = line.find("Length: ") + 8;
        pos2 = line.find(", Mismatched: ");
        entity.length = stoi(line.substr(pos1, pos2 - pos1));
        pos1 = line.find("Mismatched: ") + 12;
        entity.misMatched = line.substr(pos1);
        mismatchedEntities.push_back(entity);
    }

    // Read 'N' characters
    while (getline(inFile, line) && line != "Lowercase List:") {
        if (line.empty()) continue;
        CharInfo info;
        size_t pos1 = line.find("Position: ") + 10;
        size_t pos2 = line.find(", Length: ");
        info.position = stoi(line.substr(pos1, pos2 - pos1));
        pos1 = line.find("Length: ") + 8;
        info.length = stoi(line.substr(pos1));
        nList.push_back(info);
    }

    // Read lowercase characters
    while (getline(inFile, line) && line != "Special Characters:") {
        if (line.empty()) continue;
        LowercaseChar info;
        size_t pos1 = line.find("Position: ") + 10;
        size_t pos2 = line.find(", Length: ");
        info.position = stoi(line.substr(pos1, pos2 - pos1));
        pos1 = line.find("Length: ") + 8;
        info.length = stoi(line.substr(pos1));
        lowercaseList.push_back(info);
    }

    // Read special characters
    while (getline(inFile, line) && line != "Reference Sequence:") {
        if (line.empty()) continue;
        SpecialChar special;
        size_t pos1 = line.find("Position: ") + 10;
        size_t pos2 = line.find(", Character: ");
        special.position = stoi(line.substr(pos1, pos2 - pos1));
        pos1 = line.find("Character: ") + 11;
        special.c = line[pos1];
        specialList.push_back(special);
    }

    // Read reference sequence
    getline(inFile, referenceSequence);

    inFile.close();
}

void decompressData(const string &inputFilename, string &decompressedSequence) {
    vector<Entity> matchedEntities;
    vector<Entity> mismatchedEntities;
    vector<CharInfo> nList;
    vector<LowercaseChar> lowercaseList;
    vector<SpecialChar> specialList;
    string referenceSequence;
    readDataFromFile(inputFilename, matchedEntities, mismatchedEntities, nList, lowercaseList, specialList, referenceSequence);

    // Determine the length of the decompressed sequence
    int maxLength = 0;
    for (const auto &entity : matchedEntities) {
        maxLength = max(maxLength, entity.position + entity.length);
    }
    decompressedSequence.resize(maxLength, 'N');

    // Reconstruct the sequence using the matched entities
    for (const auto &entity : matchedEntities) {
        for (int i = 0; i < entity.length; ++i) {
            decompressedSequence[entity.position + i] = referenceSequence[entity.position + i];
        }
        // Handle mismatched portion if necessary
        for (size_t j = 0; j < entity.misMatched.length(); ++j) {
            decompressedSequence[entity.position + entity.length + j] = entity.misMatched[j];
        }
    }

    // Reconstruct lowercase characters
    for (const auto &lowercase : lowercaseList) {
        for (int i = 0; i < lowercase.length; ++i) {
            decompressedSequence[lowercase.position + i] = tolower(decompressedSequence[lowercase.position + i]);
        }
    }

    // Reconstruct 'N' characters
    for (const auto &nChar : nList) {
        for (int i = 0; i < nChar.length; ++i) {
            decompressedSequence[nChar.position + i] = 'N';
        }
    }

    // Reconstruct special characters
    for (const auto &special : specialList) {
        decompressedSequence[special.position] = special.c;
    }
}

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
    string decompressedSequence;
    decompressData("compressed_data2.txt", decompressedSequence);
    writeDecompressedSequenceToFile("decompressed_sequence2.fa", decompressedSequence);

    // Output decompressed sequence for verification
    cout << "Decompressed Sequence: " << decompressedSequence << endl;

    return 0;
}
