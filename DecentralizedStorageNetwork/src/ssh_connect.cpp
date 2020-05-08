#include <iostream>
#include <string>
#include <string.h>
#define LIBSSH_STATIC 1
#include <libssh/libssh.h>
#include <stdlib.h>
using namespace std;

/** SECTION: Temporary windows implementation for GNU/Unix functions: */
// strncasecmp
// int strncasecmp(const char *s1, const char *s2, int n)
// {
//     if (n && s1 != s2)
//     {
//         do
//         {
//             int d = tolower(*s1) - tolower(*s2);
//             if (d || *s1 == '\0' || *s2 == '\0')
//                 return d;
//             s1++;
//             s2++;
//         } while (--n);
//     }
//     return 0;
// }

// string getpass(string prompt, bool show_asterisk = true)
// {
//     const char BACKSPACE = 8;
//     const char RETURN = 13;

//     string password;
//     unsigned char ch = 0;

//     cout << prompt << endl;

//     DWORD con_mode;
//     DWORD dwRead;

//     HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

//     GetConsoleMode(hIn, &con_mode);
//     SetConsoleMode(hIn, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

//     while (ReadConsoleA(hIn, &ch, 1, &dwRead, NULL) && ch != RETURN)
//     {
//         if (ch == BACKSPACE)
//         {
//             if (password.length() != 0)
//             {
//                 if (show_asterisk)
//                     cout << "\b \b";
//                 password.resize(password.length() - 1);
//             }
//         }
//         else
//         {
//             password += ch;
//             if (show_asterisk)
//                 cout << '*';
//         }
//     }
//     cout << endl;
//     return password;
// }


/** SECTION: Code Copied from libssh tutorial*/
// //TODO: Rewrite this C function to modern C++ syntax
int verify_knownhost(ssh_session session)
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = NULL;
    ssh_key srv_pubkey = NULL;
    size_t hlen;
    char buf[10];
    char *hexa;
    char *p;
    int cmp;
    int rc;

    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0)
    {
        return -1;
    }

    rc = ssh_get_publickey_hash(srv_pubkey,
                                SSH_PUBLICKEY_HASH_SHA1,
                                &hash,
                                &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0)
    {
        return -1;
    }

    state = ssh_session_is_known_server(session);
    switch (state)
    {
    case SSH_KNOWN_HOSTS_OK:
        /** OK */

        break;
    case SSH_KNOWN_HOSTS_CHANGED:
        fprintf(stderr, "Host key for server changed: it is now:\n");
        ssh_print_hexa("Public key hash", hash, hlen);
        fprintf(stderr, "For security reasons, connection will be stopped\n");
        ssh_clean_pubkey_hash(&hash);

        return -1;
    case SSH_KNOWN_HOSTS_OTHER:
        fprintf(stderr, "The host key for this server was not found but an other"
                        "type of key exists.\n");
        fprintf(stderr, "An attacker might change the default server key to"
                        "confuse your client into thinking the key does not exist\n");
        ssh_clean_pubkey_hash(&hash);

        return -1;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
        fprintf(stderr, "Could not find known host file.\n");
        fprintf(stderr, "If you accept the host key here, the file will be"
                        "automatically created.\n");

        /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */

    case SSH_KNOWN_HOSTS_UNKNOWN:
        hexa = ssh_get_hexa(hash, hlen);
        fprintf(stderr, "The server is unknown. Do you trust the host key?\n");
        fprintf(stderr, "Public key hash: %s\n", hexa);
        ssh_string_free_char(hexa);
        ssh_clean_pubkey_hash(&hash);
        p = fgets(buf, sizeof(buf), stdin);
        if (p == NULL)
        {
            return -1;
        }

        cmp = strncasecmp(buf, "yes", 3);
        if (cmp != 0)
        {
            return -1;
        }

        rc = ssh_session_update_known_hosts(session);
        if (rc < 0)
        {
            fprintf(stderr, "Error %s\n", strerror(errno));
            return -1;
        }

        break;
    case SSH_KNOWN_HOSTS_ERROR:
        fprintf(stderr, "Error %s", ssh_get_error(session));
        ssh_clean_pubkey_hash(&hash);
        return -1;
    }

    ssh_clean_pubkey_hash(&hash);
    return 0;
}


/** SECTION: main function and own functions */
// Establish channel, execute command, and read console output
int channel_exec(ssh_session session, const char *cmd)
{
    int status;

    // Open channel
    ssh_channel channel = ssh_channel_new(session);
    if (channel == NULL)
    {
        return SSH_ERROR;
    }
    status = ssh_channel_open_session(channel);
    if (status != SSH_OK)
    {
        ssh_channel_free(channel);
        return status;
    }

    // Execute command cmd
    status = ssh_channel_request_exec(channel, cmd);
    if (status != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return status;
    }
    //read returned output
    char rd_buffer[256];
    int nbytes = ssh_channel_read(channel, rd_buffer, sizeof(rd_buffer), 0);
    while (nbytes > 0)
    {
        int i = 0;
        for (; i < nbytes; i++)
        {
            cout << rd_buffer[i];
        }
        if (i != (unsigned int)nbytes)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
        nbytes = ssh_channel_read(channel, rd_buffer, sizeof(rd_buffer), 0);
    }

    if(nbytes < 0)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return SSH_ERROR;
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}

// Authenticate using public key
int authenticate_pubkey(ssh_session session, const char *pk_file, const char *sk_file)
{
    int rc;
    ssh_key pub_key = NULL;
    ssh_key priv_key = NULL;

    rc = ssh_pki_import_pubkey_file(pk_file, &pub_key);
    if(rc!=SSH_OK)
    {
        cerr << "Error importing public key." << endl;
        ssh_key_free(pub_key);
        return rc;
    }

    rc = ssh_userauth_try_publickey(session, NULL, pub_key);
    if(rc!=SSH_AUTH_SUCCESS)
    {
        cerr << "Error offering public key." << endl;
        ssh_key_free(pub_key);
        return rc;
    }

    rc = ssh_pki_import_privkey_file(sk_file, NULL, NULL, NULL, &priv_key);
    if (rc != SSH_OK)
    {
        cerr << "Error importing private key." << endl;
        ssh_key_free(pub_key);
        ssh_key_free(priv_key);
        return rc;
    }

    rc = ssh_userauth_publickey(session, NULL, priv_key);
    if (rc != SSH_AUTH_SUCCESS)
    {
        cerr << "Error authenticating with private and public keys." << endl;
        ssh_key_free(pub_key);
        ssh_key_free(priv_key);
        return rc;
    }

    // rc = ssh_userauth_publickey_auto(session, NULL, NULL);

    // if (rc == SSH_AUTH_ERROR)
    // {
    //     fprintf(stderr, "Authentication failed: %s\n",
    //             ssh_get_error(session));
    //     return SSH_AUTH_ERROR;
    // }

    return rc;
}

int main()
{
    // SECTION: Establish connection
    const char *username = "neon";
    const char *host = "192.168.0.163";
    const char *priv_key = "/home/redrn/.ssh/id_rsa";
    const char *pub_key = "/home/redrn/.ssh/id_rsa.pub";

    int status; //temp variable for storing status of operation

    // Create ssh session
    ssh_session main_session = ssh_new();
    if(main_session == NULL)
    {
        cerr << "SSH session create failed" << endl;
        return -1;
    }

    // Connect to Server
    ssh_options_set(main_session, SSH_OPTIONS_HOST, host);
    ssh_options_set(main_session, SSH_OPTIONS_USER, username);
    status = ssh_connect(main_session);

    if(status != SSH_OK)
    {
        cerr << "Initial connection failed: " << ssh_get_error(main_session) << endl;
        ssh_free(main_session);
        return -1;
    }

    // Verify server identity
    if(verify_knownhost(main_session) < 0)
    {
        ssh_disconnect(main_session);
        ssh_free(main_session);
        return -1;
    }

    // Verify user identity
    // // FIXME: Change to cover user input
    // string pw;
    // cout << "Enter Password: " << endl;
    // cin >> pw;
    // // const char *password = getpass("Enter Password: ").c_str();
    // status = ssh_userauth_password(main_session, username, pw.c_str());
    // if(status != SSH_AUTH_SUCCESS)
    // {
    //     cerr << "Error authenticating with password: " << ssh_get_error(main_session) << endl;
    //     ssh_disconnect(main_session);
    //     ssh_free(main_session);
    //     return -1;
    // }
    status = authenticate_pubkey(main_session, pub_key, priv_key);
    if(status != SSH_AUTH_SUCCESS)
    {
        cerr << "Public key authentication failed: " << ssh_get_error(main_session) << endl;
        cerr << "rc: " << status << endl;
        ssh_disconnect(main_session);
        ssh_free(main_session);
        return -1;
    }

    // SECTION Remote Operation

    status = channel_exec(main_session, "ls");
    if(status != SSH_OK)
    {
        cerr << "Error executing commands: " << ssh_get_error(main_session) << endl;
        ssh_disconnect(main_session);
        ssh_free(main_session);
        return -1;
    }

    // End of SSH connection
    ssh_disconnect(main_session);
    ssh_free(main_session);
}