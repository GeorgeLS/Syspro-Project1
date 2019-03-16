#include "../Include/AVL_Tree.h"
#include "../Include/Attributes.h"
#include "../Include/Commands.h"
#include "../Include/Common_Types.h"
#include "../Include/DateTime.h"
#include "../Include/File_Reader.h"
#include "../Include/Hash_Table.h"
#include "../Include/IO_Utils.h"
#include "../Include/Macros.h"
#include "../Include/Tokenizer.h"
#include "../Include/Transaction.h"
#include "../Include/Utils.h"
#include "../Include/Wallet.h"
#include <argp.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define REQUEST_TRANSACTION "requestTransaction"
#define REQUEST_TRANSACTIONS "requestTransactions"
#define FIND_EARNINGS "findEarnings"
#define FIND_PAYMENTS "findPayments"
#define WALLET_STATUS "walletStatus"
#define BITCOIN_STATUS "bitcoinStatus"
#define TRACE_COIN "traceCoin"
#define EXIT "exit"

Program_Parameters parameters;

Line_Buffer stdin_buffer;
Hash_Table hash_table;
AVL_Tree bitcoin_set;

static struct option options[] = {
        {"a",    required_argument, NULL, 'a'},
        {"t",    required_argument, NULL, 't'},
        {"v",    required_argument, NULL, 'v'},
        {"h1",   required_argument, NULL, 's'},
        {"h2",   required_argument, NULL, 'r'},
        {"b",    required_argument, NULL, 'b'},
        {"help", no_argument,       NULL, 'h'},
        {0, 0, 0,                         0}
};

static inline __NO_RETURN__
void usage() {
    fprintf(stderr,
            "You must provide the following arguments:\n"
            "\t-a <File with initial balances of the users in the network>\n"
            "\t-t <Transaction File>\n"
            "\t-v <Bitcoin Value>\n"
            "\t-h1 <Number of buckets in sender hash table>\n"
            "\t-h2 <Number of buckets in receiver hash table>\n"
            "\t-b <Bucket size in bytes>\n");
    exit(EXIT_FAILURE);
}

static void parse_arguments(const int argc, char *const *argv) {
    if (argc < 13) usage();
    int c;
    while (1) {
        int option_index;
        c = getopt_long_only(argc, argv, "a:t:v:s:r:b:h", options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 'a': {
                parameters.balances_filename = SAFE_STR_COPY(optarg);
                break;
            }
            case 't': {
                parameters.transactions_filename = SAFE_STR_COPY(optarg);
                break;
            }
            case 'v': {
                int64_t value;
                if (!String_To_I64(optarg, &value)) {
                    Report_Error("The Bitcoin Value provided is invalid. Exiting...");
                    exit(EXIT_FAILURE);
                }
                parameters.bitcoin_value = value;
                break;
            }
            case 's': {
                size_t sender_buckets;
                if (!String_To_I64(optarg, (int64_t *) &sender_buckets)) {
                    Report_Error("The number of buckets provided for the senders is invalid. Exiting...");
                    exit(EXIT_FAILURE);
                }
                parameters.sender_buckets = sender_buckets;
                break;
            }
            case 'r': {
                size_t receiver_buckets;
                if (!String_To_I64(optarg, (int64_t *) &receiver_buckets)) {
                    Report_Error("The number of buckets provided for the receivers is invalid. Exiting...");
                    exit(EXIT_FAILURE);
                }
                parameters.receiver_buckets = receiver_buckets;
                break;
            }
            case 'b': {
                size_t bucket_size;
                if (!String_To_I64(optarg, (int64_t *) &bucket_size)) {
                    Report_Error("The Bucket size provided is invalid. Exiting...");
                    exit(EXIT_FAILURE);
                }
                parameters.bucket_size = bucket_size;
                break;
            }
            case 'h':
                usage();
            case '?':
                break;
            default:
                abort();
        }
    }
}

void Handle_Command(char *command_string) {
    static Tokenizer tokenizer;
    tokenizer.delimiter = ' ';
    tokenizer.stream = command_string;
    char *command;
    Tokenizer_Next_Token(&tokenizer, &command);
    size_t n_tokens = Tokenizer_Count_Remaining_Tokens(&tokenizer);
    if (!strcmp(command, REQUEST_TRANSACTION)) {
        if (n_tokens < 3 || n_tokens > 5) {
            Report_Error("Invalid syntax for command<requestTransaction>. Usage: -/requestTransaction");
            return;
        }
        Command_Request_Transaction(tokenizer.stream, &hash_table, &bitcoin_set);
    } else if (!strcmp(command, REQUEST_TRANSACTIONS)) {
        if (n_tokens == 1) {
            Command_Request_Multiple_Transactions_From_File(tokenizer.stream, &hash_table, &bitcoin_set);
        } else {
            Line_Buffer transaction_buffer = Line_Buffer_Create_With_Size(50);
            Line_Buffer_Write(&transaction_buffer, tokenizer.stream);
            while (1) {
                fprintf(stderr, "\t");
                Read_Line_From_Stdin(&stdin_buffer);
                if (!*stdin_buffer.contents) break;
                Line_Buffer_Append(&transaction_buffer, stdin_buffer.contents);
            }
            Command_Request_Multiple_Transactions(transaction_buffer.contents, &hash_table, &bitcoin_set);
            Line_Buffer_Delete(&transaction_buffer);
        }
    } else if (!strcmp(command, FIND_EARNINGS)) {
        if (n_tokens == 0 || n_tokens > 4) {
            Report_Error(
                    "Invalid syntax for command <findEarnings>. Usage: -/findEarnings walletID [time1][year1][time2][year2]");
            return;
        }
        char *walletID;
        Tokenizer_Next_Token(&tokenizer, &walletID);
        Command_Find_Earnings(walletID, tokenizer.stream, &hash_table);
    } else if (!strcmp(command, FIND_PAYMENTS)) {
        if (n_tokens == 0 || n_tokens > 4) {
            Report_Error(
                    "Invalid syntax for command <findPayments>. Usage: -/findPayments walletID [time1][year1][time2][year2]");
            return;
        }
        char *walletID;
        Tokenizer_Next_Token(&tokenizer, &walletID);
        Command_Find_Payments(walletID, tokenizer.stream, &hash_table);
    } else if (!strcmp(command, WALLET_STATUS)) {
        if (n_tokens != 1) {
            Report_Error("Invalid syntax for command <walletStatus>. Usage: -/walletStatus walletID");
            return;
        }
        char *walletID;
        Tokenizer_Next_Token(&tokenizer, &walletID);
        Command_Wallet_Status(walletID, &hash_table);
    } else if (!strcmp(command, BITCOIN_STATUS)) {
        if (n_tokens == 0 || n_tokens > 2) {
            Report_Error("Invalid syntax for command <bitcoinStatus>. Usage: -/bitcoinStatus bitcoinID");
            return;
        }
        char *bitcoinID;
        Tokenizer_Next_Token(&tokenizer, &bitcoinID);
        AVL_Node *res = AVL_Search(&bitcoin_set, bitcoinID);
        if (!res) {
            Report_Response("The bitcoinID <%s> you requested does not exist", bitcoinID);
            return;
        }
        Command_Bitcoin_Status(AVL_Get_Entry(res, Bitcoin, avl_node));
    } else if (!strcmp(command, TRACE_COIN)) {
        if (n_tokens == 0 || n_tokens > 2) {
            Report_Error("Invalid syntax for command <traceCoin>. Usage: -/traceCoin bitcoinID");
            return;
        }
        char *bitcoinID;
        Tokenizer_Next_Token(&tokenizer, &bitcoinID);
        AVL_Node *res = AVL_Search(&bitcoin_set, bitcoinID);
        if (!res) {
            Report_Response("The bitcoinID <%s> you requested does not exist", bitcoinID);
            return;
        }
        Command_Trace_Coin(AVL_Get_Entry(res, Bitcoin, avl_node));
    } else Report_Error("Unknown Command <%s>", command);
}

void Main_Loop(void) {
    while (1) {
        fprintf(stderr, "-/");
        Read_Line_From_Stdin(&stdin_buffer);
        if (!*stdin_buffer.contents) continue;
        if (!strcmp(stdin_buffer.contents, EXIT)) break;
        Handle_Command(stdin_buffer.contents);
    }
}

int main(int argc, char **argv) {
    srandom((unsigned int) ((time(NULL) ^ (intptr_t) printf) & (intptr_t) main));
    stdin_buffer = Line_Buffer_Create_With_Size(100);
    parse_arguments(argc, argv);
    size_t buckets = MAX(parameters.receiver_buckets, parameters.sender_buckets);
    hash_table = Hash_Table_Create(buckets, parameters.bucket_size, hash_function);
    AVL_Tree_Init(&bitcoin_set, AVL_Bitcoin_Cmp, AVL_Bitcoin_Node_Cmp, (Delete_Function) Bitcoin_Delete);
    Read_Balances_File_And_Update_Structures(&hash_table, &bitcoin_set);
    Read_Transactions_File_And_Update_Structures(parameters.transactions_filename, &hash_table, &bitcoin_set);
    Main_Loop();
    Hash_Table_Delete(&hash_table);
    AVL_Tree_Delete(&bitcoin_set);
    Line_Buffer_Delete(&stdin_buffer);
    free(parameters.balances_filename);
    free(parameters.transactions_filename);
    return EXIT_SUCCESS;
}