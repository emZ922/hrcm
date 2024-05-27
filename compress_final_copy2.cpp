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
#include <fstream> // For ofstream

using namespace std;

// Structures to hold information
struct LowercaseChar
{
    int position;
    int length;
};

struct CharInfo
{
    int position;
    int length;
};

struct SpecialChar
{
    int position;
    char c;
};

struct Entity
{
    int position;
    int length;
    string misMatched;
};

// Function to remove newline characters
void removeNewline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}

// Extract reference file information
void extractReferenceFileInfo(const char *&filename, int mr, string &r_seq, vector<LowercaseChar> &lowercaseList)
{
    cout << "Reference file info extraction." << endl;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: fail to open file %s.\n", filename);
        exit(-1);
    }
    char line[1024];

    if (fgets(line, sizeof(line), fp) != NULL)
    {
        printf("Read first line and ignore it: %s", line); // Print the line
    }
    else
    {
        printf("Error: fail to read a line from file %s.\n", filename);
    }
    string lines = "";
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0'; // remove newline
        }
        lines += line;
    }

    fclose(fp);
    lines += '\0'; // terminate string
    cout << "input:" << lines << endl;
    int l = 0;
    int pos = 0;
    bool counting = false; // note whether we are counting lowercase characters
    int r_len = 0;
    int start = 0;
    for (int i = 0; i < lines.length(); i++)
    {
        if (islower(lines[i]))
        {
            lines[i] = toupper(lines[i]);
            if (counting)
            {
                l++;
            }
            else
            {
                counting = true;
                l = 1;
                pos = i - start;
            }
        }
        else
        {
            if (counting)
            {
                start = i;
                counting = false;
                LowercaseChar obj;
                obj.position = pos;
                obj.length = l;
                lowercaseList.push_back(obj);
            }
        }
        if (lines[i] == 'A' || lines[i] == 'T' || lines[i] == 'C' || lines[i] == 'G')
        {
            r_seq += lines[i];
        }
        if (i == lines.length() - 1 && counting)
        {
            LowercaseChar obj;
            obj.position = pos;
            obj.length = l;
            lowercaseList.push_back(obj);
        }
    }
    cout << "r_seq:" << r_seq << endl;
    for (const auto &item : lowercaseList)
    {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
}

// Extract target file information
void extractTargetFileInfo(const char *&filename, int mt, string &t_seq, vector<CharInfo> &lowercaseList, vector<CharInfo> &nList, vector<SpecialChar> &specialList)
{
    cout << "Target file info extraction." << endl;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: fail to open file %s.\n", filename);
        exit(-1);
    }
    char line[1024];
    string id = "";
    if (fgets(line, sizeof(line), fp) != NULL)
    {
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0'; // remove newline
        }
        id += line;
    }
    else
    {
        printf("Error: fail to read a line from file %s.\n", filename);
    }
    string lines = "";
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }
        lines += line;
    }

    fclose(fp);

    cout << "id:" << id << endl;
    lines += '\0'; // terminate string
    cout << "input:" << lines << endl;

    int lowerLen = 0;
    int nLen = 0;
    int t_len = 0;

    // starting position for current sequence of certain type
    int lowerPos = 0;
    int nPos = 0;
    int specialPos = 0;

    // flags determine whether we are counting certain type of characters
    bool lowerFlag = false;
    bool specialFlag = false;
    bool nFlag = false;

    // starting position for last sequence of certain type
    int lowerStart = 0;
    int specialStart = 0;
    int nStart = 0;

    bool isUpper = false;
    for (int i = 0; i < lines.length() - 1; i++)
    {
        isUpper = true;
        if (islower(lines[i]))
        {
            isUpper = false;
            lines[i] = toupper(lines[i]);
            if (lowerFlag)
            {
                lowerLen++;
            }
            else
            {
                lowerFlag = true;
                lowerLen = 1;
                lowerPos = i - lowerStart;
            }
        }

        if (lines[i] == 'A' || lines[i] == 'T' || lines[i] == 'C' || lines[i] == 'G')
        {
            t_seq += lines[i];
        }
        else
        {
            if (lines[i] == 'N')
            {
                if (!nFlag)
                {
                    nFlag = true;
                    nLen = 1;
                    nPos = i - nStart;
                }
                else
                {
                    nLen++;
                }
            }
            else
            {
                SpecialChar obj;
                obj.position = i - 1 - specialStart;
                obj.c = lines[i];
                specialList.push_back(obj);
                specialStart = i;
            }
        }
        if ((isUpper || i == lines.length() - 1) && lowerFlag)
        {
            lowerStart = i;
            lowerFlag = false;
            CharInfo obj;
            obj.position = lowerPos;
            obj.length = lowerLen;
            lowercaseList.push_back(obj);
        }
        if ((lines[i] != 'N' || i == lines.length() - 1) && nFlag)
        {
            nStart = i;
            nFlag = false;
            CharInfo obj;
            obj.position = nPos;
            obj.length = nLen;
            nList.push_back(obj);
        }
    }
    cout << "t_seq:" << t_seq << endl;
    cout << "Lowercase:" << endl;
    for (const auto &item : lowercaseList)
    {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
    cout << endl;
    cout << "N character:" << endl;
    for (const auto &item : nList)
    {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
    cout << endl;
    cout << "Special:" << endl;
    for (const auto &item : specialList)
    {
        cout << "Position: " << item.position << ", Character: " << item.c << endl;
    }
}

// Hash function for k-mers
int hashFunction(const string &kMer)
{
    // Calculate quaternary value of k-mer
    int hashValue = kMer[0] - '0';
    for (int i = 1; i < kMer.length(); i++)
    {
        hashValue = hashValue * 4 + kMer[i] - '0';
    }
    return hashValue;
}

// Replace DNA characters with integers
string replaceDNAChars(const string &sequence)
{
    string modifiedSequence = sequence;

    // Replace characters with integers
    for (char &c : modifiedSequence)
    {
        switch (c)
        {
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
void firstLevelMatching(const string &r_seq, int k, const string &t_seq, int n_t, vector<Entity> &matchedEntities)
{
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
    L[0] = H.count(0) ? H[0] : -1; // Act as if all H values are -1
    H[hashValue] = 0;

    // Create hash table for reference B sequence
    for (int i = 1; i <= r_seq.length() - k; i++)
    {
        hashValue = hashFunction(r_seq.substr(i, k));
        L[i] = H.count(hashValue) ? H[hashValue] : -1;
        H[hashValue] = i;
    }

    string misStr = "";
    // Iterate over t_seq
    for (int i = 0; i <= n_t - k; i++)
    {
        hashValue = 0;
        kMer = t_seq.substr(i, k);
        hashValue = hashFunction(kMer);

        // Check if k-mer's hash value has appeared before
        int pos = H.count(hashValue) ? H[hashValue] : -1;
        if (pos > -1)
        {
            l_max = -1;
            pos_max = -1;
            int j = pos;
            int r_id = j + k;
            int t_id = i + k;
            while (j != -1)
            {
                int l = k;
                int p = pos;
                r_id = j + k;
                t_id = i + k;

                while (t_id < n_t && r_id < r_seq.length() && t_id < t_seq.length() && t_seq[t_id++] == r_seq[r_id++])
                {
                    l++;
                }

                if (l_max < l)
                {
                    l_max = l;
                    pos_max = j;
                }

                j = L[j]; // Check if there is another occurance of same hash value
            }
            Entity entity;
            entity.position = i;
            entity.length = l_max;

            // Find mismatched string
            t_id = i + l_max;
            r_id = pos_max + l_max;
            while (t_seq[t_id] != r_seq[r_id] && r_id < r_seq.length() && t_id < t_seq.length())
            {
                misStr += intToCharMap[t_seq[t_id]];
                t_id++;
                r_id++;
            }
            entity.misMatched = misStr;
            matchedEntities.push_back(entity);
            misStr = "";
            i = t_id - 1;
        }
    }
}

// Second level matching function
void secondLevelMatching(const vector<Entity> &ref_entities, const vector<Entity> &to_be_compressed_entities, int k)
{
    vector<Entity> matchedEntities;

    // Create hash table for reference matched entity sequences
    unordered_map<int, vector<int>> H; // hash table
    for (int i = 0; i < ref_entities.size(); ++i)
    {
        const Entity &entity = ref_entities[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);
        H[hashValue].push_back(i);
    }

    // Perform matching for the to-be-compressed entities
    for (int i = 0; i < to_be_compressed_entities.size(); ++i)
    {
        const Entity &entity = to_be_compressed_entities[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);

        int len_max = 0;
        int pos_max = -1;

        // Check hash table for matches
        if (H.count(hashValue))
        {
            for (int ref_pos : H[hashValue])
            {
                int len = k;
                while (i + len < to_be_compressed_entities.size() && ref_pos + len < ref_entities.size() &&
                       to_be_compressed_entities[i + len].position == ref_entities[ref_pos + len].position &&
                       to_be_compressed_entities[i + len].length == ref_entities[ref_pos + len].length)
                {
                    ++len;
                }

                if (len > len_max)
                {
                    len_max = len;
                    pos_max = ref_pos;
                }
            }
        }

        Entity newEntity;
        newEntity.position = i;
        newEntity.length = len_max;

        if (len_max >= 2)
        {
            newEntity.misMatched = "";
            matchedEntities.push_back(newEntity);
            i += len_max - 1;
        }
        else
        {
            newEntity.misMatched = entity.misMatched;
            matchedEntities.push_back(newEntity);
        }
    }

    cout << "Output: matched entities (position, length, mismatched) after second level matching" << endl;
    for (const auto &entity : matchedEntities)
    {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }
}

void encodeSequenceInformation(const vector<Entity> &matchedEntities, vector<int> &encodedData)
{
    int previousPosition = 0;

    for (const auto &entity : matchedEntities)
    {
        encodedData.push_back(entity.position - previousPosition); // Delta encoding for position
        previousPosition = entity.position;

        encodedData.push_back(entity.length);
    }
}

void encodeLowercaseInformation(const vector<Entity> &matchedEntities, vector<int> &encodedData)
{
    int previousPosition = 0;

    for (const auto &entity : matchedEntities)
    {
        encodedData.push_back(entity.position - previousPosition); // Delta encoding for position
        previousPosition = entity.position;

        encodedData.push_back(entity.length);
    }
}

void encodeAdditionalInformation(const vector<CharInfo> &charInfoList, vector<int> &encodedData)
{
    for (const auto &info : charInfoList)
    {
        encodedData.push_back(info.position);
        encodedData.push_back(info.length);
    }
}

void encodeSpecialCharacters(const vector<SpecialChar> &specialList, vector<pair<int, char>> &encodedSpecialChars)
{
    for (const auto &special : specialList)
    {
        encodedSpecialChars.push_back(make_pair(special.position, special.c));
    }
}

void lowercaseMatching(const vector<LowercaseChar> &ref_lowercaseList, const vector<CharInfo> &t_lowercaseList, vector<Entity> &matchedEntities)
{
    // Create a hash table for reference lowercase segments
    unordered_map<int, vector<int>> H;
    for (int i = 0; i < ref_lowercaseList.size(); ++i)
    {
        const LowercaseChar &entity = ref_lowercaseList[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);
        H[hashValue].push_back(i);
    }

    // Match lowercase segments from the to-be-compressed sequence
    for (int i = 0; i < t_lowercaseList.size(); ++i)
    {
        const CharInfo &entity = t_lowercaseList[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);

        int len_max = 0;
        int pos_max = -1;

        if (H.count(hashValue))
        {
            for (int ref_pos : H[hashValue])
            {
                int len = 1;
                while (i + len < t_lowercaseList.size() && ref_pos + len < ref_lowercaseList.size() &&
                       t_lowercaseList[i + len].position == ref_lowercaseList[ref_pos + len].position &&
                       t_lowercaseList[i + len].length == ref_lowercaseList[ref_pos + len].length)
                {
                    ++len;
                }

                if (len > len_max)
                {
                    len_max = len;
                    pos_max = ref_pos;
                }
            }
        }

        Entity newEntity;
        newEntity.position = entity.position;
        newEntity.length = len_max;

        if (len_max >= 1)
        {
            newEntity.misMatched = "";
            matchedEntities.push_back(newEntity);
            i += len_max - 1;
        }
        else
        {
            newEntity.misMatched = to_string(entity.position) + "-" + to_string(entity.length);
            matchedEntities.push_back(newEntity);
        }
    }

    cout << "Output: matched lowercase entities (position, length, mismatched)" << endl;
    for (const auto &entity : matchedEntities)
    {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }
}

// Function to write encoded data to a file
void writeEncodedDataToFile(const vector<int> &encodedData, const vector<pair<int, char>> &encodedSpecialChars, const vector<CharInfo> &nList, const vector<LowercaseChar> &lowercaseList, const string &filename)
{
    ofstream outFile(filename, ios::binary);
    if (!outFile)
    {
        cerr << "Error opening file for writing: " << filename << endl;
        exit(1);
    }

    size_t size = encodedData.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    outFile.write(reinterpret_cast<const char *>(encodedData.data()), size * sizeof(int));

    size = encodedSpecialChars.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    for (const auto &special : encodedSpecialChars)
    {
        outFile.write(reinterpret_cast<const char *>(&special.first), sizeof(int));
        outFile.write(reinterpret_cast<const char *>(&special.second), sizeof(char));
    }

    size = nList.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    outFile.write(reinterpret_cast<const char *>(nList.data()), size * sizeof(CharInfo));

    size = lowercaseList.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    outFile.write(reinterpret_cast<const char *>(lowercaseList.data()), size * sizeof(LowercaseChar));

    outFile.close();
    cout << "Encoded data written to " << filename << endl;
}

int main()
{
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
    string r_seq_int = replaceDNAChars(r_seq);
    string t_seq_int = replaceDNAChars(t_seq);

    // First level matching
    vector<Entity> matchedEntities1;
    vector<Entity> matchedEntities2;

    firstLevelMatching(r_seq_int, 2, t_seq_int, t_seq_int.length(), matchedEntities1);
    firstLevelMatching(r_seq_int, 2, t_seq_int, t_seq_int.length(), matchedEntities2);

    cout << "First Level Matching for Sequence 1:" << endl;
    for (const auto &entity : matchedEntities1)
    {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }

    cout << "First Level Matching for Sequence 2:" << endl;
    for (const auto &entity : matchedEntities2)
    {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }
    // Second level matching
    secondLevelMatching(matchedEntities1, matchedEntities2, 3);

    vector<Entity> matchedLowercaseEntities;
    lowercaseMatching(r_lowercaseList, t_lowercaseList, matchedLowercaseEntities);

    // Sequence information encoding
    vector<int> encodedData;
    encodeSequenceInformation(matchedEntities1, encodedData);
    encodeLowercaseInformation(matchedLowercaseEntities, encodedData);
    encodeAdditionalInformation(t_nList, encodedData);
    vector<pair<int, char>> encodedSpecialChars;
    encodeSpecialCharacters(t_specialList, encodedSpecialChars);

    writeEncodedDataToFile(encodedData, encodedSpecialChars, t_nList, r_lowercaseList, "compressed_data.bin");

    cout << "Encoded Data:" << endl;
    for (const auto &data : encodedData)
    {
        cout << data << " ";
    }
    cout << endl;

    return 0;
}