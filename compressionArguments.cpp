#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>

using namespace std;

// @ema
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


// @ema
// Extract reference file information
void extractReferenceFileInfo(const char *&filename, int &mr, string &r_seq, vector<LowercaseChar> &lowercaseList)
{
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

    // Read line by line, ignore newline
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }
        lines += line;
    }

    fclose(fp);
    lines += '\0';
    int l = 0;
    int pos = 0;
    bool counting = false;
    int r_len = 0;
    int start = 0;
    mr = lines.length();
    for (int i = 0; i < mr; i++)
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
        if (i == mr - 1 && counting)
        {
            LowercaseChar obj;
            obj.position = pos;
            obj.length = l;
            lowercaseList.push_back(obj);
        }
    }
}

// @ema
// Extract target file information
void extractTargetFileInfo(string filename, int &mt, int &line_width, string &t_seq, string &id, vector<CharInfo> &lowercaseList, vector<CharInfo> &nList, vector<SpecialChar> &specialList)
{
    FILE *fp = fopen(filename.c_str(), "r");
    if (fp == NULL)
    {
        printf("Error: fail to open file %s.\n", filename.c_str());
        exit(-1);
    }
    char line[1024];
    id = "";
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
        printf("Error: fail to read a line from file %s.\n", filename.c_str());
    }
    string lines = "";
    int i = 0;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (i == 0)
        {
            i = 1;
            line_width = strlen(line) - 1;
        }
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }
        lines += line;
    }

    fclose(fp);

    lines += '\0'; // terminate string
    mt = lines.length();


    int lowerLen = 0;
    int nLen = 0;

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
    int linesLen = lines.length();
    bool isUpper = false;
    for (i = 0; i < linesLen; i++)
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
            nPos++;
            specialPos++;
        }
        else
        {
            if (lines[i] == 'N')
            {
                // specialPos++;
                if (!nFlag)
                {
                    nFlag = true;
                    nLen = 1;
                    nStart = nPos; // save current position as start
                }
                else
                {
                    nLen++;
                }
            }
            else
            {
                if (lines[i] != '\0') // special character
                {
                    nPos++;
                    SpecialChar obj;
                    obj.position = specialPos;
                    obj.c = lines[i];
                    specialList.push_back(obj);
                    specialPos = 0;
                }
            }
        }
        if ((isUpper || i == linesLen - 1) && lowerFlag)
        {
            lowerStart = i;
            lowerFlag = false;
            CharInfo obj;
            obj.position = lowerPos;
            obj.length = lowerLen;
            lowercaseList.push_back(obj);
        }
        if ((lines[i] != 'N' || i == linesLen - 1) && nFlag)
        {
            // nStart = i;
            nFlag = false;
            CharInfo obj;
            obj.position = nStart;
            obj.length = nLen;
            nList.push_back(obj);
            nPos = 1;
        }
    }
}

// @ema
// Hash function for k-mers
int hashFunction(const string &kMer)
{
    int hashValue = kMer[0] - '0';
    int k = kMer.length();
    for (int i = 1; i < k; i++)
    {
        hashValue = hashValue * 4 + kMer[i] - '0';
    }
    return hashValue;
}

// @ema
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

// @ema
// Performs first level matching on base sequences of reference and target genome
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
    int rLen = r_seq.length();

    vector<int> L(rLen, -1); // Initialize array L with -1
    int hashValue = 0;
    string kMer = r_seq.substr(0, k);
    hashValue = hashFunction(kMer);
    L[0] = H.count(0) ? H[0] : -1; // Act as if all H values are -1
    H[hashValue] = 0;
    // Create hash table for reference B sequence
    for (int i = 1; i <= rLen - k; i++)
    {
        hashValue = hashFunction(r_seq.substr(i, k));
        L[i] = H.count(hashValue) ? H[hashValue] : -1;
        H[hashValue] = i;
    }

    string misStr = "";

    //  Iterate over t_seq
    int i;
    int kLen = 0;
    for (i = 0; i <= n_t - k;)
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

                while (t_id < n_t && r_id < rLen && t_id < n_t && t_seq[t_id++] == r_seq[r_id++])
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
            entity.position = pos_max;
            entity.length = l_max;

            // Find mismatched string
            t_id = i + l_max;
            r_id = pos_max + l_max;
            misStr = "";
            while (t_seq[t_id] != r_seq[r_id] && r_id < rLen && t_id < n_t) // find first different letter
            {
                misStr += intToCharMap[t_seq[t_id]];
                t_id++;
                r_id++;
            }

            entity.misMatched = misStr;
            matchedEntities.push_back(entity);
            misStr = "";
            i = t_id;
        }
        else
        {
            //  mismatched string of length k
            misStr = "";
            kLen = kMer.length();
            for (int ind = 0; ind < kLen; ind++)
            {
                misStr += intToCharMap[kMer[ind]];
            }

            Entity entity;
            entity.misMatched = misStr;
            misStr = "";
            entity.position = -1;
            entity.length = 0;
            matchedEntities.push_back(entity);
            i += k;
        }
    }
    if (i < n_t)
    {
        kMer = t_seq.substr(i, n_t);
        misStr = "";
        kLen = kMer.length();
        for (int ind = 0; ind < kLen; ind++)
        {
            misStr += intToCharMap[kMer[ind]];
        }
        Entity entity;
        entity.misMatched = misStr;
        entity.position = -1;
        entity.length = 0;
        matchedEntities.push_back(entity);
    }
}

// Second level matching function
void secondLevelMatching(const vector<vector<Entity>> &ref_entities_list, vector<vector<Entity>> &to_be_compressed_entities_list, int k)
{
    vector<vector<Entity>> matchedEntitiesList;

    for (int t = 0; t < to_be_compressed_entities_list.size(); ++t)
    {
        const auto &to_be_compressed_entities = to_be_compressed_entities_list[t];
        vector<Entity> matchedEntities;

        // Create hash table for reference matched entity sequences
        unordered_map<string, vector<int>> H; // hash table
        for (int i = 0; i < ref_entities_list[0].size(); ++i)
        {
            const Entity &entity = ref_entities_list[0][i];
            string kMer = to_string(entity.position) + "-" + to_string(entity.length);
            H[kMer].push_back(i);
        }

        // Perform matching for the to-be-compressed entities
        for (int i = 0; i < to_be_compressed_entities.size(); ++i)
        {
            const Entity &entity = to_be_compressed_entities[i];
            string kMer = to_string(entity.position) + "-" + to_string(entity.length);
            int len_max = 0;
            int pos_max = -1;

            // Check hash table for matches
            if (H.count(kMer))
            {
                for (int ref_pos : H[kMer])
                {
                    int len = k;
                    while (i + len < to_be_compressed_entities.size() && ref_pos + len < ref_entities_list[0].size() &&
                           to_be_compressed_entities[i + len].position == ref_entities_list[0][ref_pos + len].position &&
                           to_be_compressed_entities[i + len].length == ref_entities_list[0][ref_pos + len].length)
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
            newEntity.position = newEntity.position;
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

        matchedEntitiesList.push_back(matchedEntities);
    }

    to_be_compressed_entities_list = matchedEntitiesList;
}

void encodeSequenceInformation(const vector<Entity> &matchedEntities, vector<tuple<int, int, string>> &encodedData)
{
    int previousPosition = 0;

    for (const auto &entity : matchedEntities)
    {
        encodedData.push_back(make_tuple(entity.position - previousPosition, entity.length, entity.misMatched)); // Delta encoding for position
        previousPosition = entity.position;
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

void writeEncodedDataToFile(const vector<tuple<int, int, string>> &encodedData, const vector<pair<int, char>> &encodedSpecialChars, const vector<CharInfo> &nList, const vector<CharInfo> &lowercaseList, const string &referenceSequence, const string &filename, const string &id, int mt, int line_width)
{
    ofstream outFile(filename, ios::binary);
    if (!outFile)
    {
        cerr << "Error opening file for writing: " << filename << endl;
        exit(1);
    }

    size_t size = encodedData.size();
    int length;
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    for (const auto &item : encodedData)
    {
        outFile.write(reinterpret_cast<const char *>(&get<0>(item)), sizeof(int));
        outFile.write(reinterpret_cast<const char *>(&get<1>(item)), sizeof(int));
        length = get<2>(item).size();
        outFile.write(reinterpret_cast<const char *>(&length), sizeof(int));
        outFile.write(reinterpret_cast<const char *>(get<2>(item).c_str()), length);
    }

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
    outFile.write(reinterpret_cast<const char *>(lowercaseList.data()), size * sizeof(CharInfo));

    // Write reference sequence
    size = referenceSequence.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    outFile.write(referenceSequence.data(), size);

    // Write seq length
    outFile.write(reinterpret_cast<const char *>(&mt), sizeof(int));

    // Write line length
    outFile.write(reinterpret_cast<const char *>(&line_width), sizeof(int));

    // Write sequence id
    size = id.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    outFile.write(id.data(), size);

    outFile.close();
    cout << "Encoded data written to " << filename << endl;
}

// Function to write decompressed sequence to a .fa file
void writeDecompressedSequenceToFile(const string &filename, const string &decompressedSequence)
{
    ofstream outFile(filename);
    if (!outFile)
    {
        cerr << "Error opening file for writing: " << filename << endl;
        exit(1);
    }

    outFile << decompressedSequence << endl;

    outFile.close();
    cout << "Decompressed sequence written to " << filename << endl;
}

void compressSingleFile(const char *ref_filename, const char *tar_filename, int &mr, int &mt, int k, const string &output_filename, int &line_width)
{
    string r_seq;
    vector<LowercaseChar> r_lowercaseList;
    extractReferenceFileInfo(ref_filename, mr, r_seq, r_lowercaseList);
    string r_seq_int = replaceDNAChars(r_seq);

    string t_seq;
    string id;
    vector<CharInfo> t_lowercaseList;
    vector<CharInfo> t_nList;
    vector<SpecialChar> t_specialList;
    extractTargetFileInfo(tar_filename, mt, line_width, t_seq, id, t_lowercaseList, t_nList, t_specialList);
    string t_seq_int = replaceDNAChars(t_seq);
    vector<Entity> matchedEntities;
    firstLevelMatching(r_seq_int, k, t_seq_int, t_seq_int.length(), matchedEntities);

    vector<tuple<int, int, string>> encodedData;
    encodeSequenceInformation(matchedEntities, encodedData);
    vector<pair<int, char>> encodedSpecialChars;
    encodeSpecialCharacters(t_specialList, encodedSpecialChars);

    writeEncodedDataToFile(encodedData, encodedSpecialChars, t_nList, t_lowercaseList, r_seq, output_filename, id, mt, line_width);
}

void compressMultipleFiles(const char *ref_filename, const char *file_list, int mr, int mt, int line_width, int k, int percent)
{
    string r_seq;
    vector<LowercaseChar> r_lowercaseList;
    extractReferenceFileInfo(ref_filename, mr, r_seq, r_lowercaseList);
    string r_seq_int = replaceDNAChars(r_seq);

    ifstream file(file_list);
    if (!file)
    {
        cerr << "Error opening file list: " << file_list << endl;
        exit(1);
    }

    vector<string> t_filenames;
    string line;
    while (getline(file, line))
    {
        t_filenames.push_back(line);
    }
    file.close();

    vector<string> t_seq_int_vec;
    vector<vector<Entity>> matchedEntitiesList;
    vector<vector<CharInfo>> t_lowercaseList_vec;
    vector<vector<CharInfo>> t_nList_vec;
    vector<vector<SpecialChar>> t_specialList_vec;

    for (const auto &t_filename : t_filenames)
    {
        string t_seq;
        vector<CharInfo> t_lowercaseList;
        vector<CharInfo> t_nList;
        vector<SpecialChar> t_specialList;
        string id;
        extractTargetFileInfo(t_filename, mt, line_width, t_seq, id, t_lowercaseList, t_nList, t_specialList);
        string t_seq_int = replaceDNAChars(t_seq);

        vector<Entity> matchedEntities;
        firstLevelMatching(r_seq_int, k, t_seq_int, t_seq_int.length(), matchedEntities);

        matchedEntitiesList.push_back(matchedEntities);
        t_lowercaseList_vec.push_back(t_lowercaseList);
        t_nList_vec.push_back(t_nList);
        t_specialList_vec.push_back(t_specialList);
    }

    // Perform second level matching on a subset of sequences based on the percentage
    if (percent > 0)
    {
        vector<vector<Entity>> subset_matchedEntitiesList;
        for (int i = 0; i < t_filenames.size() * percent / 100; ++i)
        {
            subset_matchedEntitiesList.push_back(matchedEntitiesList[i]);
        }
        secondLevelMatching(matchedEntitiesList, subset_matchedEntitiesList, k);
    }
    for (int i = 0; i < t_filenames.size(); ++i)
    {
        string output_filename = t_filenames[i] + ".bin";
        vector<tuple<int, int, string>> encodedData;
        encodeSequenceInformation(matchedEntitiesList[i], encodedData);
        vector<pair<int, char>> encodedSpecialChars;
        encodeSpecialCharacters(t_specialList_vec[i], encodedSpecialChars);

        writeEncodedDataToFile(encodedData, encodedSpecialChars, t_nList_vec[i], t_lowercaseList_vec[i], r_seq, output_filename, "", mt, line_width);
    }
}

// the main function
int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cerr << "Usage: hrcm compress -r ref-file-path [-t tar-file-path | -f filename [percent]]" << endl;
        return 1;
    }

    string mode = argv[1];
    const char *ref_filename = argv[3];
    int k = 14; // k-mer length
    int mr = 0;
    int mt = 0;
    int line_width = 0;

    if (mode == "compress")
    {
        if (string(argv[4]) == "-t")
        {
            const char *tar_filename = argv[5];
            compressSingleFile(ref_filename, tar_filename, mr, mt, k, string(tar_filename).substr(0, string(tar_filename).find_last_of('.')) + ".bin", line_width);
        }
        else if (string(argv[4]) == "-f")
        {
            const char *file_list = argv[5];
            int percent = (argc > 6) ? stoi(argv[6]) : 10;
            compressMultipleFiles(ref_filename, file_list, mr, mt, line_width, k, percent);
        }
        else
        {
            cerr << "Invalid option: " << argv[4] << endl;
            return 1;
        }
    }
    else
    {
        cerr << "Invalid mode: " << mode << endl;
        return 1;
    }

    return 0;
}
