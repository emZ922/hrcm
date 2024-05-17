#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility> // For std::pair

using namespace std;

struct Entity
{
    std::pair<int,int> position;
    int length;
    bool matched;
};
#include <algorithm>
void firstLevelMatching(const string &r_seq, int k, const string &t_seq, int n_t)
{

    int l_max = k;
    int pos_max = 0;

    vector<Entity> matchedEntities;

    unordered_map<int, int> H;
    vector<int> L(r_seq.length(), -1); // Initialize array L with -1
    int hashValue = 0;
    string kMer = r_seq.substr(0, k);
    reverse(kMer.begin(), kMer.end());
    for (char c : kMer)
    {
        hashValue <<= 2;
        hashValue += c - '0';
    }
    L[0] = H.count(0) ? H[0] : -1;
    H[hashValue] = 0;
    cout << 0 << "-ti iz r_seq " << hashValue << endl;
    // Create hash table for reference B sequence
    for (int i = 1; i <= r_seq.length() - k; i++)
    {
        // Calculate the hash value of the k-mer which starts from i in t_seq
        hashValue >>= 2;
        hashValue += (r_seq[i + k - 1] - '0') << (k * 2 - 2);
        L[i] = H.count(hashValue) ? H[hashValue] : -1;
        H[hashValue] = i;
        cout << i << "-ti iz r_seq " << hashValue << endl;

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

    int pre_pos = 0;
    for (int i = 0; i <= n_t - k; i++)
    {
        hashValue = 0;
        kMer = t_seq.substr(i, i+k);
        reverse(kMer.begin(), kMer.end());
        for (char c : kMer)
        {
            hashValue <<= 2;
            hashValue += c - '0';
        }
        cout << i << "-ti iz t_seq " << hashValue << endl;
        int pos = H.count(hashValue) ? H[hashValue] : -1;
        cout << "Pos:" << pos << ",hash:" << hashValue << endl;

        if (pos <= -1)
        {
            // Mismatched k-mer
            Entity entity;
            entity.position = std::make_pair(pos, i);
            entity.length = k;
            entity.matched = false;
            matchedEntities.push_back(entity);
        }
        else
        {
            l_max = -1;
            pos_max = -1;
            int j = pos;
            while (j != -1)
            {
                // cout << pos << endl;
                int l = k;
                int p = pos;
                int r_id = j + k;
                int t_id = i + k;

                while (t_id < n_t && r_id < r_seq.length() && t_seq[t_id++] == r_seq[r_id++] )
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
            entity.position = std::make_pair(pos_max, i);
            entity.length = l_max;
            entity.matched = true;
            matchedEntities.push_back(entity);
        }
    }
    cout << "Output: matched entities (position, length, mismatched)" << endl;
    for (const auto &entity : matchedEntities)
    {
        cout << "Position: " << entity.position.first << "," << entity.position.second << ", Length: " << entity.length << endl;
        cout << "Matched: " << (entity.matched ? "true" : "false") << endl;
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
    cout << "welcome" << endl;

    string r_seq = replaceDNAChars("AGAG");
    string t_seq = replaceDNAChars("TGAGAGAAG");

    firstLevelMatching(r_seq, 2, t_seq, t_seq.length());

    return 0;
}
