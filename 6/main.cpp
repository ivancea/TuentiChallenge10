#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <cmath>
#include "sockets.cpp"
#include "strings.h"

using namespace std;

struct Pos {
    int x;
    int y;

    Pos(int _x, int _y) : x(_x), y(_y) {}
    Pos(const Pos& p) : x(p.x), y(p.y) {}

    bool operator==(const Pos& p) const {
        return x == p.x && y == p.y;
    }
    bool operator!=(const Pos& p) const {
        return !(*this == p);
    }
    bool operator<(const Pos& p) const {
        if (x < p.x) {
            return true;
        }
        if (x > p.x) {
            return false;
        }

        return y < p.y;
    }
};

struct Node {
    Pos position;
    bool visited = false;

    vector<Node*> connectedNodes;
    
    Node(const Pos& _position) : position(_position) {}
};

bool finishedReceiving(const string& str) {
    auto pos = str.find("---");

    if (pos == string::npos) {
        return false;
    }

    return str.find("---", pos + 3);
}

vector<string> readInitialLabyrinth(TCPClient& client) {
    string labyrinthStr;
    do {
        string str = client.recv();
        if (str.size() > 0) {
            labyrinthStr += str;
        }
    } while (!finishedReceiving(labyrinthStr));
    
    vector<string> labyrinth = split(labyrinthStr, "\n");

    labyrinth.erase(labyrinth.begin() + labyrinth.size() - 1);

    return labyrinth;
}

vector<string> readLabyrinth(TCPClient& client) {
    string labyrinthStr;
    do {
        string str = client.recv();
        if (str.size() > 0) {
            labyrinthStr += str;
        }
    } while (labyrinthStr.size() < 6*5);
    
    return split(labyrinthStr, "\n");
}

Pos findInLabyrinth(const vector<string>& labyrinth, char c) {
    for (int i=0; i<labyrinth.size(); i++) {
        int pos = labyrinth[i].find(c);
        if (pos != string::npos) {
            return Pos(pos, i);
        }
    }
    return Pos(-1, -1);
}

string getStep(const Pos& from, const Pos& to) {
    int xDiff = abs(to.x - from.x);
    int yDiff = abs(to.y - from.y);

    if (xDiff + yDiff != 3) {
        cout << "ERROR" << endl;
        exit(-1);
    }

    string xDir = to.x - from.x > 0 ? "R" : "L";
    string yDir = to.y - from.y > 0 ? "D" : "U";

    return to_string(yDiff) + yDir + to_string(xDiff) + xDir;
}

void fillWithRelated(const map<Pos, Node*>& allNodes, Node* node) {
    for (int i = -2; i<=2; i++) {
        for (int j=-2; j<=2; j++) {
            int x = node->position.x + i;
            int y = node->position.y + j;
            if (abs(i) + abs(j) == 3) {
                auto it = allNodes.find(Pos(x, y));
                if (it != allNodes.end()) {
                    node->connectedNodes.push_back(it->second);
                    it->second->connectedNodes.push_back(node);
                }
            }
        }
    }
}

void loadNodes(map<Pos, Node*>& allNodes, const Pos& zeroPoint, const vector<string>& labyrinth) {
    vector<Node*> createdNodes;
    for(int i=0; i<labyrinth.size(); i++){
        for(int j=0; j<labyrinth[i].size(); j++) {
            Pos currentPosition(j + zeroPoint.x, i + zeroPoint.y);
            if (labyrinth[i][j] != '#' && allNodes.find(currentPosition) == allNodes.end()) {
                Node* node = new Node(currentPosition);
                allNodes[currentPosition] = node;
                createdNodes.push_back(node);
                fillWithRelated(allNodes, node);
            }
        }
    }
}

list<Node*> findSteps(const map<Node*, int>& nodesToSteps, Node* nodeToReach) {
    list<Node*> steps;

    Node* currentNode = nodeToReach;

    while (nodesToSteps.at(currentNode) > 0) {
        steps.push_front(currentNode);
        int currentSteps = nodesToSteps.at(currentNode);

        for(Node* nextNode : currentNode->connectedNodes) {
            if (nodesToSteps.find(nextNode) != nodesToSteps.end() && nodesToSteps.at(nextNode) < currentSteps) {
                currentNode = nextNode;
                break;
            }
        }
    }

    return steps;
}

list<Node*> findSteps(Node* currentNode, Node* finalNode) {
    map<Node*, int> nodesToSteps;
    nodesToSteps[currentNode] = 0;

    queue<Node*> nodesToCheck;
    nodesToCheck.push(currentNode);

    while (!nodesToCheck.empty()) {
        Node* node = nodesToCheck.front();
        nodesToCheck.pop();
        int currentSteps = nodesToSteps[node];

        for(Node* nextNode : node->connectedNodes) {
            auto it = nodesToSteps.find(nextNode);
            if (it == nodesToSteps.end() || it->second > currentSteps + 1) {
                nodesToSteps[nextNode] = currentSteps + 1;
                nodesToCheck.push(nextNode);
                if (!nextNode->visited) {
                    return findSteps(nodesToSteps, nextNode);
                }
            }
        }
    }

    cout << "ERROR" << endl;
    return list<Node*>();
}

int main(){
    TCPClient client("52.49.91.111", 2003);
    client.setBlocking(true);
    
    map<Pos, Node*> allNodes;
    vector<string> labyrinth = readInitialLabyrinth(client);
    Pos zeroPointDiff(0, 0);

    const Pos finalPosition = findInLabyrinth(labyrinth, 'P');
    const Pos initialPosition = findInLabyrinth(labyrinth, 'K');

    loadNodes(allNodes, zeroPointDiff, labyrinth);

    Node* const finalNode = allNodes[finalPosition];
    Node* currentNode = allNodes[initialPosition];

    list<Node*> steps;
    
    while (client.isConnected() && currentNode != finalNode) {
        /*for(const string& str : labyrinth) {
            cout << str << endl;
        }*/
        currentNode->visited = true;

        Node* nextNode = nullptr;

        if (steps.empty() || steps.back()->position != finalPosition) {
            for(Node* node : currentNode->connectedNodes) {
                if (!node->visited) {
                    nextNode = node;
                    break;
                }
            }
        }

        if (nextNode == nullptr) {
            if (steps.empty()){
                steps = findSteps(currentNode, finalNode);
            }
            
            nextNode = steps.front();
            steps.pop_front();
        }

        if (nextNode != nullptr) {
            string nextStep = getStep(currentNode->position, nextNode->position);
            cout << currentNode->position.x << ", " << currentNode->position.y << endl;
            client.send(nextStep + "\n");
            zeroPointDiff.x += nextNode->position.x - currentNode->position.x;
            zeroPointDiff.y += nextNode->position.y - currentNode->position.y;
            currentNode = nextNode;
            steps.clear();
        }

        labyrinth = readLabyrinth(client);
        loadNodes(allNodes, zeroPointDiff, labyrinth);
    }
    for(const string& str : labyrinth) {
        cout << str << endl;
    }
    
    cout << "FINISHED" << endl;

    while (true) {
        string str = client.recv();
        if (!str.empty()) {
            cout << str << endl;
        }
    }

    return 0;
}