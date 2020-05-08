// C++onsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
using namespace std;

//#pragma comment(lib,"libssl.lib")
//#pragma comment(lib,"libcrypto.lib")



// key must be longer tha 16 byte, create a file "en_output.txt"
int my_aes_encrypt(const unsigned char* key, string filename)
{
	string file_content("");
	FILE* in_file, * out_file;

	if (!(in_file = fopen(filename.c_str(), "rb")))
	{
		printf("Fail to open file!\n");
		return -1;
	}

	// read file
	char ch = fgetc(in_file);
	while (ch != EOF)
	{
		file_content.push_back(ch);
		ch = fgetc(in_file);
	}

	//KCS5Padding：填充的原则是,如果长度少于16个字节，需要补满16个字节，补(16-len)个(16-len)例如：
	//"31325980"这个节符串是8个字节，16-8=8,补满后如：31325980+8个十进制的8
	//如果字符串长度正好是16字节，则需要再补16个字节的十进制的16。
	auto in_file_size = file_content.size();
	int fill_number = 0;

	fill_number = 16 - (in_file_size % 16);

	for (int i = 0; i < fill_number; i++)
	{
		file_content.push_back(fill_number);
	}

	// copy string to char*
	char* en_char = (char*)malloc(sizeof(char) * (file_content.size() + 1));
	strcpy(en_char, file_content.c_str());

	// // encode en_char to base64
	// unsigned char *b64_en_char = (unsigned char*)malloc(sizeof(char) * (file_content.size() + 1) / 6 + 1);
	// memset(b64_en_char, 0, )
	// 	EVP_EncodeBlock(b64_en_char, (unsigned char *)en_char, (file_content.size() + 1));

	// convert key
	AES_KEY aes_key;
	memset(&aes_key, 0x00, sizeof(AES_KEY));
	if (AES_set_encrypt_key(key, 128, &aes_key) < 0)
	{
		printf("Unable to set encryption key in AES...\n");
		return -1;
	}

	// generate initial vector
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0, AES_BLOCK_SIZE);

	// encrypt
	unsigned char* ouput = (unsigned char*)malloc(sizeof(unsigned char) * (file_content.size() + 1));
	memset(ouput, 0, file_content.size() + 1);
	AES_cbc_encrypt((unsigned char*)en_char, ouput, file_content.size(), &aes_key, iv, AES_ENCRYPT);

	// output
	if (!(out_file = fopen((filename+="_en").c_str(), "wb")))
	{
		printf("Fail to open file!\n");
		return -1;
	}
	fwrite(ouput, file_content.size(), 1, out_file);

	for (size_t i = 0; i <= file_content.size(); i++)
	{
		int temp = *(i + ouput);
		cout << temp << '\n';
	}

	std::fclose(in_file);
	std::fclose(out_file);
	return 0;
}

// key must be longer tha 16 byte, create a file "de_output.txt"
int my_aes_decrypt(const unsigned char* key, string filename)
{
	string file_content("");
	FILE* in_file, * out_file;

	if (!(in_file = fopen(filename.c_str(), "rb")))
	{
		printf("Fail to open file!\n");
		return -1;
	}

	// read file
	char ch = fgetc(in_file);
	while (ch != EOF)
	{
		file_content.push_back(ch);
		ch = fgetc(in_file);
	}

	// copy string to char*
	char* de_char = (char*)malloc(sizeof(char) * (file_content.size() + 1));
	strcpy(de_char, file_content.c_str());

	// convert key
	AES_KEY aes_key;
	memset(&aes_key, 0x00, sizeof(AES_KEY));
	if (AES_set_decrypt_key(key, 128, &aes_key) < 0)
	{
		printf("Unable to set decryption key in AES...\n");
		return -1;
	}

	// generate initial vector
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0, AES_BLOCK_SIZE);

	// decrypt
	unsigned char* ouput = (unsigned char*)malloc(sizeof(unsigned char) * (file_content.size() + 1));
	memset(ouput, 0, file_content.size() + 1);
	AES_cbc_encrypt((unsigned char*)de_char, ouput, file_content.size(), &aes_key, iv, AES_DECRYPT);

	// output
	if (!(out_file = fopen((filename+="_de").c_str(), "wb")))
	{
		printf("Fail to open file!\n");
		return -1;
	}
	fwrite(ouput, file_content.size(), 1, out_file);

	std::fclose(in_file);
	std::fclose(out_file);
	return 0;
}


int main()
{
	//TODO: Randomly generated key

	std::string filename;
	filename = "/home/redrn/Documents/program/Cryptogrpahy/DecentralizedStorageNetwork/test/test_graph";
	// std::cout << "input file name:";
	// std::cin >> filename;

	// Encrypt
	unsigned char key[33] = "8rrh1086omGe8qF0jgvxM53tASc46YHa";
	int err = my_aes_encrypt((unsigned char*)key, filename.c_str());
	if (err == 0)
	{
		std::cout << "\n"<< "success" << endl;
	}
	else
	{
		std::cout << "fail" << endl;
	}

	// Decrypt
	// unsigned char key[33] = "8rrh1086omGe8qF0jgvxM53tASc46YHa";
	err = my_aes_decrypt(key, filename+="_en");
	if (err == 0)
	{
		std::cout << "\n"<< "success" << endl;
	}
	else
	{
		std::cout << "fail" << endl;
	}
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
