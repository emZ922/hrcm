#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility> // For std::pair
#include <algorithm>

using namespace std;

struct Entity {
    int position;
    int length;
    string misMatched;
};

int hashFunction(const string &kMer) {
    int hashValue = kMer[0] - '0';
    for (int i = 1; i < kMer.length(); i++) {
        hashValue = hashValue * 4 + kMer[i] - '0';
    }
    return hashValue;
}

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
            // Handle other characters if necessary
        }
    }

    return modifiedSequence;
}

#include <map>
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

void secondLevelMatching(const vector<Entity> &ref_entities, const vector<Entity> &to_be_compressed_entities, int k) {
    vector<Entity> matchedEntities;

    // Svaramo hash tablicu za referentne matchane sekvence 
    unordered_map<int, vector<int>> H; // hash tablica
    // std::cout << std::endl;
    for (int i = 0; i < ref_entities.size(); ++i) {
        const Entity &entity = ref_entities[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);
        // std::cout<< kMer<< std::endl;
        H[hashValue].push_back(i);
    }

    // Matchanje referentne sekvence s ciljnom sekvencom

    for (int i = 0; i < to_be_compressed_entities.size(); ++i) {
        const Entity &entity = to_be_compressed_entities[i];
        string kMer = to_string(entity.position) + "-" + to_string(entity.length);
        int hashValue = hashFunction(kMer);
        // Inicijalizacija len_max i pos_max
        int len_max = 0;
        int pos_max = -1;

        // Hash tablica za matchane sekvence
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

int main() {
    // Example reference and target sequences
    string r_seq = replaceDNAChars("AGATGGGCCCAGATGGGCCC");
    string t_seq1 = replaceDNAChars("AGATGGTCCCAGATGGTCCC");
    string t_seq2 = replaceDNAChars("AGATGGGCCCAGATGGGCCC");

    // First level matching
    vector<Entity> matchedEntities1;
    vector<Entity> matchedEntities2;

    firstLevelMatching(r_seq, 3, t_seq1, t_seq1.length(), matchedEntities1);
    firstLevelMatching(r_seq, 3, t_seq2, t_seq2.length(), matchedEntities2);

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

    return 0;
}
