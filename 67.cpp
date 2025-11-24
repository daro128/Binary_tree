#include "67.hpp"
#include <iostream>
#include <vector>
using namespace std;

int main() {
    Bracket T;

    vector<string> players = {"A","B","C","D","E","F","G","H"};
    T.buildBracket(players);

    cout << "\n--- INITIAL BRACKET ---\n";
    T.printBracket();

    // Round 1: random results
    cout << "\n--- ROUND 1 RESULTS ---\n";
    for (int i = 1; i <= 4; i++) T.recordResult(i); // first 4 matches
    T.printBracket();

    // Round 2: semifinal
    cout << "\n--- ROUND 2 RESULTS ---\n";
    for (int i = 5; i <= 6; i++) T.recordResult(i); // matches 5-6
    T.printBracket();

    // Round 3: final
    cout << "\n--- FINAL MATCH ---\n";
    T.recordResult(7); // match 7
    T.printBracket();

    // Path to final
    cout << "\n--- PATH TO FINAL ---\n";
    T.pathToFinal("A");
    T.pathToFinal("F");

    // Meeting query
    cout << "\n--- WOULD MEET QUERY ---\n";
    T.wouldMeet("C","H");
    T.wouldMeet("A","D");

    return 0;
}
