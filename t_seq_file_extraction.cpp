#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cctype>  // For islower and toupper functions
#include <cstdlib> // For exit()

struct CharInfo
{
    int position;
    int length;
};
using namespace std;
#include <cstring> // For strlen
void removeNewline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}

void extractReferenceFileInfo(const char *&filename, int mt)
{
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
            line[strlen(line) - 1] = '\0';
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
    lines += '\0';
    cout << "input:" << lines << endl;
    int lowerLen = 0;
    int specialLen = 0;
    int nLen = 0;
    int pos = 0;
    bool lowerFlag = false;
    bool specialFlag = false;
    bool nFlag = false;
    char t_seq[1024];
    int t_len = 0;
    int lowerStart = 0;
    int specialStart = 0;
    int nStart = 0;
    vector<CharInfo> lowercaseList;
    vector<CharInfo> nList;
    vector<CharInfo> specialList;
    for (int i = 0; i < mt; i++)
    {

        if (islower(lines[i]))
        {
            lines[i] = toupper(lines[i]);
            if (lowerFlag)
            {
                lowerLen++;
            }
            else
            {
                lowerFlag = true;
                lowerLen = 1;
                pos = i - lowerStart;
            }
        }

        if (lines[i] == 'A' | lines[i] == 'T' | lines[i] == 'C' | lines[i] == 'G')
        {
            t_seq[t_len++] = lines[i];
            if (specialFlag)
            {
                specialStart = i;
                specialFlag = false;
                CharInfo obj;
                obj.position = pos;
                obj.length = lowerLen;
                specialList.push_back(obj);
            }
        }
        else
        {
            if (lines[i] == 'N')
            {
                if (specialFlag)
                {
                    specialStart = i;
                    specialFlag = false;
                    CharInfo obj;
                    obj.position = pos;
                    obj.length = lowerLen;
                    specialList.push_back(obj);
                }
                if (!nFlag)
                {
                    nFlag = true;
                    nLen = 1;
                    nStart = i;
                }
                else
                {
                    nLen++;
                }
            }
            else
            {
                if (!specialFlag)
                {
                    specialFlag = true;
                    specialLen = 1;
                    specialStart = i;
                }
                else
                {
                    specialLen++;
                }
            }
        }
        if ((isupper(lines[i]) | i == mt - 1) && lowerFlag)
        {
            lowerStart = i;
            lowerFlag = false;
            CharInfo obj;
            obj.position = pos;
            obj.length = lowerLen;
            lowercaseList.push_back(obj);
        }
        if ((lines[i] != 'N' | i == mt - 1) && nFlag)
        {
            nStart = i;
            nFlag = false;
            CharInfo obj;
            obj.position = pos;
            obj.length = lowerLen;
            nList.push_back(obj);
        }
        if ((i == (mt - 1)) && specialFlag)
        {
            specialStart = i;
            specialFlag = false;
            CharInfo obj;
            obj.position = pos;
            obj.length = lowerLen;
            specialList.push_back(obj);
        }
    }
    cout << lines.length() << endl;
    t_seq[t_len] = '\0';
    cout << "t_seq:" << t_seq << endl;
    cout << "Lowercase:" << endl;
    for (const auto &item : lowercaseList)
    {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
    cout << "N character:" << endl;
    for (const auto &item : nList)
    {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
    cout << "Special:" << endl;
    for (const auto &item : specialList)
    {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
}

int main()
{
    const char *filename = "t_seq.fa"; // Filename as specified
    int mt = 50;                       // Adjust this to your actual character limit if needed
    extractReferenceFileInfo(filename, mt);
    return 0;
}