#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cctype>  // For islower and toupper functions
#include <cstdlib> // For exit()

struct LowercaseChar
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

void extractReferenceFileInfo(const char *&filename, int mr)
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
    cout << "input:" << lines << endl;
    int l = 0;
    int pos = 0;
    bool counting = false;
    char r_seq[1024];
    int r_len = 0;
    int start = 0;
    vector<LowercaseChar> lowercaseList;
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
                pos = i-start;
            }
        }
        else
        {
            if (counting)
            {
                cout << i << endl;
                start = i;
                counting = false;
                LowercaseChar obj;
                obj.position = pos;
                obj.length = l;
                lowercaseList.push_back(obj);
            }
        }
        if (lines[i] == 'A' | lines[i] == 'T' | lines[i] == 'C' | lines[i] == 'G')
        {
            r_seq[r_len++] = lines[i];
        }
        if (i == mr-1 && counting) {
            LowercaseChar obj;
            obj.position = pos;
            obj.length = l;
            lowercaseList.push_back(obj);
        } 
    }
    cout << lines.length() <<endl;
    r_seq[r_len] = '\0';
    cout << "r_seq:" << r_seq << endl;
    for (const auto &item : lowercaseList)
    {
        cout << "Position: " << item.position << ", Length: " << item.length << endl;
    }
}

int main()
{
    const char *filename = "ref_seq.fa"; // Filename as specified
    int mr = 50;                         // Adjust this to your actual character limit if needed
    extractReferenceFileInfo(filename, mr);
    return 0;
}
