#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char** args){
    if (argc != 3) {
        cout << "Invalid number of args" << endl;
        return 1;
    }
    /* GET KEY
    string msg(args[1]);
    string ciphered(args[2]);

    string key;

    for (int i=0; i<msg.size(); i++) {
        int cipheredNumber = stoi(ciphered.substr(i*2, 2), nullptr, 16);
        key = to_string((int)(msg[i] ^ cipheredNumber)) + key;
    }

    cout << key << endl;
    */

    string key(args[1]);
    string ciphered(args[2]);

    string msg;

    for (int i=0; i<key.size(); i++) {
        int cipheredNumber = stoi(ciphered.substr(i*2, 2), nullptr, 16);
        msg += (char)((key[key.size() - 1 - i]-'0') ^ cipheredNumber);
    }

    cout << msg << endl;

    /* PARSED
    stringstream ss;

    for (int i=0; i<msg.size(); i++) {
        char c = msg[i];
        char asc_chr = c;
        int key_pos = key.size() - 1 - i;
        char key_char = key[key_pos];
        cout << (int)asc_chr << ":" << (int)key_char << endl;
        char crpt_chr = asc_chr ^ key_char;
        ss << hex << (int)(crpt_chr + 100);
    }

    cout << ss.str() << endl;*/
}