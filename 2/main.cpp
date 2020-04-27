#include <iostream>
#include <map>

using namespace std;

int main(){
    int n;
    cin >> n;

    for(int i=1; i<=n; i++){
        int matches;
        cin  >> matches;

        map<int, int> playersByWins;

        int currentWinner = 1;
        int currentWinnerWins = 0;

        for (int j = 0; j < matches; j++) {
            int a, b, m;
            cin >> a >> b >> m;

            int matchWinner = m == 1 ? a : b;

            playersByWins[matchWinner]++;

            if (playersByWins[matchWinner] > currentWinnerWins) {
                currentWinner = matchWinner;
                currentWinnerWins = playersByWins[matchWinner];
            }
        }

        cout << "Case #" << i << ": " << currentWinner << endl;
    }
}