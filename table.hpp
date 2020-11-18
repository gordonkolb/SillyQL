
//
//  Created by gordie2 on 11/3/20.
//  Copyright © 2020 Gordon Kolb. All rights reserved.
//

#ifndef table_hpp
#define table_hpp

#include <stdio.h>

#endif /* table_hpp */

#include <string>
#include <vector>
#include "TableEntry.h"
#include <unordered_map>
#include <map>

using namespace std;

class Table {
    public:
    
    vector<vector<TableEntry>> data;
    map<TableEntry, vector<size_t>> bst_index;
    // string is name of col and int is index
    unordered_map<string, int> cols;
    unordered_map<TableEntry, vector<size_t>> hash_index;
    vector<char> col_types;
    string table_name;
    string index_colName;
    char indType = ' ';
    bool index_broken = false;
    
    void table_join(Table & otherTable, bool & quiet){
        vector<pair<unordered_map<string, int>::iterator, int>> col_its;
        string cmd;
        
        //takes care of WHERE
        cin >> cmd;
        
        string colName1;
        cin >> colName1;
        
        if (cols.find(colName1) == cols.end()){
            cout << "Error: " << colName1 << " does not name a column in " << table_name << "\n";
            getline(cin, cmd);
            return;
        }
        
        //takes care of =
        cin >> cmd;
        
        
        string colName2;
        cin >> colName2;
        
        if (otherTable.cols.find(colName2) == otherTable.cols.end()){
            cout << "Error: " << colName2 << " does not name a column in " << otherTable.table_name << "\n";
            getline(cin, cmd);
            return;
        }
        
        //takes care of AND PRINT
        cin >> cmd >> cmd;
        
        int n;
        cin >> n;
        
        if (!quiet){
            col_its.reserve(size_t(n));
        }
        
        string currentColName;
        int currentColTable;
        
        for (int i = 0; i < n; ++i){
            cin >> currentColName;
            cin >> currentColTable;
            
            if (currentColTable == 1){
                auto it = cols.find(currentColName);
                if (it == cols.end()) {
                    cout << "Error: " << currentColName << " does not name a column in " << table_name << "\n";
                    getline(cin, cmd);
                    return;
                }
                else {
                    col_its.emplace_back(make_pair(it, currentColTable));
                }
            }
            else {
                auto it = otherTable.cols.find(currentColName);
                if (it == otherTable.cols.end()) {
                    cout << "Error: " << currentColName << " does not name a column in " << otherTable.table_name << "\n";
                    getline(cin, cmd);
                    return;
                }
                else {
                    col_its.emplace_back(make_pair(it, currentColTable));
                }
            }
        }
        
        if (!quiet) {
            for (auto j : col_its){
                cout << j.first->first << " ";
            }
            cout << "\n";
        }
        
        
        join_helper(otherTable, quiet, colName1, colName2, col_its);
    }
    
    
    void join_helper(Table & otherTable, bool & quiet, string & colName1, string & colName2, vector<pair<unordered_map<string, int>::iterator, int>> & col_its){
        
        size_t c1 = size_t(cols.at(colName1));
        
        
        
        if (otherTable.indType == 'h' && otherTable.index_colName == colName2){
            if (otherTable.index_broken){
                otherTable.table_fill_index(otherTable.index_colName);
            }
            join_with_index(otherTable, quiet, col_its, c1, otherTable.hash_index);
        }
        else if (otherTable.indType == 'b' && otherTable.index_colName == colName2){
            if (otherTable.index_broken){
                otherTable.table_fill_index(otherTable.index_colName);
            }
            join_with_index(otherTable, quiet, col_its, c1, otherTable.bst_index);
        }
        else {
            bst_index.clear();
            hash_index.clear();
            index_broken = true;
            
            unordered_map<TableEntry, vector<size_t>> join_hash_index;
            
            size_t c2 = size_t(otherTable.cols.at(colName2));
            for (size_t i = 0; i < otherTable.data.size(); ++i){
                join_hash_index[otherTable.data[i][c2]].emplace_back(i);
            }
            
            join_with_index(otherTable, quiet, col_its, c1, join_hash_index);
        }
    }
    
   
    
    
    template<typename T>
    void join_with_index(Table & otherTable, bool & quiet, vector<pair<unordered_map<string, int>::iterator, int>> & col_its, size_t & c1, T & index){
        
        
        int count = 0;
        for (size_t r = 0; r < data.size(); ++r){
            TableEntry comp = data[r][c1];
            
            auto it1 = index.find(comp);
            if (it1 != index.end()){
                count += int(index.find(comp)->second.size());
                auto it = index.find(comp)->second.begin();
                
                if (!quiet){
                    //loop the rows
                    while (it != index.find(comp)->second.end()){
                        //loop the cols
                        for (auto iter : col_its){
                            if (iter.second == 1){
                                cout << data[r][size_t(iter.first->second)] << " ";
                            }
                            else {
                                cout << otherTable.data[*it][size_t(iter.first->second)] << " ";
                            }
                        }
                        cout << "\n";
                        ++it;
                    }
                }
            }
        }
        cout << "Printed " << count << " rows from joining " << table_name << " to "
        << otherTable.table_name << "\n";
    }
    
    
    //make 3 functors
    class less_than {
        TableEntry my_te;
        size_t my_col;
    public:
        less_than(TableEntry te, size_t col) : my_te(te), my_col(col) {}
        bool operator()(vector<TableEntry> & other_te) {
            return other_te[my_col] < my_te;
        }
    };
            
    class greater_than {
        TableEntry my_te;
        size_t my_col;
    public:
        greater_than(TableEntry te, size_t col) : my_te(te), my_col(col) {}
        bool operator()(vector<TableEntry> & other_te) {
            return other_te[my_col] > my_te;
        }
    };
    
    
    class equal_to {
        TableEntry my_te;
        size_t my_col;
    public:
        equal_to(TableEntry te, size_t col) : my_te(te), my_col(col) {}
        bool operator()(vector<TableEntry> & other_te) {
            return other_te[my_col] == my_te;
        }
    };
    
    void table_create(string & cmd) {
        //reasing in table name
        table_name = cmd;
        cout << "New table " << cmd << " with column(s) ";
        
        //reading in n
        cin >> cmd;
        int col_count = stoi(cmd);
        col_types.reserve(size_t(col_count));
        cols.reserve(size_t(col_count));
        
        for (int i = 0; i < col_count; ++i){
            //reading in col types
            cin >> cmd;
            col_types.emplace_back(cmd[0]);
        }
        for (int j = 0; j < col_count; ++j){
            //reading in col names
            cin >> cmd;
            cout << cmd << " ";
            cols.emplace(make_pair(cmd, j));
        }
        cout << "created\n";
    }
    
    void table_insert(int & num_rows_add) {
        string cmd;
       
        
        //takes care of ROWS
        cin >> cmd;
        
        
        
        
        int curr_row = int(data.size());
        
        cout << "Added " << num_rows_add << " rows to " << table_name
             << " from position " << curr_row << " to "
             << (curr_row) + num_rows_add - 1 << "\n";
        
        //resizes data to rows and holds index of current row
        data.resize(size_t(curr_row + num_rows_add));
        
        for (int i = 0; i < num_rows_add; ++i){
            data[size_t(i)].reserve(col_types.size());
            for (size_t j = 0; j < col_types.size(); ++j){
                char s = col_types[j];
                switch (s) {
                    //boolean
                    case 'b':
                        bool input2;
                        cin >> input2;
                        data[size_t(curr_row)].emplace_back(input2);
                        break;
                    //string
                    case 's':
                        cin >> cmd;
                        data[size_t(curr_row)].emplace_back(cmd);
                        break;
                    //int
                    case 'i':
                        int input;
                        cin >> input;
                        data[size_t(curr_row)].emplace_back(input);
                        break;
                    //double
                    case 'd':
                        double input1;
                        cin >> input1;
                        data[size_t(curr_row)].emplace_back(input1);
                        break;
                }
            }
            if (indType != ' ' && index_broken == false){
                int colIndex = cols.at(index_colName);
                switch (indType){
                    case 'h': {
                        hash_index[data[size_t(curr_row)][size_t(colIndex)]].emplace_back(size_t(curr_row));
                        break;
                    }
                    case 'b': {
                        bst_index[data[size_t(curr_row)][size_t(colIndex)]].emplace_back(size_t(curr_row));
                        break;
                    }
                }
            }
            ++curr_row;
        }

    }
    
    
    void table_generate() {
        string index_type;
        cin >> index_type;
        
        string cmd;
        //takes care of INDEX
        cin >> cmd;
        //takes care of ON
        cin >> cmd;
        
        string colName;
        cin >> colName;;
        if (cols.find(colName) == cols.end()) {
            cout << "Error: " << colName << " does not name a column in "
            << table_name << "\n";
            return;
        }
        else {
            indType = index_type[0];
            index_colName = colName;
        }
        
        
        cout << "Created " << index_type << " index for table "
        << table_name << " on column " << index_colName << "\n";
        
        index_broken = true;
        //table_fill_index(index_colName);
    }
    
    void table_fill_index(string & colName){
        
        size_t col;
        col = size_t(cols.at(colName));
        
        bst_index.clear();
        hash_index.clear();
        
        for (size_t i = 0; i < data.size(); ++i){
            switch (indType) {
                case 'h': {
                    hash_index[data[i][col]].emplace_back(i);
                    break;
                }
                case 'b': {
                    bst_index[data[i][col]].emplace_back(i);
                    break;
                }
            }
        }
    }
    
    
    TableEntry get_table_entry(char varType){
        switch (varType) {
            case 'b': {
                bool varBool;
                cin >> varBool;
                TableEntry compBool(varBool);
                return compBool;
                break;
            }
            case 'i': {
                int varInt;
                cin >> varInt;
                TableEntry compInt(varInt);
                return compInt;
                break;
            }
            case 's': {
                string varString;
                cin >> varString;
                TableEntry compString(varString);
                return compString;
                break;
            }
            case 'd': {
                double varDouble;
                cin >> varDouble;
                TableEntry compDouble(varDouble);
                return compDouble;
                break;
            }
        }
        cerr << "get_table_entry tries to get a table entry with invalid type\n";
        exit(1);
    }
    
    
    template<typename Comp>
    void table_delete_from(const string & colName, const string & tableName) {
        auto iter_col = cols.find(colName);
        if (iter_col == cols.end()) {
                cout << "Error: " << colName << " does not name a column in "
                     << tableName << "\n";
            string cmd;
            getline(cin, cmd);
            return;
        }
        //split into 4 switch and read value into correct variable type
        //make a TableEntry of that variable
        size_t index = size_t(iter_col->second);
        char varType = col_types[index];
        
        //make comp with that TableEntry
        Comp comp(get_table_entry(varType), index);
        
        
        //remove_if(start, end, comp)
        auto it = remove_if(data.begin(), data.end(), comp);
        size_t size = size_t(it - data.begin());
        size_t rows_removed = size_t(data.end() - it);
        data.resize(size);
        
        cout << "Deleted " << rows_removed << " rows from " << tableName << "\n";
        
        if (rows_removed > 0){
            index_broken = true;
            hash_index.clear();
            bst_index.clear();
        }
    }
    
    void table_print_header_cols(bool quiet){
        string cmd;
        vector<unordered_map<string, int>::iterator> col_its;
        int num_cols;
        cin >> num_cols;
        
        if (!quiet){
            col_its.reserve(size_t(num_cols));
        }
        
        string col_name;
        
        for (int i = 0; i < num_cols; ++i){
            cin >> col_name;
            auto it = cols.find(col_name);
            if (it == cols.end()) {
                cout << "Error: " << col_name << " does not name a column in " << table_name << "\n";
                getline(cin, col_name);
                return;
            }
            else {
               col_its.emplace_back(it);
            }
        }
        
        //will either be where or all
        cin >> cmd;
        char type_print = cmd[0];
        
        switch (type_print) {
            //print all
            case ('A'):
                if (!quiet){
                    for (auto j : col_its){
                        cout << j->first << " ";
                    }
                    cout << "\n";
                }
                table_print_all(col_its, quiet);
                break;
            //print where
            case ('W'):{
                string col_for_compare;
                cin >> col_for_compare;
                
                auto it = cols.find(col_for_compare);
                if (it == cols.end()) {
                    cout << "Error: " << col_for_compare << " does not name a column in " << table_name << "\n";
                    getline(cin, col_name);
                    return;
                }
                
                if (!quiet){
                    for (auto j : col_its){
                        cout << j->first << " ";
                    }
                    cout << "\n";
                }
                
                if (indType == 'h' && index_colName == col_for_compare){
                    if (index_broken){
                        table_fill_index(index_colName);
                    }
                    table_print_hash_index(col_its, quiet, col_for_compare);
                }
                else if (indType == 'b' && index_colName == col_for_compare){
                    if (index_broken){
                        table_fill_index(index_colName);
                    }
                    table_print_bst_index(col_its, quiet, col_for_compare);
                }
                else {
                    table_print_no_index(col_its, quiet, col_for_compare);
                }
                break;
            }
            default:
                break;
        }
    }
    
    
      
    void table_print_bst_index(vector<unordered_map<string, int>::iterator> & col_its, bool & quiet, string & colName){
        char op;
        cin >> op;
        
        switch (op) {
            case '<':
                table_print_bst_less_than(col_its, colName, quiet);
                break;
            case '=':
                table_print_bst_equals(col_its, colName, quiet);
                break;
            case '>':
                table_print_bst_greater_than(col_its, colName, quiet);
                break;
            default:
                break;
        }
        
        
    }
    
    void table_print_bst_greater_than(vector<unordered_map<string, int>::iterator> & col_its, string & colName, bool & quiet){
        size_t index = size_t(cols.at(colName));
        char varType = col_types[index];
        
        //make comp with that TableEntry
        TableEntry greater_than = get_table_entry(varType);
        
        int count = 0;
        map<TableEntry, vector<size_t>>::iterator it;
        it = bst_index.upper_bound(greater_than);
        
        while (it != bst_index.end()){
            if (!quiet){
                for (size_t r = 0; r < it->second.size(); ++r){
                    for (size_t c = 0; c < col_its.size(); ++c){
                        cout << data[it->second[r]][size_t(col_its[c]->second)] << " ";
                    }
                    cout << "\n";
                }
            }
            count += int(it->second.size());
            ++it;
        }
        
        cout << "Printed " << count
        << " matching rows from " << table_name << "\n";
    }
    
    void table_print_bst_less_than(vector<unordered_map<string, int>::iterator> & col_its, string & colName, bool & quiet){
        
        size_t index = size_t(cols.at(colName));
        char varType = col_types[index];
        
        //make comp with that TableEntry
        TableEntry less_than = get_table_entry(varType);
        
        int count = 0;
        map<TableEntry, vector<size_t>>::iterator it;
        it = bst_index.begin();
        
        //bst_index.lower_bound(less_than);
        
        while (it != bst_index.end() && it->first < less_than){
            if (!quiet){
                for (size_t r = 0; r < it->second.size(); ++r){
                    for (size_t c = 0; c < col_its.size(); ++c){
                        cout << data[it->second[r]][size_t(col_its[c]->second)] << " ";
                    }
                    cout << "\n";
                }
            }
            count += int(it->second.size());
            ++it;
        }
        
        
        cout << "Printed " << count
        << " matching rows from " << table_name << "\n";
    }
    
    void table_print_bst_equals(vector<unordered_map<string, int>::iterator> & col_its, string & colName, bool & quiet){
        size_t index = size_t(cols.at(colName));
        char varType = col_types[index];
        
        //make comp with that TableEntry
        TableEntry equals = get_table_entry(varType);
        
        auto it = bst_index.find(equals);
        
        int count = 0;
        if (!quiet && it != bst_index.end()){
            for (auto i : it->second){
                ++count;
                for (size_t c = 0; c < col_its.size(); ++c){
                    cout << data[i][size_t(col_its[c]->second)] << " ";
                }
                cout << "\n";
            }
        }
        else if (it != bst_index.end()){
            count = int(it->second.size());
        }
        
        cout << "Printed " << count
        << " matching rows from " << table_name << "\n";
    }
   
    
    void table_print_hash_index(vector<unordered_map<string, int>::iterator> & col_its, bool & quiet, string & colName){
        
        char op;
        cin >> op;
        
        switch (op) {
            case '<':
                table_print_no_index_helper<less_than>(colName, col_its, quiet);
                //table_print_hash_less_than(col_its, colName, quiet);
                break;
            case '=':
                table_print_hash_equals(col_its, colName, quiet);
                break;
            case '>':
                //table_print_hash_greater_than(col_its, colName, quiet);
                table_print_no_index_helper<greater_than>(colName, col_its, quiet);
                break;
            default:
                break;
        }
    }
    
    
    void table_print_hash_equals(vector<unordered_map<string, int>::iterator> & col_its, string & colName, bool & quiet){
        size_t index = size_t(cols.at(colName));
        char varType = col_types[index];
        
        //make comp with that TableEntry
        TableEntry equals = get_table_entry(varType);
        
        auto it = hash_index.find(equals);
        
        int count = 0;
        if (!quiet && it != hash_index.end()){
            for (auto i : it->second){
                ++count;
                for (size_t c = 0; c < col_its.size(); ++c){
                    cout << data[i][size_t(col_its[c]->second)] << " ";
                }
                cout << "\n";
            }
        }
        else if (it != hash_index.end()){
            count = int(it->second.size());
        }
        
        cout << "Printed " << count
        << " matching rows from " << table_name << "\n";
    }
    
    
    
    void table_print_no_index(vector<unordered_map<string, int>::iterator> & col_its, bool & quiet, string & colName){
        
        char op;
        cin >> op;
        
        switch (op) {
            case '<':
                table_print_no_index_helper<less_than>(colName, col_its, quiet);
                break;
            case '=':
                table_print_no_index_helper<equal_to>(colName, col_its, quiet);
                break;
            case '>':
                table_print_no_index_helper<greater_than>(colName, col_its, quiet);
                break;
            default:
                break;
        }
        
    }
    
    template<typename Comp>
    void table_print_no_index_helper(const string & colName,
                                     vector<unordered_map<string, int>::iterator> & col_its, bool & quiet) {
        
        size_t index = size_t(cols.at(colName));
        char varType = col_types[index];
        
        //make comp with that TableEntry
        Comp comp(get_table_entry(varType), index);
        
        int count = 0;
        for (size_t r = 0; r < data.size(); ++r){
            if (comp(data[r])){
                ++count;
                if (!quiet){
                    for (size_t c = 0; c < col_its.size(); ++c){
                        cout << data[r][size_t(col_its[c]->second)] << " ";
                    }
                    cout << "\n";
                }
            }
        }

        cout << "Printed " << count
        << " matching rows from " << table_name << "\n";
        
    }
    
    
    
    
    void table_print_all(vector<unordered_map<string, int>::iterator>& col_its, bool & quiet) {
        
        if (!quiet) {
            for (int r = 0; r < int(data.size()); ++r){
                for (int c = 0; c < int(col_its.size()); ++c){
                    cout << data[size_t(r)][size_t(col_its[size_t(c)]->second)] << " ";
                }
                cout << "\n";
            }
        }
        
        cout << "Printed " << int(data.size())
        << " matching rows from " << table_name << "\n";
    }
};



