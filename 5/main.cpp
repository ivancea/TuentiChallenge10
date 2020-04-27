#include <iostream>
#include <string>
#include <map>

using namespace std;

int main(){
    int n;
    cin >> n;

    for(int i=1; i<=n; i++){
        long long number;
        cin  >> number;

        long long result = -1;

        if (number != 59) { // Max 2 Tuentistic numbers, impossible to sum 59 with only 2 (it's always possible with more)
            if (number >= 20 && number < 30) {
                result = 1;
            } else if (number >= 40) {
                result = number / 20;
            }
        }


        cout << "Case #" << i << ": " << (result < 0 ? "IMPOSSIBLE" : to_string(result)) << endl;
    }
}