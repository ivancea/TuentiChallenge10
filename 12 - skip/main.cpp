#include <iostream>
#include <set>
#include <map>
#include "C:/includes/Cpp/strings.h"

using namespace std;

long long countPossibilities(map<int, map<int, map<bool, long long>>>& possibilitiesByHasAnyByActualByNumber, const set<int>& banned, const int number, const int actual, const int count) {
    if (actual > number) {
        return 0;
    }

    if (possibilitiesByHasAnyByActualByNumber[actual][number].find(count > 0) != possibilitiesByHasAnyByActualByNumber[actual][number].end()) {
        return possibilitiesByHasAnyByActualByNumber[actual][number][count > 0];
    }

    long long total = 0;

    // If number isn't banned
    if (banned.find(actual) == banned.end()) {
        if (actual >= number && count > 0) {
            return 1;
        }
        total += countPossibilities(possibilitiesByHasAnyByActualByNumber, banned, number - actual, actual, count + 1);
    }
    
    // Skipping this number
    total += countPossibilities(possibilitiesByHasAnyByActualByNumber, banned, number, actual + 1, count);

    return possibilitiesByHasAnyByActualByNumber[actual][number][count > 0] = total;
}

int main(){
    int n;
    cin >> n;
    cin.ignore();

    for(int caseNumber=1; caseNumber<=n; caseNumber++){
        string line;
        getline(cin, line);
        
        vector<string> numbersStr = split(line, ' ');

        int number = stoi(numbersStr[0]);
        set<int> banned;

        for (int i = 1; i<numbersStr.size(); i++) {
            banned.insert(stoi(numbersStr[i]));
        }

        map<int, map<int, map<bool, long long>>> possibilitiesByHasAnyByActualByNumber;

        long long max = countPossibilities(possibilitiesByHasAnyByActualByNumber, banned, number, 1, 0);

        cout << "Case #" << caseNumber << ": " << max << endl;
    }
}