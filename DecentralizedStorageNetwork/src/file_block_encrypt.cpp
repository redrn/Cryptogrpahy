#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <openssl/aes.h>
#include <openssl/evp.h>
using namespace std;

void toBase64(string &s_in)
{
    BIO *b_s = BIO_new(BIO_s_mem());
    BIO *b64_f = BIO_new(BIO_f_base64());

    b_s = BIO_push(b64_f, b_s);

    cout << "IN: " << s_in.length();
    BIO_write(b_s, s_in.c_str(), s_in.length());

    char *pp;
    int sz = BIO_get_mem_data(b_s, &pp);

    cout << "OUT: " << sz << endl;

    s_in.assign(pp, sz);
    //std::cout << sz << " " << std::string(pp,sz) << std::endl;

    BIO_free(b64_f); // TODO ret error potential
    BIO_free(b_s);   //
}

int main(int argc, char *argv[])
{

    ifstream test_file("test_file.txt");
    ifstream test_graph("test_graph.jpg");
    ofstream fout("test_graph_out.jpg");

    string line;
    int i = 0;
    
    while(getline(test_graph, line) && i<10)
    {
        fout << line << endl;
        i++;
    }

    return 0;
}