#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility> // For std::pair
#include <algorithm>

using namespace std;

struct Entity
{
    int position;
    int length;
    string misMatched;
};

int hashFunction(const string &kMer){
    int hashValue = kMer[0]-'0';
    for (int i = 1; i<kMer.length(); i++) {
        hashValue = hashValue*4+kMer[i] - '0';
    }
    return hashValue;
}
#include <map>
void firstLevelMatching(const string &r_seq, int k, const string &t_seq, int n_t)
{
    // Create a map to store the mapping from int to char
    std::map<char, char> intToCharMap;

    // Insert the mappings
    intToCharMap['0'] = 'A';
    intToCharMap['1'] = 'C';
    intToCharMap['2'] = 'G';
    intToCharMap['3'] = 'T';
    int l_max = k;
    int pos_max = 0;

    vector<Entity> matchedEntities;

    unordered_map<int, int> H;
    vector<int> L(r_seq.length(), -1); // Initialize array L with -1
    int hashValue = 0;
    string kMer = r_seq.substr(0, k);
    //reverse(kMer.begin(), kMer.end());
    hashValue = hashFunction(kMer);
    // for (char c : kMer)
    // {
    //     hashValue <<= 2;
    //     hashValue += c - '0';
    // }
    L[0] = H.count(0) ? H[0] : -1;
    H[hashValue] = 0;
    cout << 0 << "-ti iz r_seq " << hashValue << " " << kMer << endl;
    // Create hash table for reference B sequence
    for (int i = 1; i <= r_seq.length() - k; i++)
    {
        // Calculate the hash value of the k-mer which starts from i in t_seq
        // hashValue >>= 2;
        // hashValue += (r_seq[i + k - 1] - '0') << (k * 2 - 2);
        hashValue = hashFunction(r_seq.substr(i, k));
        L[i] = H.count(hashValue) ? H[hashValue] : -1;
        H[hashValue] = i;
        cout << i << "-ti iz r_seq " << hashValue <<  " " << r_seq.substr(i, k) << endl;

    }

    cout << "Contents of H:" << endl;
    for (const auto &pair : H)
    {
        cout << "Key: " << pair.first << ", Value: " << pair.second << endl;
    }

    // Printing L
    cout << "Contents of L:" << endl;
    for (int i = 0; i < L.size(); i++)
    {
        cout << "Index: " << i << ", Value: " << L[i] << endl;
    }
    string misStr = "";
    int pre_pos = 0;
    for (int i = 0; i <= n_t - k; i++)
    {
        hashValue = 0;
        kMer = t_seq.substr(i, k);
        // reverse(kMer.begin(), kMer.end());
        // for (char c : kMer)
        // {
        //     hashValue <<= 2;
        //     hashValue += c - '0';
        // }
        hashValue = hashFunction(kMer);
        //cout << i << "-ti iz t_seq " << hashValue <<  " " << kMer << endl;
        int pos = H.count(hashValue) ? H[hashValue] : -1;
        //cout << "Pos:" << pos << ",hash:" << hashValue << endl;

        if (pos > -1)
        {
            cout << r_seq.substr(i,k) << " " << t_seq.substr(i,k) << endl;
            l_max = -1;
            pos_max = -1;
            int j = pos;
            int r_id = j+k;
            int t_id = i+k;
            while (j != -1)
            {
                // cout << pos << endl;
                int l = k;
                int p = pos;
                r_id = j + k;
                t_id = i + k;

                while (t_id < n_t && r_id < r_seq.length() && t_seq[t_id++] == r_seq[r_id++])
                { 
                    l++;
                }

                if (l_max < l)
                {
                    l_max = l;
                    pos_max = j;
                }

                j = L[j];
            }
            Entity entity;
            entity.position = i;
            entity.length = l_max;
            r_id = pos_max + l_max;
            t_id = i + l_max;
            while (t_seq[t_id] != r_seq[r_id] && t_id < t_seq.length() && r_id < r_seq.length()) {
                misStr += intToCharMap[t_seq[t_id]];
                t_id++;r_id++;
            }
            entity.misMatched = misStr;
            matchedEntities.push_back(entity);
            misStr = "";
            i = t_id-1;
        }
    }
    cout << "Output: matched entities (position, length, mismatched)" << endl;
    for (const auto &entity : matchedEntities)
    {
        cout << "Position: " << entity.position << ", Length: " << entity.length << endl;
        cout << "Mismatched: " << entity.misMatched << endl;
    }
}

#include <iostream>
#include <string>

using namespace std;

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
            // Handle other characters if necessary
        }
    }

    return modifiedSequence;
}

int main()
{
    string r_seq = replaceDNAChars("AGATGGGCCCTTTAGGTATT");
    string t_seq = replaceDNAChars("AGCTGGTCCCTGAAGGAATC");

    firstLevelMatching(r_seq, 2, t_seq, t_seq.length()-1);

    return 0;
}
