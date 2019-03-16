#include "../Include/Bitcoin.h"
#include "../Include/Common_Types.h"
#include "../Include/File_Reader.h"
#include "../Include/Hash_Table.h"
#include "../Include/IO_Utils.h"
#include "../Include/Tokenizer.h"
#include "../Include/Transaction.h"
#include "../Include/Utils.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>

DateTime last_datetime;

static void __report(const char *tag, const char *fmt, va_list args) {
    fprintf(stderr, "%s", tag);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);
}

void Report_Error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __report("[ERROR]: ", fmt, args);
}

void Report_Warning(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __report("[WARNING]: ", fmt, args);
}

void Report_Response(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __report("[RESPONSE]: ", fmt, args);
}

void Read_Line_From_Stdin(struct Line_Buffer *line_buffer) {
    int ch;
    size_t i = 0;
    char *contents = line_buffer->contents;
    memset(contents, '\0', line_buffer->bytes);
    while (1) {
        ch = getchar();
        if (ch == '\n' || ch == EOF) break;
        ++i;
        if (i > line_buffer->bytes) {
            Line_Buffer_Resize(line_buffer, line_buffer->bytes << 1U);
        }
        contents[i - 1] = (char) ch;
    }
    contents[i] = '\0';
}

void Read_Balances_File_And_Update_Structures(Hash_Table *hash_table, AVL_Tree *tree) {
    int fd = open(parameters.balances_filename, O_RDONLY);
    if (fd == -1) {
        Report_Error("Could not open file <%s>. Exiting...", parameters.balances_filename);
        exit(EXIT_FAILURE);
    }
    File_Reader file_reader = File_Reader_Create(fd);
    Tokenizer tokenizer = {.delimiter = ' '};
    while (!file_reader.eof) {
        File_Reader_Read_Line(&file_reader);
        tokenizer.stream = file_reader.buffer.contents;
        size_t tokens = Tokenizer_Count_Remaining_Tokens(&tokenizer);
        char *walletID;
        Tokenizer_Next_Token(&tokenizer, &walletID);
        if ((tokens - 1) == 0) {
            Hash_Table_Insert(hash_table, Wallet_Create_Empty(walletID));
            continue;
        }
        char **bitcoinIDs = __MALLOC(tokens - 1, char*);
        size_t i = 0U;
        while (Tokenizer_Next_Token(&tokenizer, &bitcoinIDs[i])) ++i;
        Bitcoin **bitcoins = __MALLOC(tokens - 1, Bitcoin*);
        for (i = 0U; i != tokens - 1; ++i) {
            bitcoins[i] = Bitcoin_Create(bitcoinIDs[i]);
            bitcoins[i]->transaction_tree = Bitcoin_TT_Create(walletID, parameters.bitcoin_value);
            AVL_Node *search_res = AVL_Search(tree, bitcoinIDs[i]);
            if (search_res) {
                Report_Error("The same bitcoin exists in the initial state. Exiting...");
                exit(EXIT_FAILURE);
            }
            AVL_Insert(tree, &(bitcoins[i]->avl_node));
        }
        Wallet *wallet = Wallet_Create(walletID, parameters.bitcoin_value, bitcoins, tokens - 1);
        Hash_Table_Insert(hash_table, wallet);
        free(bitcoinIDs);
        free(bitcoins);
    }
    close(fd);
    File_Reader_Delete(&file_reader);
}

void Parse_Transaction_And_Update_Structures(char *transaction_string, Hash_Table *hash_table, AVL_Tree *tree,
                                             uint8_t report_result) {
    static Tokenizer tokenizer = {.delimiter = ' '};
    tokenizer.stream = transaction_string;
    char *transaction_string_copy = SAFE_STR_COPY(transaction_string);
    size_t tokens = Tokenizer_Count_Remaining_Tokens(&tokenizer);
    char *transactionID;
    if (tokens != 4 && tokens != 6) {
        Report_Response("Transaction <%s> is invalid. Canceling transaction.", transaction_string_copy);
        goto __EXIT__;
    }
    char *senderID;
    char *receiverID;
    char *_amount_str;
    uint64_t amount;
    char *date;
    char *time;
    DateTime dt;
    Tokenizer_Next_Token(&tokenizer, &transactionID);
    Tokenizer_Next_Token(&tokenizer, &senderID);
    Tokenizer_Next_Token(&tokenizer, &receiverID);
    Tokenizer_Next_Token(&tokenizer, &_amount_str);
    if (tokens == 6) {
        Tokenizer_Next_Token(&tokenizer, &date);
        Tokenizer_Next_Token(&tokenizer, &time);
        dt = DateTime_Create_From_String(date, time);
        if (DateTime_Compare(&dt, &last_datetime) < 0) {
            Report_Response("Date and Time are invalid. Canceling transaction <%s>", transaction_string_copy);
            goto __EXIT__;
        }
    } else {
        dt = DateTime_Create_New();
    }
    last_datetime = dt;
    if (!String_To_I64(_amount_str, (int64_t *) &amount)) {
        Report_Error("The amount you provided is not valid!");
        goto __EXIT__;
    }
    Wallet *sender_wallet = Hash_Table_Try_Get_Value(hash_table, senderID);
    Wallet *receiver_wallet = Hash_Table_Try_Get_Value(hash_table, receiverID);
    if (!sender_wallet || !receiver_wallet) {
        Report_Response("Sender or Receiver do not exist. Canceling transaction <%s>", transaction_string_copy);
        goto __EXIT__;
    }
    if (sender_wallet == receiver_wallet) {
        Report_Response("Sender and Receiver cannot be the same person. Canceling transaction <%s>",
                        transaction_string_copy);
    }
    if (sender_wallet->info.total_balance < amount) {
        Report_Response("Sender has insufficient money. Canceling transaction");
        goto __EXIT__;
    }
    Transaction *transaction = Transaction_Create(transactionID, senderID, receiverID, amount, &dt);
    Do_Transaction(tree, sender_wallet, receiver_wallet, transaction);
    if (report_result) Report_Response("Transaction <%s> was successfully completed.", transaction_string_copy);
    __EXIT__:
    free(transaction_string_copy);
}

void
Parse_Multiple_Transactions_And_Update_Structures(char *transactions_string, Hash_Table *hash_table, AVL_Tree *tree) {
    static Tokenizer tokenizer;
    tokenizer.delimiter = ';';
    tokenizer.stream = transactions_string;
    char *line;
    while (Tokenizer_Next_Token(&tokenizer, &line)) {
        char *transaction_string = Append_Transaction_ID_To_Transaction_String(line);
        Parse_Transaction_And_Update_Structures(transaction_string, hash_table, tree, 1);
        free(transaction_string);
    }
}

void Read_Transactions_File_And_Update_Structures(const char *filename, Hash_Table *hash_table, AVL_Tree *tree) {
    int transaction_file_from_params = !strcmp(filename, parameters.transactions_filename);
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        Report_Error("Could not open file <%s>.", filename);
        if (transaction_file_from_params) {
            exit(EXIT_FAILURE);
        }
        return;
    }
    File_Reader file_reader = File_Reader_Create(fd);
    while (!file_reader.eof) {
        File_Reader_Read_Line(&file_reader);
        if (!*file_reader.buffer.contents) continue;
        if (!transaction_file_from_params) {
            Parse_Multiple_Transactions_And_Update_Structures(file_reader.buffer.contents, hash_table, tree);
        } else {
            Parse_Transaction_And_Update_Structures(file_reader.buffer.contents, hash_table, tree, 0);
        }
    }
    close(fd);
    File_Reader_Delete(&file_reader);
}