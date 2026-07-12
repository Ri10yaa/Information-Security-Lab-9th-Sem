#include<iostream>
#include<vector>
using namespace std;

string encrypt(string pt, vector<int> key){
    int col = key.size();
    int row = (pt.length() + col - 1) / col;
    vector<vector<char>> grid(row,vector<char>(col,','));
    int k = 0;
    for(int i=0; i<row; i++){
        for(int j=0; j<col && k <pt.length(); j++){
            grid[i][j] = pt[k++];
        }
    }

    string ct = "";
    for(int i=0; i<col; i++){
        int c = key[i]-1;
        for(int j=0; j<row; j++){
            ct += grid[j][c];
        }
    }
    
    return ct;

}

string decrypt(string ct, vector<int> key){
    int col = key.size();
    int row = (ct.length() + col - 1) / col;

    vector<vector<char>> grid(row, vector<char>(key.size()));
    int k = 0;
    for(int i=0; i<col && k < ct.length(); i++){
        int c = key[i]-1;
        for(int j=0; j< row; j++){
            grid[j][c] = ct[k++];
        }
    }

    string pt = "";
    for(int i=0; i < row; i++){
        for(int j=0; j<col; j++){
            if(grid[i][j]!= ',') pt += grid[i][j];
        }
    }

    return pt;
}