#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cctype>  // For islower and toupper functions
#include <cstdlib> // For exit()
#include <unordered_map>
#include <map>
#include <algorithm>
#include <cstring> // For strlen


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
void extractReferenceFileInfo(const char *&filename, int mr, string &r_seq, vector<LowercaseChar> &lowercaseList) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: fail to open file %s.\n", filename);
        exit(-1);
    }
    char line[1024];

    if (fgets(line, sizeof(line), fp) != NULL) {
        printf("Read first line and ignore it: %s", line); // Print the line
    } else {
        printf("Error: fail to read a line from file %s.\n", filename);
    }
    string lines = "";
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        lines += line;
    }

    fclose(fp);
    lines += '\0';
    cout << "input:" << lines << endl;
    int l = 0;
    int pos = 0;
    bool counting = false;
    int r_len = 0;
    int start = 0;
    for (int i = 0; i < mr; i++) {
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
                LowercaseChar obj;
                obj.position = pos;
                obj.length = l;
                lowercaseList.push_back(obj);
            }
        }
        if (lines[i] == 'A' || lines[i] == 'T' || lines[i] == 'C' || lines[i] == 'G') {
            r_seq += lines[i];
        }
        if (i == mr - 1 && counting) {
            LowercaseChar obj;
            obj.position = pos;
            obj.length = l;
            lowercaseList.push_back(obj);
        }
    }
    cout << lines.length() << endl;
    cout << "r_seq:" << r_seq << endl;
    for (const auto &item : lowercaseList) {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
}

// Extract target file information
void extractTargetFileInfo(const char *&filename, int mt, string &t_seq, vector<CharInfo> &lowercaseList, vector<CharInfo> &nList, vector<SpecialChar> &specialList) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: fail to open file %s.\n", filename);
        exit(-1);
    }
    char line[1024];
    string id = "";
    if (fgets(line, sizeof(line), fp) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        id += line;
    } else {
        printf("Error: fail to read a line from file %s.\n", filename);
    }
    string lines = "";
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        lines += line;
    }

    fclose(fp);

    cout << "id:" << id << endl;
    lines += '\0';
    cout << "input:" << lines << endl;
    int lowerLen = 0;
    int nLen = 0;
    int lowerPos = 0;
    int nPos = 0;
    int specialPos = 0;
    bool lowerFlag = false;
    bool specialFlag = false;
    bool nFlag = false;
    int t_len = 0;
    int lowerStart = 0;
    int specialStart = 0;
    int nStart = 0;
    bool isUpper = false;
    for (int i = 0; i < mt; i++) {
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
                SpecialChar obj;
                obj.position = i - 1 - specialStart;
                obj.c = lines[i];
                specialList.push_back(obj);
                specialStart = i;
            }
        }
        if ((isUpper || i == mt - 1) && lowerFlag) {
            lowerStart = i;
            lowerFlag = false;
            CharInfo obj;
            obj.position = lowerPos;
            obj.length = lowerLen;
            lowercaseList.push_back(obj);
        }
        if ((lines[i] != 'N' || i == mt - 1) && nFlag) {
            nStart = i;
            nFlag = false;
            CharInfo obj;
            obj.position = nPos;
            obj.length = nLen;
            nList.push_back(obj);
        }
    }
    cout << lines.length() << endl;
    cout << "t_seq:" << t_seq << endl;
    cout << "Lowercase:" << endl;
    for (const auto &item : lowercaseList) {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
    cout << endl;
    cout << "N character:" << endl;
    for (const auto &item : nList) {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
    cout << endl;
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

    // Replace characters with integers
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
            Entity entity;
            entity.position = i;
            entity.length = l_max;
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
void secondLevelMatching(const vector<Entity> &ref_entities, const vector<Entity> &to_be_compressed_entities, int k) {
    vector<Entity> matchedEntities;

    // Create hash table for reference matched entity sequences
    unordered_map<int, vector<int>> H; // hash table
    for (int i = 0; i < ref_entities.size(); ++i) {
        const Entity &entity = ref_entities[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);
        H[hashValue].push_back(i);
    }

    // Perform matching for the to-be-compressed entities
    for (int i = 0; i < to_be_compressed_entities.size(); ++i) {
        const Entity &entity = to_be_compressed_entities[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);

        int len_max = 0;
        int pos_max = -1;

        // Check hash table for matches
        if (H.count(hashValue)) {
            for (int ref_pos : H[hashValue]) {
                int len = k;
                while (i + len < to_be_compressed_entities.size() && ref_pos + len < ref_entities.size() &&
                       to_be_compressed_entities[i + len].position == ref_entities[ref_pos + len].position &&
                       to_be_compressed_entities[i + len].length == ref_entities[ref_pos + len].length) {
                    ++len;
                }

                if (len > len_max) {
                    len_max = len;
                    pos_max = ref_pos;
                }
            }
        }

        Entity newEntity;
        newEntity.position = i;
        newEntity.length = len_max;

        if (len_max >= 2) {
            newEntity.misMatched = ""; 
            matchedEntities.push_back(newEntity);
            i += len_max - 1;
        } else {
            newEntity.misMatched = entity.misMatched;
            matchedEntities.push_back(newEntity);
        }
    }

    cout << "Output: matched entities (position, length, mismatched) after second level matching" << endl;
    for (const auto &entity : matchedEntities) {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }
}

void encodeSequenceInformation(const vector<Entity> &matchedEntities, vector<int> &encodedData) {
    int previousPosition = 0;

    for (const auto &entity : matchedEntities) {
        encodedData.push_back(entity.position - previousPosition); // Delta encoding for position
        previousPosition = entity.position;

        encodedData.push_back(entity.length);
        // Optionally encode mismatched information if necessary
    }
}

void encodeLowercaseInformation(const vector<Entity> &matchedEntities, vector<int> &encodedData) {
    int previousPosition = 0;

    for (const auto &entity : matchedEntities) {
        encodedData.push_back(entity.position - previousPosition); // Delta encoding for position
        previousPosition = entity.position;

        encodedData.push_back(entity.length);
        // Optionally encode mismatched information if necessary
    }
}

void lowercaseMatching(const vector<LowercaseChar> &ref_lowercaseList, const vector<CharInfo> &t_lowercaseList, vector<Entity> &matchedEntities) {
    // Create a hash table for reference lowercase segments
    unordered_map<int, vector<int>> H;
    for (int i = 0; i < ref_lowercaseList.size(); ++i) {
        const LowercaseChar &entity = ref_lowercaseList[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);
        H[hashValue].push_back(i);
    }

    // Match lowercase segments from the to-be-compressed sequence
    for (int i = 0; i < t_lowercaseList.size(); ++i) {
        const CharInfo &entity = t_lowercaseList[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);

        int len_max = 0;
        int pos_max = -1;

        if (H.count(hashValue)) {
            for (int ref_pos : H[hashValue]) {
                int len = 1;
                while (i + len < t_lowercaseList.size() && ref_pos + len < ref_lowercaseList.size() &&
                       t_lowercaseList[i + len].position == ref_lowercaseList[ref_pos + len].position &&
                       t_lowercaseList[i + len].length == ref_lowercaseList[ref_pos + len].length) {
                    ++len;
                }

                if (len > len_max) {
                    len_max = len;
                    pos_max = ref_pos;
                }
            }
        }

        Entity newEntity;
        newEntity.position = entity.position;
        newEntity.length = len_max;

        if (len_max >= 1) {
            newEntity.misMatched = "";
            matchedEntities.push_back(newEntity);
            i += len_max - 1;
        } else {
            newEntity.misMatched = to_string(entity.position) + "-" + to_string(entity.length);
            matchedEntities.push_back(newEntity);
        }
    }

    cout << "Output: matched lowercase entities (position, length, mismatched)" << endl;
    for (const auto &entity : matchedEntities) {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }
}

int main() {
    const char *ref_filename = "ref_seq.fa";
    const char *t_filename = "t_seq.fa";
    int mr = 50;
    int mt = 50;
    string r_seq;
    vector<LowercaseChar> r_lowercaseList;
    string t_seq;
    vector<CharInfo> t_lowercaseList;
    vector<CharInfo> t_nList;
    vector<SpecialChar> t_specialList;

    // Extract sequence information
    extractReferenceFileInfo(ref_filename, mr, r_seq, r_lowercaseList);
    extractTargetFileInfo(t_filename, mt, t_seq, t_lowercaseList, t_nList, t_specialList);

    // Convert DNA chars to integers for matching
    string r_seq_int = replaceDNAChars(r_seq);
    string t_seq_int = replaceDNAChars(t_seq);

    // First level matching
    vector<Entity> matchedEntities1;
    vector<Entity> matchedEntities2;

    firstLevelMatching(r_seq_int, 3, t_seq_int, t_seq_int.length(), matchedEntities1);
    firstLevelMatching(r_seq_int, 3, t_seq_int, t_seq_int.length(), matchedEntities2);

    cout << "First Level Matching for Sequence 1:" << endl;
    for (const auto &entity : matchedEntities1) {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }

    cout << "First Level Matching for Sequence 2:" << endl;
    for (const auto &entity : matchedEntities2) {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }

    // Second level matching
    secondLevelMatching(matchedEntities1, matchedEntities2, 3);

    // Lowercase character information matching
    vector<Entity> matchedLowercaseEntities;
    lowercaseMatching(r_lowercaseList, t_lowercaseList, matchedLowercaseEntities);

    // Sequence information encoding
    vector<int> encodedData;
    encodeSequenceInformation(matchedEntities1, encodedData);
    encodeLowercaseInformation(matchedLowercaseEntities, encodedData);

    // Output encoded data for verification
    cout << "Encoded Data:" << endl;
    for (const auto &data : encodedData) {
        cout << data << " ";
    }
    cout << endl;

    return 0;
}
