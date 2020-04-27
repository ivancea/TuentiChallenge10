#include <iostream>
#include <set>

using namespace std;

int main(){
    int n;
    cin >> n;

    for(int i=1; i<=n; i++){
        string a,b;
        cin >> a >> b;

        set<string> s;

        s.emplace(a);
        s.emplace(b);

        string result = "-";

        if (s.find("R") != s.end() && s.find("S") != s.end()) {
            result = "R";
        } else if (s.find("S") != s.end() && s.find("P") != s.end()) {
            result = "S";
        } else if (s.find("P") != s.end() && s.find("R") != s.end()) {
            result = "P";
        }

        cout << "Case #" << i << ": " << result << endl;
    }
}