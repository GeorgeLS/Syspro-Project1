#include <stdio.h>
#include <inttypes.h>
#include "../Include/Commands.h"
#include "../Include/Wallet.h"
#include "../Include/Bitcoin.h"
#include "../Include/Pointer_Link.h"
#include "../Include/Utils.h"
#include "../Include/Tokenizer.h"

typedef struct {
    DateTime dt1;
    DateTime dt2;
} Interval;

static __INLINE__
Interval *Interval_Create(const char *d1, const char *t1, const char *d2, const char *t2) {
    if (!d1 && !t1 && !d2 && !t2) {
        return NULL;
    } else {
        Interval *interval = __MALLOC(1, Interval);
        interval->dt1 = DateTime_Create_From_String(d1, t1);
        interval->dt2 = DateTime_Create_From_String(d2, t2);
        return interval;
    }
}

static Interval *Parse_Interval(char *interval_str) {
    Tokenizer tokenizer = {.stream = interval_str, .delimiter = ' '};
    char *tokens[4] = {0};
    size_t i = 0U;
    while (Tokenizer_Next_Token(&tokenizer, &tokens[i++])) {}
    char *t1_str = NULL;
    char *t2_str = NULL;
    char *d1_str = NULL;
    char *d2_str = NULL;
    uint8_t n_dates = 0;
    uint8_t n_times = 0;
    for (i = 0U; i != 4U; ++i) {
        if (tokens[i]) {
            if (Is_Time(tokens[i])) ++n_times;
            else if (Is_Date(tokens[i])) ++n_dates;
            else {
                Report_Error("Invalid token <%s>", tokens[i]);
                return (Interval *) -1;
            }
        }
    }
    if (n_times == 2 && n_dates == 2) {
        t1_str = tokens[0];
        d1_str = tokens[1];
        t2_str = tokens[2];
        d2_str = tokens[3];
    } else if (n_times == 2 && n_dates == 0) {
        t1_str = tokens[0];
        t2_str = tokens[1];
    } else if (n_times == 0 && n_dates == 2) {
        d1_str = tokens[0];
        d2_str = tokens[1];
    } else if (n_times != 0 && n_dates != 0) {
        Report_Error("Invalid Datetime periods");
        return (Interval *) -1;
    }
    if (n_dates == 0 && n_times == 0) {
        return NULL;
    }
    return Interval_Create(d1_str, t1_str, d2_str, t2_str);
}

void Command_Request_Transaction(char *transaction_string, Hash_Table *hash_table, AVL_Tree *tree) {
    char *final_transaction_string = Append_Transaction_ID_To_Transaction_String(transaction_string);
    Parse_Transaction_And_Update_Structures(final_transaction_string, hash_table, tree, 1);
    free(final_transaction_string);
}

void
Command_Request_Multiple_Transactions_From_File(const char *restrict filename, Hash_Table *hash_table, AVL_Tree *tree) {
    Read_Transactions_File_And_Update_Structures(filename, hash_table, tree);
}

void Command_Request_Multiple_Transactions(char *transactions_string, Hash_Table *hash_table, AVL_Tree *tree) {
    Parse_Multiple_Transactions_And_Update_Structures(transactions_string, hash_table, tree);
}

static void
Print_Transactions_And_Total_Amount_In_Interval(const Generic_List *list, const DateTime *dt1, const DateTime *dt2) {
    uint64_t amount = 0U;
    Pointer_Link *transaction_link = NULL;
    if (!dt1 && !dt2) {
        list_for_each_entry(list, transaction_link, node) {
            Transaction *transaction = transaction_link->ptr;
            amount += transaction->amount;
        }
        printf("Total money in transactions : %" PRIu64"\n", amount);
        list_for_each_entry(list, transaction_link, node) {
            Transcation_Print(transaction_link->ptr);
        }
    } else {
        list_for_each_entry(list, transaction_link, node) {
            Transaction *transaction = transaction_link->ptr;
            DateTime *t_dt = &transaction->dateTime;
            if (DateTime_Compare(t_dt, dt1) > 0 && DateTime_Compare(t_dt, dt2) < 0) {
                amount += transaction->amount;
            }
        }
        printf("Total money in transactions: %" PRIu64 "\n", amount);
        list_for_each_entry(list, transaction_link, node) {
            Transaction *transaction = transaction_link->ptr;
            DateTime *t_dt = &transaction->dateTime;
            if (DateTime_Compare(t_dt, dt1) > 0 && DateTime_Compare(t_dt, dt2) < 0) {
                Transcation_Print(transaction);
            }
        }
    }
}

void Command_Find_Earnings(const char *walletID, char *interval_string, Hash_Table *hash_table) {
    Wallet *wallet = Hash_Table_Try_Get_Value(hash_table, walletID);
    if (!wallet) {
        Report_Response("The walletID <%s> does not exist", walletID);
        return;
    }
    const Generic_List *received = &wallet->received_list;
    if (List_Is_Empty(received)) {
        printf("The user with walletID <%s> hasn't received any money yet!", wallet->walletID);
        return;
    }
    Interval *interval = Parse_Interval(interval_string);
    if (interval == (Interval *) -1) return;
    if (!interval) {
        Print_Transactions_And_Total_Amount_In_Interval(received, NULL, NULL);
    } else {
        Print_Transactions_And_Total_Amount_In_Interval(received, &interval->dt1, &interval->dt2);
    }
    free(interval);
}

void Command_Find_Payments(const char *walletID, char *interval_string, Hash_Table *hash_table) {
    Wallet *wallet = Hash_Table_Try_Get_Value(hash_table, walletID);
    if (!wallet) {
        Report_Response("The walletID <%s> does not exist", walletID);
        return;
    }
    const Generic_List *send = &wallet->send_list;
    if (List_Is_Empty(send)) {
        printf("The user with walletID <%s> hasn't send any money yet!", wallet->walletID);
        return;
    }
    Interval *interval = Parse_Interval(interval_string);
    if (interval == (Interval *) -1) return;
    if (!interval) {
        Print_Transactions_And_Total_Amount_In_Interval(send, NULL, NULL);
    } else {
        Print_Transactions_And_Total_Amount_In_Interval(send, &interval->dt1, &interval->dt2);
    }
    free(interval);
}

void Command_Wallet_Status(const char *walletID, Hash_Table *hash_table) {
    Wallet *wallet = Hash_Table_Try_Get_Value(hash_table, walletID);
    if (!wallet) {
        Report_Response("The walletID <%s> you provided does not exist", walletID);
        return;
    }
    Wallet_Print_Status(wallet);
}

void Command_Bitcoin_Status(Bitcoin *bitcoin) {
    Bitcoin_TT_Print_Status(bitcoin);
}

void Command_Trace_Coin(Bitcoin *bitcoin) {
    Bitcoin_TT_Print_Transactions_Involved(&bitcoin->transaction_tree);
}