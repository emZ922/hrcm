#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include <cctype>
#include <cstdlib>
#include <cmath>
#include <sys/time.h>  //Linux

#include <string.h>

using namespace std;

// Structures to hold information
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

// Function to read encoded data from a file
void readEncodedDataFromFile(const string &filename, vector<tuple<int, int, string>> &encodedData, vector<pair<int, char>> &encodedSpecialChars, vector<CharInfo> &nList, vector<CharInfo> &lowercaseList, string &referenceSequence, int &mt, int &line_width)
{
    ifstream inFile(filename, ios::binary);
    if (!inFile)
    {
        cerr << "Error opening file for reading: " << filename << endl;
        exit(1);
    }

    int data;
    size_t size;

    // Read encoded data
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    encodedData.resize(size);
    int length;
    for (auto &item : encodedData)
    {
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
    for (auto &special : encodedSpecialChars)
    {
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

    // Write seq length
    inFile.read(reinterpret_cast<char *>(&mt), sizeof(int));
    cout << "n" << mt << endl;
    // Write line length
    inFile.read(reinterpret_cast<char *>(&line_width), sizeof(int));
    cout << line_width << endl;
    inFile.close();
}

// Function to decode sequence information from encoded data
void decodeSequenceInformation(const vector<tuple<int, int, string>> &encodedData, vector<Entity> &matchedEntities)
{
    int previousPosition = 0;
    for (size_t i = 0; i < encodedData.size(); i++)
    {
        Entity entity;
        entity.position = previousPosition + get<0>(encodedData[i]);
        previousPosition = entity.position;

        entity.length = get<1>(encodedData[i]);
        entity.misMatched = get<2>(encodedData[i]); // Placeholder if mismatched info is used

        matchedEntities.push_back(entity);
    }
}

// Function to decompress data
string decompressData(const string &encodedFilename, const char *ref_filename, int &mt, int &line_width)
{
    vector<tuple<int, int, string>> encodedData;
    vector<pair<int, char>> encodedSpecialChars;
    vector<CharInfo> nList;
    vector<CharInfo> lowercaseList;
    string referenceSequence;
    readEncodedDataFromFile(encodedFilename, encodedData, encodedSpecialChars, nList, lowercaseList, referenceSequence, mt, line_width);

    // Decode sequence information
    vector<Entity> matchedEntities;
    decodeSequenceInformation(encodedData, matchedEntities);

    // Determine the length of the decompressed sequence
    char *tempSeq = new char[mt];

    //  Reconstruct the base sequence using the matched entities
    int start = 0;
    int read = 0;
    int i, j;

    for (const auto &entity : matchedEntities)
    {
        // cout << entity.position <<entity.misMatched<<entity.length << endl;
        read = entity.position;
        for (i = 0; i < entity.length; i++)
        {
            tempSeq[start++] = referenceSequence[read + i];
        }
        // start = start + entity.length;
        //  Handle mismatched portion if necessary
        for (j = 0; j < entity.misMatched.length(); j++)
        {
            tempSeq[start++] = entity.misMatched[j];
        }
        // read = read + entity.misMatched.length();
        // start = start + entity.misMatched.length();
    }
    tempSeq[start] = '\0';
    // cout << "f:" << tempSeq << endl;
    char *str = new char[mt];
    read = 0;
    start = 0;
    for (const auto &special : encodedSpecialChars)
    {
        // cout << decompressedSequence<<endl;
        for (i = 0; i < special.first; i++)
        {
            // cout << str[read] <<endl;
            str[start + i] = tempSeq[read++];
        }
        str[start + special.first] = special.second;
        start = start + special.first + 1;
        // cout << special.first << special.second << endl;
    }
    cout << read << " " << strlen(tempSeq) << endl;
    int tempSeqLen = strlen(tempSeq);
    for (i = read; i < tempSeqLen; i++)
    {
        str[start++] = tempSeq[i];
    }
    cout << 8 << endl;
    // cout << "s:" << str <<endl;

    //  Add 'N' characters
    start = 0;
    read = 0;
    string decompressedSequence1(mt, '?');

    for (const auto &nChar : nList)
    {
        // cout << nChar.position << "N" << nChar.length << endl;
        for (i = 0; i < nChar.position; i++)
        {
            decompressedSequence1[start++] = str[read++];
        }
        // start = start + nChar.position;
        for (i = 0; i < nChar.length; i++)
        {
            decompressedSequence1[start++] = 'N';
        }
        // start = start + nChar.length;
    }
    // cout << mt << " " << strlen(tempSeq) << " " << strlen(str) << endl;
    int strLen = strlen(str);
    for (i = read; i < strLen; i++)
    {
        decompressedSequence1[start++] = str[i];
        // cout << i <<endl;
    }
    // str[start] = '\0';
    // cout << "n:" << decompressedSequence1 <<endl;

    int str_len = start;

    str[mt] = '\0';

    // cout << "N:" << strlen(str) << endl;
    //   Reconstruct special characters
    int pos = 0;
    start = 0;
    read = 0;

    // strcpy();
    //  cout << "s:" << decompressedSequence1 << endl;
    //    Reconstruct lowercase characters
    cout << 56 << endl;
    start = 0;
    for (const auto &lowercase : lowercaseList)
    {
        // cout << lowercase.length << "L" << lowercase.position << endl;
        start = start + lowercase.position;
        for (i = 0; i < lowercase.length; i++)
        {
            decompressedSequence1[start + i] = tolower(decompressedSequence1[start + i]);
        }
        start = start + lowercase.length;
    }
    // cout << "d1:" << decompressedSequence1 << endl;
    start = 0;
    string decompressedSequence(static_cast<int>(round(mt / line_width)) + mt - 1, '?');

    int m;
    for (i = 0, m = 0; i < mt - 1; i++, m++)
    {
        if (m == line_width)
        {
            decompressedSequence[start++] = '\n';
            m = 0;
        }
        decompressedSequence[start++] = decompressedSequence1[i];
    }
    decompressedSequence[start++] = '\n';

    decompressedSequence[start] = '\0';

    cout << "o:" << "GGCTGXGCCggtttnAAAGGnnXXXTTXCNNNaaaTTTccACGTTTCTGT" << endl;
    // cout << "d:" << decompressedSequence << endl;
    cout << "o:" << "AGCTGGGCCCTTaaggtttnnnXXXTTTCCCGGGNNNaaaTTTccctttg" << endl;

    delete[] str;
    return decompressedSequence;
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

void decompressSingleFile(const char *ref_filename, const char *tar_filename, int &mt, int &line_width)
{
    cout << ref_filename << endl;
    string decompressedSequence = decompressData(tar_filename, ref_filename, mt, line_width);
    cout << "  " << endl;
    string output_filename = string(tar_filename).substr(0, string(tar_filename).find_last_of('.')) + ".fasta";
    writeDecompressedSequenceToFile(output_filename, decompressedSequence);
}

void decompressMultipleFiles(const char *ref_filename, const char *file_list, int &mt, int &line_width)
{
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
    cout << t_filenames[0] << endl;
    line_width = 0;
    for (const auto &t_filename : t_filenames)
    {
        decompressSingleFile(ref_filename, t_filename.c_str(), mt, line_width);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cerr << "Usage: hrcm decompress -r ref-file-path [-t tar-file-path | -f filename]" << endl;
        return 1;
    }

    string mode = argv[1];
    const char *ref_filename = argv[3];
    int mt = 0;
    int line_width = 0;

    struct timeval start_time;
    struct timeval end_time;
    unsigned long timer;
    gettimeofday(&start_time, NULL);
    if (mode == "decompress")
    {
        if (string(argv[4]) == "-t")
        {
            const char *tar_filename = argv[5];
            decompressSingleFile(ref_filename, tar_filename, mt, line_width);
        }
        else if (string(argv[4]) == "-f")
        {
            const char *file_list = argv[5];
            decompressMultipleFiles(ref_filename, file_list, mt, line_width);
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
    
    gettimeofday(&end_time, NULL);
    timer = 1000000 * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_usec - start_time.tv_usec;
    printf("Total decompression time = %lf ms; %lf s\n", timer / 1000.0, timer / 1000.0 / 1000.0);
    return 0;
}
