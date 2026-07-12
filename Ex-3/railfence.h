#ifndef rail_fence
#define rail_fence

#include<iostream>
using namespace std;
#include<vector>

string encrypt(string plaintext, int rails){
    vector<vector<char>> mat(rails,vector<char>(plaintext.length(),'\n'));
    int row =0, col = 0;
    int dir = true;
    for(char c : plaintext){
        if(row == 0) dir = true;
        if(row == rails -1) dir = false;

        mat[row][col++] = c;

        dir ? row++ : row--;
    }
    string ct = "";
    for(vector<char> v : mat){
        for( char c : v){
            if( c != '\n'){
                ct += c;
            }
        }
    }
    return ct;
}

string decrypt(string ciphertext, int rails){
    vector<vector<char>> mat(rails, vector<char>(ciphertext.size(),'\n'));
    int dir = true;
    int row=0, col=0;
    for(int i=0; i<ciphertext.length(); i++){
        if(row == 0) dir = true;
        if(row == rails-1) dir = false;

        mat[row][col++] = '*';

        dir ? row++ : row--;
    }
    int c = 0;
    for(int i=0; i<rails; i++){
        for(int j = 0; j<ciphertext.length(); j++){
            if(mat[i][j] == '*' && c < ciphertext.length()) mat[i][j] = ciphertext[c++];
        }
    }
    dir = true; row =0; col =0; 
    string pt = "";
   for(int i=0; i<ciphertext.length(); i++){
        if(row == 0) dir = true;
        if(row == rails-1) dir = false;

        if(mat[row][col] != '*'){
            pt += mat[row][col++];
        }

        dir ? row++ : row--;
    } 

    return pt;
}

#endif