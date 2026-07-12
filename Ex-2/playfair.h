#ifndef playfair
#define playfair

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Position {
    int row;
    int col;
};

string cleanText(string txt) {
    string cleaned = "";
    for (char c : txt) {
        if (c != ' ') {
            c = tolower(c);
            if (c == 'j') c = 'i';
            cleaned += c;
        }
    }
    return cleaned;
}

vector<vector<char>> formMatrix(string key) {
    key = cleanText(key);
    
    vector<vector<char>> keyMat(5, vector<char>(5));
    vector<bool> filled(26, false);
    filled['j' - 'a'] = true;

    int row = 0, col = 0;

    for (char c : key) {
        if (!filled[c - 'a']) {
            keyMat[row][col] = c;
            filled[c - 'a'] = true;
            col++;
            if (col >= 5) {
                col = 0;
                row++;
            }
        }
    }

    for (int i = 0; i < 26; i++) {
        if (!filled[i]) {
            keyMat[row][col] = (char)(i + 'a');
            col++;
            if (col >= 5) {
                col = 0;
                row++;
            }
        }
    }

    return keyMat;
}

vector<string> splitPT(string pt) {
    pt = cleanText(pt);
    vector<string> digraphs;
    
    for (size_t i = 0; i < pt.size(); i += 2) {
        string pair = "";
        pair += pt[i];
        
        if (i + 1 == pt.size()) {
            pair += 'x';
        } 
        else if (pt[i] == pt[i + 1]) {
            pair += 'x';
            i--;
        } 
        else {
            pair += pt[i + 1];
        }
        digraphs.push_back(pair);
    }
    return digraphs;
}

Position findCharIndex(const vector<vector<char>>& mat, char s) {
    if (s == 'j') s = 'i';
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (mat[i][j] == s) {
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

string encrypt(const vector<string>& digraphs, const vector<vector<char>>& mat) {
    string ciphertext = "";

    for (string pair : digraphs) {
        Position p1 = findCharIndex(mat, pair[0]);
        Position p2 = findCharIndex(mat, pair[1]);

        if (p1.row == p2.row) {
            ciphertext += mat[p1.row][(p1.col + 1) % 5];
            ciphertext += mat[p2.row][(p2.col + 1) % 5];
        }
        else if (p1.col == p2.col) {
            ciphertext += mat[(p1.row + 1) % 5][p1.col];
            ciphertext += mat[(p2.row + 1) % 5][p2.col];
        }
        else {
            ciphertext += mat[p1.row][p2.col];
            ciphertext += mat[p2.row][p1.col];
        }
    }

    return ciphertext;
}

void printMatrix(const vector<vector<char>>& mat) {
    cout << "\n--- 5x5 Key Matrix ---" << endl;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
    cout << "----------------------\n" << endl;
}

string decrypt(string ciphertext, const vector<vector<char>>& mat) {
    string plaintext = "";
    
    // Safety check: Playfair cipher texts MUST be even in length
    if (ciphertext.length() % 2 != 0) {
        return "Error: Ciphertext length must be even.";
    }

    for (size_t i = 0; i < ciphertext.length(); i += 2) {
        Position p1 = findCharIndex(mat, ciphertext[i]);
        Position p2 = findCharIndex(mat, ciphertext[i+1]);

        // DEFENSIVE CHECK: Prevent segmentation fault if char is not found
        if (p1.row == -1 || p2.row == -1) {
            cerr << "Warning: Character '" << ciphertext[i] << "' or '" 
                 << ciphertext[i+1] << "' not found in matrix. Skipping pair." << endl;
            continue; 
        }

        if (p1.row == p2.row) {
            plaintext += mat[p1.row][(p1.col + 4) % 5];
            plaintext += mat[p2.row][(p2.col + 4) % 5];
        } else if (p1.col == p2.col) {
            plaintext += mat[(p1.row + 4) % 5][p1.col];
            plaintext += mat[(p2.row + 4) % 5][p2.col];
        } else {
            plaintext += mat[p1.row][p2.col];
            plaintext += mat[p2.row][p1.col];
        }
    }
    return plaintext;
}

#endif