#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdlib>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <cstring>

using namespace std;

// Structures to hold information
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

// Function to remove newline characters
void removeNewline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// Extract reference file information
void extractReferenceFileInfo(const char *&filename, string &r_seq, vector<LowercaseChar> &lowercaseList) {
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error: fail to open file " << filename << endl;
        exit(1);
    }

    string line;
    getline(inFile, line); // Read and ignore the first line
    cout << "Read first line and ignore it: " << line << endl;

    string lines;
    while (getline(inFile, line)) {
        lines += line;
    }
    inFile.close();

    int l = 0;
    int pos = 0;
    bool counting = false;
    int start = 0;
    for (int i = 0; i < lines.length(); i++) {
        if (islower(lines[i])) {
            lines[i] = toupper(lines[i]);
            if (counting) {
                l++;
            } else {
                counting = true;
                l = 1;
                pos = i - start;
            }
        } else {
            if (counting) {
                start = i;
                counting = false;
                LowercaseChar obj = {pos, l};
                lowercaseList.push_back(obj);
            }
        }
        if (lines[i] == 'A' || lines[i] == 'T' || lines[i] == 'C' || lines[i] == 'G') {
            r_seq += lines[i];
        }
        if (i == lines.length() - 1 && counting) {
            LowercaseChar obj = {pos, l};
            lowercaseList.push_back(obj);
        }
    }
    cout << "r_seq:" << r_seq << endl;
    for (const auto &item : lowercaseList) {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
}

// Extract target file information
void extractTargetFileInfo(const char *&filename, string &t_seq, vector<CharInfo> &lowercaseList, vector<CharInfo> &nList, vector<SpecialChar> &specialList) {
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error: fail to open file " << filename << endl;
        exit(1);
    }

    string id;
    getline(inFile, id); // Read the first line
    cout << "id:" << id << endl;

    string lines;
    string line;
    while (getline(inFile, line)) {
        lines += line;
    }
    inFile.close();

    cout << "input:" << lines << endl;
    int lowerLen = 0;
    int nLen = 0;
    int lowerPos = 0;
    int nPos = 0;
    int specialPos = 0;
    bool lowerFlag = false;
    bool specialFlag = false;
    bool nFlag = false;
    int lowerStart = 0;
    int specialStart = 0;
    int nStart = 0;
    bool isUpper = false;
    for (int i = 0; i < lines.length(); i++) {
        isUpper = true;
        if (islower(lines[i])) {
            isUpper = false;
            lines[i] = toupper(lines[i]);
            if (lowerFlag) {
                lowerLen++;
            } else {
                lowerFlag = true;
                lowerLen = 1;
                lowerPos = i - lowerStart;
            }
        }

        if (lines[i] == 'A' || lines[i] == 'T' || lines[i] == 'C' || lines[i] == 'G') {
            t_seq += lines[i];
        } else {
            if (lines[i] == 'N') {
                if (!nFlag) {
                    nFlag = true;
                    nLen = 1;
                    nPos = i - nStart;
                } else {
                    nLen++;
                }
            } else {
                SpecialChar obj = {i - 1 - specialStart, lines[i]};
                specialList.push_back(obj);
                specialStart = i;
            }
        }
        if ((isUpper || i == lines.length() - 1) && lowerFlag) {
            lowerStart = i;
            lowerFlag = false;
            CharInfo obj = {lowerPos, lowerLen};
            lowercaseList.push_back(obj);
        }
        if ((lines[i] != 'N' || i == lines.length() - 1) && nFlag) {
            nStart = i;
            nFlag = false;
            CharInfo obj = {nPos, nLen};
            nList.push_back(obj);
        }
    }
    cout << "t_seq:" << t_seq << endl;
    cout << "Lowercase:" << endl;
    for (const auto &item : lowercaseList) {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
    cout << "N character:" << endl;
    for (const auto &item : nList) {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
    cout << "Special:" << endl;
    for (const auto &item : specialList) {
        cout << "Position: " << item.position << ", Character: " << item.c << endl;
    }
}

// Hash function for k-mers
int hashFunction(const string &kMer) {
    int hashValue = kMer[0] - '0';
    for (int i = 1; i < kMer.length(); i++) {
        hashValue = hashValue * 4 + kMer[i] - '0';
    }
    return hashValue;
}

// Replace DNA characters with integers
string replaceDNAChars(const string &sequence) {
    string modifiedSequence = sequence;

    for (char &c : modifiedSequence) {
        switch (c) {
        case 'A':
            c = '0';
            break;
        case 'C':
            c = '1';
            break;
        case 'G':
            c = '2';
            break;
        case 'T':
            c = '3';
            break;
        }
    }

    return modifiedSequence;
}

// First level matching function
void firstLevelMatching(const string &r_seq, int k, const string &t_seq, int n_t, vector<Entity> &matchedEntities) {
    // Create a map to store the mapping from int to char
    std::map<char, char> intToCharMap;
    intToCharMap['0'] = 'A';
    intToCharMap['1'] = 'C';
    intToCharMap['2'] = 'G';
    intToCharMap['3'] = 'T';

    int l_max = k;
    int pos_max = 0;

    unordered_map<int, int> H;
    vector<int> L(r_seq.length(), -1); // Initialize array L with -1
    int hashValue = 0;
    string kMer = r_seq.substr(0, k);
    hashValue = hashFunction(kMer);
    L[0] = H.count(0) ? H[0] : -1;
    H[hashValue] = 0;

    // Create hash table for reference B sequence
    for (int i = 1; i <= r_seq.length() - k; i++) {
        hashValue = hashFunction(r_seq.substr(i, k));
        L[i] = H.count(hashValue) ? H[hashValue] : -1;
        H[hashValue] = i;
    }

    string misStr = "";
    for (int i = 0; i <= n_t - k; i++) {
        hashValue = 0;
        kMer = t_seq.substr(i, k);
        hashValue = hashFunction(kMer);
        int pos = H.count(hashValue) ? H[hashValue] : -1;

        if (pos > -1) {
            l_max = -1;
            pos_max = -1;
            int j = pos;
            int r_id = j + k;
            int t_id = i + k;
            while (j != -1) {
                int l = k;
                int p = pos;
                r_id = j + k;
                t_id = i + k;

                while (t_id < n_t && r_id < r_seq.length() && t_seq[t_id++] == r_seq[r_id++]) {
                    l++;
                }

                if (l_max < l) {
                    l_max = l;
                    pos_max = j;
                }

                j = L[j];
            }
            Entity entity = {i, l_max, ""};
            t_id--; r_id--;
            while (t_seq[t_id] != r_seq[r_id]) {
                misStr += intToCharMap[t_seq[t_id]];
                t_id++; r_id++;
            }
            entity.misMatched = misStr;
            matchedEntities.push_back(entity);
            misStr = "";
            i = t_id - 1;
        }
    }
}

// Second level matching function
void secondLevelMatching(const vector<Entity> &ref_entities, const vector<Entity> &to_be_compressed_entities, vector<Entity> &matchedEntities, vector<Entity> &mismatchedEntities) {
    int posmax = 0;
    int lenmax = 0;
    unordered_map<int, vector<int>> H; 

    for (int i = 0; i < ref_entities.size(); ++i) {
        const Entity &entity = ref_entities[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);
        H[hashValue].push_back(i);
    }

    for (int i = 0; i < to_be_compressed_entities.size(); ++i) {
        const Entity &entity = to_be_compressed_entities[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);

        lenmax = 0;
        posmax = -1;

        if (H.count(hashValue)) {
            for (int ref_pos : H[hashValue]) {
                int len = 0;
                while (i + len < to_be_compressed_entities.size() && ref_pos + len < ref_entities.size() &&
                       to_be_compressed_entities[i + len].position == ref_entities[ref_pos + len].position &&
                       to_be_compressed_entities[i + len].length == ref_entities[ref_pos + len].length) {
                    len++;
                }

                if (lenmax < len) {
                    lenmax = len;
                    posmax = ref_pos;
                }
            }
        }

        if (lenmax < 2) {
            mismatchedEntities.push_back(entity);
        } else {
            Entity matchedEntity = {entity.position, lenmax, ""};
            matchedEntities.push_back(matchedEntity);
            i += lenmax - 1;
        }
    }

    cout << "Output: matched entities (position, length, mismatched) after second level matching" << endl;
    for (const auto &entity : matchedEntities) {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }

    cout << "Output: mismatched entities after second level matching" << endl;
    for (const auto &entity : mismatchedEntities) {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }
}

void writeDataToFile(const string &filename, const vector<Entity> &matchedEntities, const vector<Entity> &mismatchedEntities, const vector<CharInfo> &nList, const vector<LowercaseChar> &lowercaseList, const vector<SpecialChar> &specialList, const string &referenceSequence) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error opening file for writing: " << filename << endl;
        exit(1);
    }

    outFile << "Matched Entities:" << endl;
    for (const auto &entity : matchedEntities) {
        outFile << "(" << entity.position << ", " << entity.length << ")" << endl;
    }

    outFile << "Mismatched Entities:" << endl;
    for (const auto &entity : mismatchedEntities) {
        outFile << "(" << entity.position << ", " << entity.length << ", " << entity.misMatched << ")" << endl;
    }

    outFile << "N List:" << endl;
    for (const auto &info : nList) {
        outFile << "Position: " << info.position << ", Length: " << info.length << endl;
    }

    outFile << "Lowercase List:" << endl;
    for (const auto &info : lowercaseList) {
        outFile << "Position: " << info.position << ", Length: " << info.length << endl;
    }

    outFile << "Special Characters:" << endl;
    for (const auto &special : specialList) {
        outFile << "Position: " << special.position << ", Character: " << special.c << endl;
    }

    outFile << "Reference Sequence:" << endl;
    outFile << referenceSequence << endl;

    outFile.close();
    cout << "Data written to " << filename << endl;
}

int main() {
    const char *ref_filename = "ref_seq.fa";
    const char *t_filename = "t_seq.fa";
    string r_seq;
    vector<LowercaseChar> r_lowercaseList;
    string t_seq;
    vector<CharInfo> t_lowercaseList;
    vector<CharInfo> t_nList;
    vector<SpecialChar> t_specialList;

    extractReferenceFileInfo(ref_filename, r_seq, r_lowercaseList);
    extractTargetFileInfo(t_filename, t_seq, t_lowercaseList, t_nList, t_specialList);

    vector<Entity> ref_entities;
    vector<Entity> to_be_compressed_entities;

    string r_seq_int = replaceDNAChars(r_seq);
    string t_seq_int = replaceDNAChars(t_seq);

    firstLevelMatching(r_seq_int, 3, r_seq_int, r_seq_int.length(), ref_entities);
    firstLevelMatching(r_seq_int, 3, t_seq_int, t_seq_int.length(), to_be_compressed_entities);

    vector<Entity> matchedEntities;
    vector<Entity> mismatchedEntities;
    secondLevelMatching(ref_entities, to_be_compressed_entities, matchedEntities, mismatchedEntities);
    writeDataToFile("compressed_data2.txt", matchedEntities, mismatchedEntities, t_nList, r_lowercaseList, t_specialList, r_seq);

    return 0;
}
