#include <iostream>
#include <vector>
#include <String>
#include <chrono>
#include <random>
using namespace std;


//Variables
int length;
vector<int> key;
vector<int> message;
vector<int> encrypted_m;
vector<int> decrypted_m;

// Random Generator
unsigned seed = chrono::system_clock::now().time_since_epoch().count();
mt19937 generator(seed);

//FUNCTION
// Generate a random key
void gen(vector<int> k)
{
    for (int i = 0; i < length; i++)
    {
        key.push_back(generator() % 2);
    }
}

// Encrypt
void encrypt()
{
    for (int i = 0; i < length; i++)
    {
        int n = key[i] == message[i] ? 0 : 1;
        encrypted_m.push_back(n);
    }
}

// Decrypt
void decrypt()
{
    for (int i = 0; i < length; i++)
    {
        int n = key[i] != encrypted_m[i] ? 1 : 0;
        decrypted_m.push_back(n);
    }
}

int main()
{
    string m = "";
    vector<int> k;

    cout << "Enter the message to encrypt:" << endl;
    cin >> m;

    length = m.length();
    // Convert string input to number
    for (int i = 0; i < m.length(); i++)
    {
        int n = m[i] == '1' ? 1 : 0;
        message.push_back(n);
    }

    return 0;
}