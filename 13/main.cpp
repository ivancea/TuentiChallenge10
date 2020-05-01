#include <iostream>
#include <map>
#include <cmath>

using namespace std;

map<uint64_t, map<uint64_t, map<uint64_t, uint64_t>>> cache;

uint64_t count(uint64_t height, uint64_t x, uint64_t y) {
    if (height < 3) {
        return 0;
    }

    if (cache[height][x][y] != 0) {
        return cache[height][x][y];
    }

    uint64_t total = x * y * height;

    bool decreasing = true;

    do {
        x += 2;
        y += 2;

        if (decreasing) {
            height -= 2;
        } else {
            height += 1;
        }
        decreasing = !decreasing;

        total += x * height * 2;
        total += (y - 2) * height * 2;
    } while (height > 0);

    return cache[height][x][y] = total;
}

uint64_t findHeight(uint64_t packs) {
    if (packs < count(3, 1, 1)) {
        return 0;
    }

    uint64_t maxHeightExclusive = sqrt(packs);
    uint64_t minHeight = 3;

    while (maxHeightExclusive > minHeight + 1) {
        uint64_t height = minHeight + sqrt(maxHeightExclusive - minHeight);
        uint64_t value = count(height, 1, 1);

        if (value > packs) {
            maxHeightExclusive = height;
        } else {
            minHeight = height;
        }
    }

    return minHeight;
}

int main(){
    int n;
    cin >> n;
    cin.ignore();

    for(int caseNumber=1; caseNumber<=n; caseNumber++){
        uint64_t packs;
        cin >> packs;
        cerr << "Packs: " << packs << endl;

        uint64_t height = findHeight(packs);

        if (height > 0) {
            uint64_t usedPacks;

            for (uint64_t x=1, y=1; ;) {
                if (count(height, x, y) > packs) {
                    usedPacks = count(height, x, y-1);
                    break;
                }
                if (x == y) {
                    y++;
                } else {
                    x++;
                }
            }

            cout << "Case #" << caseNumber << ": " << height << " " << usedPacks << endl;
            cerr << "Case #" << caseNumber << ": " << height << " " << usedPacks << endl;
        } else {
            cout << "Case #" << caseNumber << ": " << "IMPOSSIBLE" << endl;
            cerr << "Case #" << caseNumber << ": " << "IMPOSSIBLE" << endl;
        }
    }
}