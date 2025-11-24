#ifndef BRACKET_HPP
#define BRACKET_HPP

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <random>

using namespace std;

struct Node {
    string leftPlayer;
    string rightPlayer;
    string winner = "?";
    int leftScore = -1;
    int rightScore = -1;
    int matchId = 0;
    Node* left = nullptr;
    Node* right = nullptr;
    Node(string l = "", string r = "") : leftPlayer(l), rightPlayer(r) {}
};

class Bracket {
private:
    Node* root = nullptr;
    vector<Node*> matches; // internal nodes
    vector<Node*> leaves;  // initial players
    int nextMatchId = 1;
    mt19937 rng{random_device{}()};

    bool isPowerOf2(int n) { return (n & (n-1)) == 0; }
    int nextPowerOf2(int n) { int p=1; while(p<n) p*=2; return p; }

    Node* buildTree(vector<Node*>& nodes) {
        if (nodes.size() == 1) return nodes[0];

        vector<Node*> parents;
        for (size_t i=0; i<nodes.size(); i+=2) {
            Node* left = nodes[i];
            Node* right = (i+1<nodes.size()) ? nodes[i+1] : new Node("BYE","");
            Node* parent = new Node(left->winner=="?"? left->leftPlayer:left->winner,
                                    right->winner=="?"? right->leftPlayer:right->winner);
            parent->left = left;
            parent->right = right;
            parent->matchId = nextMatchId++;
            matches.push_back(parent);
            parents.push_back(parent);
        }
        return buildTree(parents);
    }

    Node* findNode(int matchId) {
        for (Node* m : matches)
            if (m->matchId == matchId) return m;
        return nullptr;
    }
    

    int computeRound(Node* n) {
    if (!root) return 0;
    queue<pair<Node*, int>> q;
    q.push({root, 1});
    while (!q.empty()) {
        pair<Node*, int> pr = q.front();
        Node* cur = pr.first;
        int r = pr.second;
        q.pop();
        if (cur == n) return r; // return round of this match
        if (cur->left) q.push({cur->left, r+1});
        if (cur->right) q.push({cur->right, r+1});
    }
    return 0;
}

    Node* LCA(Node* n, const string& p1, const string& p2) {
        if (!n) return nullptr;
        if (!n->left && !n->right) {
            if (n->winner == p1 || n->winner == p2) return n;
            return nullptr;
        }
        Node* leftLCA = LCA(n->left, p1, p2);
        Node* rightLCA = LCA(n->right, p1, p2);

        if (leftLCA && rightLCA) return n; // current node is LCA
        return leftLCA ? leftLCA : rightLCA;
    }

    Node* parentOf(Node* cur, Node* target) {
        if (!cur) return nullptr;
        if (cur->left == target || cur->right == target) return cur;
        Node* L = parentOf(cur->left, target);
        return L ? L : parentOf(cur->right, target);
    }

public:
    void buildBracket(vector<string> players) {
        int N = players.size();
        if (!isPowerOf2(N)) {
            int M = nextPowerOf2(N);
            for(int i=N; i<M; i++) players.push_back("BYE");
        }

        for (string& p : players) {
            Node* leaf = new Node(p,"");
            leaf->winner = p;
            leaves.push_back(leaf);
        }

        root = buildTree(leaves);
    }

    // Record manual winner or auto-random if empty
    bool recordResult(int matchId, string winner="") {
        Node* m = findNode(matchId);
        if (!m) return false;
        if (m->winner != "?") return false;

        string L = m->left->winner;
        string R = m->right->winner;

        // BYE auto advance
        if (L == "BYE") m->winner = R;
        else if (R == "BYE") m->winner = L;
        else {
            // random if winner not specified
            if (winner.empty()) {
                uniform_int_distribution<int> dist(0,15);
                int scoreL = dist(rng), scoreR = dist(rng);
                m->leftScore = scoreL;
                m->rightScore = scoreR;
                m->winner = (scoreL >= scoreR) ? L : R;
                cout<<"[Match "<<m->matchId<<"] "<<L<<" ("<<scoreL<<") vs "
                    <<R<<" ("<<scoreR<<") -> Winner: "<<m->winner<<"\n";
            } else {
                if (winner != L && winner != R) return false;
                m->winner = winner;
            }
        }

        propagate(root);
        return true;
    }

    string propagate(Node* n) {
        if (!n->left && !n->right) return n->winner;
        string lw = propagate(n->left);
        string rw = propagate(n->right);
        n->leftPlayer = lw;
        n->rightPlayer = rw;
        if (n->winner=="?") {
            if (lw=="BYE" && rw!="BYE") n->winner=rw;
            else if (rw=="BYE" && lw!="BYE") n->winner=lw;
        }
        return n->winner;
    }

    void printBracket() {
        if (!root) return;
        queue<pair<Node*,int>> q;
        q.push({root,1});
        int curRound=1;
        cout << "\n=== TOURNAMENT BRACKET ===\n";
        while(!q.empty()) {
            pair<Node*,int> pr = q.front(); 
            Node* n = pr.first;
            int r = pr.second;
            q.pop();

            if (r != curRound) { curRound=r; cout<<"\n"; }
            if (n->left && n->right) {
                cout<<"[R"<<r<<"] Match "<<n->matchId<<": "<<n->leftPlayer
                    <<" vs "<<n->rightPlayer<<" -> Winner: "<<n->winner<<"\n";
                q.push({n->left,r+1});
                q.push({n->right,r+1});
            }
        }
        cout << "===========================\n";
    }

    void wouldMeet(string p1, string p2) {
        Node* m = LCA(root,p1,p2);
        if (!m) { cout<<"They never meet.\n"; return;}
        cout << p1 << " vs " << p2 << " would meet at match "<<m->matchId
             <<" (Round "<<computeRound(m)<<")\n";
    }

    void pathToFinal(string player) {
        vector<int> path;
        findPath(root, player, path);
        cout<<"Path for "<<player<<": ";
        for(int id : path) cout<<id<<" ";
        cout<<"\n";
    }

    bool findPath(Node* n, string player, vector<int>& path) {
        if (!n) return false;
        if (!n->left && !n->right) return n->winner==player;
        bool inLeft = findPath(n->left, player, path);
        bool inRight = findPath(n->right, player, path);
        if (inLeft || inRight) {
            path.push_back(n->matchId);
            return true;
        }
        return false;
    }
};

#endif
