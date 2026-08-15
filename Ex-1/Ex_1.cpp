// Ex 1 - Caesar Cipher
#include<iostream>
#include <string>
using namespace std;

string cleanText(string txt){
	string cleaned;
	int i=0;
	for(char c : txt){
		if(c != ' '){
			cleaned.insert(cleaned.begin()+i,tolower(c));
			i++;
		}
	}
	return cleaned;
}

string encrypt(string msg, int k){
	string cleanedTxt = cleanText(msg);
	string cipherText;
	int i = 0;
	for(char c : cleanedTxt){
		cipherText.insert(cipherText.begin() + i,(c - 'a' + k) % 26 + 'a');
		i++;
	}

	return cipherText;
}

int main(){
	string pt;
	int k = 3;
	cout << " ====== Caesar Cipher with key as " << k << " ======\n";
	cout << "Enter the plaintext : ";
	cin >> pt;
	string ct = encrypt(pt, k);
	cout << "Plain text : " << pt << "\n";
	cout << "Cipher text : " << ct << "\n";
}
// #include<stdio.h>
// #include<ctype.h>
// #include<string.h>
// #include<stdlib.h>

// char* cleanText(char* msgtxt, int n){
// 	char* txt = malloc((n+1)*sizeof(char));
// 	int j = 0;
// 	for(int i=0; i<n; i++){
// 		if(msgtxt[i] != ' '){
// 			txt[j] = tolower(msgtxt[i]);
// 			j++;
// 		}
// 	}
// 	txt[j] = '\0';
// 	return txt;
// }

// char* encrypt(char* msg, int k, int n){
// 	char *m = cleanText(msg, n);
// 	char* cipherText = malloc((n+1)*sizeof(char));
// 	int i;
// 	for(i=0; i<strlen(m); i++){
// 		int ind = ((m[i] - 'a' + k) % 26) + 'a';
// 		cipherText[i] = (char)ind;
// 	}
// 	cipherText[n] = '\0';
// 	return cipherText;
// }

// int main(){
// 	char* pt = malloc(100*sizeof(char));
// 	printf("Enter the plaintext: ");
// 	scanf("%s", pt);
// 	int k;
// 	printf("Enter the number of positions to shift: ");
// 	scanf("%d", &k);
// 	char * ct = encrypt(pt,k,strlen(pt));
// 	printf("%s\n",ct );
// }