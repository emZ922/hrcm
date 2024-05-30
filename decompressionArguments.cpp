#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include <cctype>
#include <cstdlib>

using namespace std;

// Structures to hold information
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

// Function to read encoded data from a file
void readEncodedDataFromFile(const string &filename, vector<tuple<int, int, string>> &encodedData, vector<pair<int, char>> &encodedSpecialChars, vector<CharInfo> &nList, vector<CharInfo> &lowercaseList, string &referenceSequence) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cerr << "Error opening file for reading: " << filename << endl;
        exit(1);
    }

    int data;
    size_t size;

    // Read encoded data
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    encodedData.resize(size);
    int length;
    for (auto &item : encodedData) {
        inFile.read(reinterpret_cast<char *>(&get<0>(item)), sizeof(int));
        inFile.read(reinterpret_cast<char *>(&get<1>(item)), sizeof(int));
        inFile.read(reinterpret_cast<char *>(&length), sizeof(int));
        char *buffer = new char[length + 1]; // +1 for the null terminator

        inFile.read(buffer, length);
        buffer[length] = '\0';
        std::string str(buffer);

        delete[] buffer;
        get<2>(item) = str;
    }

    // Read special characters
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    encodedSpecialChars.resize(size);
    for (auto &special : encodedSpecialChars) {
        inFile.read(reinterpret_cast<char *>(&special.first), sizeof(int));
        inFile.read(reinterpret_cast<char *>(&special.second), sizeof(char));
    }

    // Read 'N' character information
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    nList.resize(size);
    inFile.read(reinterpret_cast<char *>(nList.data()), size * sizeof(CharInfo));

    // Read lowercase character information
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    lowercaseList.resize(size);
    inFile.read(reinterpret_cast<char *>(lowercaseList.data()), size * sizeof(CharInfo));

    // Read reference sequence
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    referenceSequence.resize(size);
    inFile.read(&referenceSequence[0], size);

    inFile.close();
}

// Function to decode sequence information from encoded data
void decodeSequenceInformation(const vector<tuple<int, int, string>> &encodedData, vector<Entity> &matchedEntities) {
    int previousPosition = 0;
    for (size_t i = 0; i < encodedData.size(); i++) {
        Entity entity;
        entity.position = previousPosition + get<0>(encodedData[i]);
        previousPosition = entity.position;

        entity.length = get<1>(encodedData[i]);
        entity.misMatched = get<2>(encodedData[i]); // Placeholder if mismatched info is used

        matchedEntities.push_back(entity);
    }
}

// Function to decompress data
void decompressData(const string &encodedFilename, const char *ref_filename, string &decompressedSequence, int mt) {
    vector<tuple<int, int, string>> encodedData;
    vector<pair<int, char>> encodedSpecialChars;
    vector<CharInfo> nList;
    vector<CharInfo> lowercaseList;
    string referenceSequence;
    readEncodedDataFromFile(encodedFilename, encodedData, encodedSpecialChars, nList, lowercaseList, referenceSequence);
    cout << "encdata" << referenceSequence << endl;

    // Decode sequence information
    vector<Entity> matchedEntities;
    decodeSequenceInformation(encodedData, matchedEntities);

    // Determine the length of the decompressed sequence
    int maxLength = 0;
    string tempSeq;
    tempSeq.resize(mt, '?');
    int t_len = 50;
    for (const auto &entity : matchedEntities) {
        cout << entity.position << " " << entity.length << " " << entity.misMatched << endl;
        maxLength = max(maxLength, entity.position + entity.length);
    }
    decompressedSequence.resize(mt, '?');

    cout << decompressedSequence.length() << endl;
    // Reconstruct the base sequence using the matched entities
    int start = 0;
    int read = 0;
    for (const auto &entity : matchedEntities) {
        cout << "start" << start << endl;
        for (int i = 0; i < entity.length; i++) {
            tempSeq[start + i] = referenceSequence[read++];
        }
        start = start + entity.length;
        // Handle mismatched portion if necessary
        for (int j = 0; j < entity.misMatched.length(); j++) {
            tempSeq[start + j] = entity.misMatched[j];
        }
        read = read + entity.misMatched.length();
        start = start + entity.misMatched.length();
    }
    cout << "f:" << tempSeq << endl;
    // Add 'N' characters
    start = 0;
    read = 0;
    char *str = new char[mt * sizeof(char)];
    for (const auto &nChar : nList) {
        // cout << nChar.position << "N" << nChar.length << endl;
        for (int i = 0; i < nChar.position; i++) {
            str[start + i] = tempSeq[read++];
        }
        start = start + nChar.position;
        for (int i = 0; i < nChar.length; i++) {
            str[start + i] = 'N';
        }
        start = start + nChar.length;
    }
    // cout << str << tempSeq.length() <<read  <<endl;
    for (int i = read; i < tempSeq.length(); i++) {
        str[start++] = tempSeq[i];
        // cout << i <<endl;
    }
    str[start] = '\0';
    int str_len = start;
    cout << "N:" << str << endl;
    // Reconstruct special characters
    int pos = 0;
    start = 0;
    read = 0;
    for (const auto &special : encodedSpecialChars) {
        // cout << decompressedSequence<<endl;
        for (int i = 0; i < special.first; i++) {
            // cout << str[read] <<endl;
            decompressedSequence[start + i] = str[read++];
        }
        decompressedSequence[start + special.first] = special.second;
        start = start + special.first + 1;
        // cout << special.first << special.second << endl;
    }

    for (int i = read; i < str_len; i++) {
        decompressedSequence[start++] = str[i];
        // cout << i <<endl;
    }
    cout << "s:" << decompressedSequence << endl;
    // Reconstruct lowercase characters
    start = 0;
    for (const auto &lowercase : lowercaseList) {
        // cout << lowercase.length << "L" << lowercase.position << endl;
        start = start + lowercase.position;
        for (int i = 0; i < lowercase.length; i++) { 
            decompressedSequence[start + i] = tolower(decompressedSequence[start + i]);
        }
        start = start + lowercase.length;
    }
    cout << "d:" << decompressedSequence << endl;
    cout << "o:" << "AGCTGGGCCCTTaaggtttnnnXXXTTTCCCGGGNNNaaaTTTccctttg" << endl;
}

// Function to write decompressed sequence to a .fa file
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

void decompressSingleFile(const char *ref_filename, const char *tar_filename, int mt) {
    string decompressedSequence;
    decompressData(tar_filename, ref_filename, decompressedSequence, mt);
    string output_filename = string(tar_filename).substr(0, string(tar_filename).find_last_of('.')) + ".fasta";
    writeDecompressedSequenceToFile(output_filename, decompressedSequence);
}

void decompressMultipleFiles(const char *ref_filename, const char *file_list, int mt) {
    ifstream file(file_list);
    if (!file) {
        cerr << "Error opening file list: " << file_list << endl;
        exit(1);
    }

    vector<string> t_filenames;
    string line;
    while (getline(file, line)) {
        t_filenames.push_back(line);
    }
    file.close();

    for (const auto &t_filename : t_filenames) {
        decompressSingleFile(ref_filename, t_filename.c_str(), mt);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cerr << "Usage: hrcm decompress -r ref-file-path [-t tar-file-path | -f filename]" << endl;
        return 1;
    }

    string mode = argv[1];
    const char *ref_filename = argv[3];
    int mt = 50;

    if (mode == "decompress") {
        if (string(argv[4]) == "-t") {
            const char *tar_filename = argv[5];
            decompressSingleFile(ref_filename, tar_filename, mt);
        } else if (string(argv[4]) == "-f") {
            const char *file_list = argv[5];
            decompressMultipleFiles(ref_filename, file_list, mt);
        } else {
            cerr << "Invalid option: " << argv[4] << endl;
            return 1;
        }
    } else {
        cerr << "Invalid mode: " << mode << endl;
        return 1;
    }

    return 0;
}
