#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cmath>

using namespace std;

using FloorIndex = int;
using GroupIndex = int;

struct Group {
    int employees;
    set<FloorIndex> floors;
};

FloorIndex getMaxFloor(const map<FloorIndex, int>& floorToEmployees) {
    int maxEmployees = -1;
    FloorIndex maxFloor = 0;

    for (const auto& floorIt : floorToEmployees) {
        if (floorIt.second > maxEmployees) {
            maxEmployees = floorIt.second;
            maxFloor = floorIt.first;
        }
    }

    return maxFloor;
}

/*int getMinFloor(const map<int, int>& floorToEmployees) {
    int minEmployees = -1;
    int minFloor = 0;

    for (const auto& floorIt : floorToEmployees) {
        if (floorIt.second < minEmployees || minEmployees == -1) {
            minEmployees = floorIt.second;
            minFloor = floorIt.first;
        }
    }

    return minFloor;
}*/

int solve(const int employeeCount, const vector<Group>& groups) {
    map<GroupIndex, map<FloorIndex, int>> groupToFloorToEmployees;
    map<FloorIndex, map<GroupIndex, int>> floorToGroupToEmployees;
    map<FloorIndex, int> floorToEmployees;

    // First, divide every group in N (floors) and assign that many employees to each floor (caution with remainders)
    for (int i=0; i<groups.size(); i++) {
        int remainingEmployees = groups[i].employees;
        int evaluatedFloors = 0;
        for (int floor : groups[i].floors) {
            int employeesForFloor = remainingEmployees / (groups[i].floors.size() - evaluatedFloors++);
            remainingEmployees -= employeesForFloor;
            groupToFloorToEmployees[i][floor] = employeesForFloor;
            floorToGroupToEmployees[floor][i] = employeesForFloor;
            floorToEmployees[floor] += employeesForFloor;
        }
    }
    
    const int minEmployeesPerFloor = ceil(employeeCount / (double) floorToEmployees.size());

    // Then, repeat until no movements done:
    // -> Find floor with the most employees (A)
    // -> Find floor with the fewer employees (B)
    // -> For every group targeting A, try to assign N (A - MIN) employees without exceeding A - 1 in the reassigned employees floors
    //    (MIN = minimum employees per floor (ceil(totalEmployees / floors)))
    //    (Reassign ordering by connected groups connected to floors with less employees)
    // Result: Floor with most employees * number of floors

    bool somethingChanged;

    do {
        somethingChanged = false;

        FloorIndex floorWithMostEmployees = getMaxFloor(floorToEmployees);

        int employeesToReassign = floorToEmployees[floorWithMostEmployees] - minEmployeesPerFloor;

        if (employeesToReassign > 0) {
            for (const auto& groupToEmployees : floorToGroupToEmployees[floorWithMostEmployees]) {
                GroupIndex currentGroupIndex = groupToEmployees.first;
                const Group& currentGroup = groups[groupToEmployees.first];
                for (FloorIndex currentFloorIndex : currentGroup.floors) {
                    int employeeDifference = floorToEmployees[floorWithMostEmployees] - floorToEmployees[currentFloorIndex];
                    if (employeeDifference > 1) {
                        int employeesToMove = min(employeeDifference - 1, groupToFloorToEmployees[currentGroupIndex][floorWithMostEmployees]);

                        if (employeesToMove > 0) {
                            floorToGroupToEmployees[floorWithMostEmployees][currentGroupIndex] -= employeesToMove;
                            floorToGroupToEmployees[currentFloorIndex][currentGroupIndex] += employeesToMove;
                            groupToFloorToEmployees[currentGroupIndex][floorWithMostEmployees] -= employeesToMove;
                            groupToFloorToEmployees[currentGroupIndex][currentFloorIndex] += employeesToMove;
                            floorToEmployees[floorWithMostEmployees] -= employeesToMove;
                            floorToEmployees[currentFloorIndex] += employeesToMove;

                            somethingChanged = true;
                            break;
                        }
                    }
                }

                if (somethingChanged) {
                    break;
                }
            }
        }

    } while (somethingChanged);

    return floorToEmployees[getMaxFloor(floorToEmployees)];
}

int main(){
    int n;
    cin >> n;

    for(int caseNumber=1; caseNumber<=n; caseNumber++){
        int floorCount, groupCount;
        cin >> floorCount >> groupCount;

        int employeeCount = 0;
        vector<Group> groups;

        for (int i=0; i<groupCount; i++) {
            Group group;
            int groupFloorCount;
            cin >> group.employees >> groupFloorCount;

            employeeCount += group.employees;

            for (int j=0; j<groupFloorCount; j++) {
                int groupFloor;
                cin >> groupFloor;
                group.floors.emplace(groupFloor);
            }

            groups.emplace_back(move(group));
        }

        int min = solve(employeeCount, groups);

        cout << "Case #" << caseNumber << ": " << min << endl;
        cerr << "Case #" << caseNumber << ": " << min << endl;
    }
}