#include <iostream>
#include <string>
#include <map>
#include <set>

using namespace std;

const map<char, char> replacements({
    {'\'', 'q'},//
    {'-', '\''},//
    {',', 'w'},//
    {';', 'z'},//
    {'.', 'e'},//
    {'a', 'a'},//
    {'b', 'n'},//
    {'c', 'i'},//
    {'d', 'h'},//
    {'e', 'd'},//
    {'f', 'y'},//
    {'g', 'u'},//
    {'h', 'j'},//
    {'i', 'g'},//
    {'j', 'c'},//
    {'k', 'v'},//
    {'l', 'p'},//
    {'m', 'm'},//
    {'n', 'l'},//
    {'o', 's'},//
    {'p', 'r'},//
    {'q', 'x'},//
    {'r', 'o'},//
    {'s', ';'},//
    {'t', 'k'},//
    {'u', 'f'},//
    {'v', '.'},//
    {'w', ','},//
    {'x', 'b'},//
    {'y', 't'},//
    {'z', '/'},//
});

map<char, int> stats;

string decipheredLine(const string& line) {
    string deciphered(line);

    for (int i=0; i<line.size(); i++) {
        if (line[i] != ' ') {
            auto replacement = replacements.find(line[i]);

            if (replacement != replacements.end()) {
                deciphered[i] = replacement->second;
            }

            stats[line[i]]++;
        }
    }

    return deciphered;
}

int main(){
    int n;
    cin >> n;
    cin.ignore();

    for (int caseNumber = 1; caseNumber <= n; caseNumber++) {
        string line;
        getline(cin, line);

        string deciphered = decipheredLine(line);

        cout << "Case #" << caseNumber << ": " << deciphered << endl;
    }

    map<int, set<char>> ranking;

    for (auto p : stats) {
        ranking[p.second].insert(p.first);
    }

    for (auto p : ranking) {
        cerr << p.first << ": ";
        for (char c : p.second) {
            cerr << c << ", "; 
        }
        cerr << endl;
    }

    return 0;
}