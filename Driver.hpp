
//  Created by gordie2 on 11/3/20.
//  Copyright © 2020 Gordon Kolb. All rights reserved.
//

#ifndef Driver_hpp
#define Driver_hpp
#include <string>
#include <iostream>
#include <stdio.h>
//#include "xcode_redirect.hpp"
#include "table.hpp"
#include <unordered_map>
#include <getopt.h>
#include <vector>
#include <algorithm>
using namespace std;


#endif /* Driver_hpp */


class driver {
    bool quiet = false;
    unordered_map<string, Table> tables;
    
  public:
    void getOptions (int argc, char** argv){
        static option long_options[] = {
            // LONG        ARGUMENT USED?     (ignore) RETURN VALUE
            {"help",    no_argument,  nullptr, 'h'},
            {"quiet",   no_argument,  nullptr, 'q'},
            
            // this one tells getopt_long that there are no more options:
            {nullptr,      0,                 nullptr,  0}
        };
        
        // 'choice' is the value returned by getopt_long.
        int choice = 0;
                                        //short hand args with colon after if required argument after
        while ((choice = getopt_long(argc, argv, "hq", long_options, nullptr)) != -1) {
            
            if (choice == 'h') { // stats
                cout << "help description \n";
                exit(0);
            }
            if (choice == 'q') { // help
                quiet = true;
            }
        }
        //error checking???
    }
    
    void read_from_user() {
        string cmd;
        while (cmd != "QUIT"){
            cout << "% ";
            cin >> cmd;
            char switchy;
            switchy = cmd[0];
            
            switch (switchy) {
                //QUIT
                case 'Q':
                    //delete everything
                    cout << "Thanks for being silly!\n";
                    return;
                    break;
                //COMMENT
                case '#':
                    //get rest of line
                    getline(cin, cmd);
                    break;
                //CREATE
                case 'C':
                    create();
                    break;
                //REMOVE
                case 'R':
                    remove();
                    break;
                //INSERT
                case 'I':
                    insert();
                    break;
                //PRINT...ALL
                case 'P':
                    print();
                    break;
                case 'D':
                    delete_from();
                    break;
                case 'G':
                    generate();
                    break;
                case 'J': {
                    join();
                    break;
                }
                default:
                    cout << "Error: unrecognized command\n";
                    getline(cin, cmd);
            }
        }
    }
    
    void join() {
        string cmd;
        string tableName1;
        cin >> tableName1;
        
        auto it1 = tables.find(tableName1);
        if (it1 == tables.end()) {
        cout << "Error: " << tableName1 << " does not name a table in the database\n";
        getline(cin, cmd);
        return;
        }
        
        //takes care of AND
        cin >> cmd;
        
        string tableName2;
        cin>> tableName2;
        
        auto it2 = tables.find(tableName2);
        if (it2 == tables.end()) {
        cout << "Error: " << tableName2 << " does not name a table in the database\n";
        getline(cin, cmd);
        return;
        }
        
        it1->second.table_join(it2->second, quiet);
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
    
    
    
    //what if the map or unordered map i make has the same key multiple times...does it matter?
    //should the maps be of the types they are? tableentry and the vec of tableentry
    void generate() {
        string cmd;
        //takes care of FOR
        cin >> cmd;
        
        string tableName;
        cin >> tableName;
        
        auto it = tables.find(tableName);
        if (it == tables.end()) {
            cout << "Error: " << tableName << " does not name a table in the database\n";
            getline(cin, cmd);
            return;
        }
        it->second.table_generate();
    }
    
    void delete_from() {
        string cmd;
        //takes care of from
        cin >> cmd;
        
        string tableName;
        cin >> tableName;
        auto it = tables.find(tableName);
        if (it == tables.end()) {
            cout << "Error: " << tableName << " does not name a table in the database\n";
            getline(cin, cmd);
            return;
        }
        
        //takes care of where
        cin >> cmd;
        
        string colName;
        cin >> colName;
        
      
        char op;
        cin >> op;
        
        
        //less_than lcomp(TableEntry(2));
        
        
        switch (op) {
            case '<':
                it->second.table_delete_from<less_than>(colName, tableName);
                break;
            case '=':
                it->second.table_delete_from<equal_to>(colName, tableName);
                break;
            case '>':
                it->second.table_delete_from<greater_than>(colName, tableName);
                break;
            default:
                break;
        }
    }
    
    void print() {
        //takes care of FROM
        string cmd;
        cin >> cmd;
        
        string tableName;
        cin >> tableName;
        
        
        auto it = tables.find(tableName);
        if (it == tables.end()) {
        cout << "Error: " << tableName << " does not name a table in the database\n";
        getline(cin, cmd);
        return;
        }
        
        it->second.table_print_header_cols(quiet);
        
        
        
        //t.table_print(tables, quiet);
    }
    
    
    
    
    
    
    
    
    
    
    void insert() {
        string cmd;
        //takes care of INTO
       cin >> cmd;
       
       //reads in table name that will be inserted into
       string tableName;
       cin >> tableName;
        
        //reads in amount of rows
       int num_rows_add;
       cin >> num_rows_add;
        
        auto it = tables.find(tableName);
        if (it == tables.end()) {
            cout << "Error: " << tableName << " does not name a table in the database\n";
            getline(cin, cmd);
            
            for (int i = 0; i < num_rows_add; i++){
                getline(cin, cmd);
            }
            return;
        }
        it->second.table_insert(num_rows_add);
        //t.table_insert(tables);
    }
    
    void remove() {
        string cmd;
        //reading in name of table to be removed
        cin >> cmd;
        if (tables.find(cmd) == tables.end()) {
            cout << "Error: " << cmd << " does not name a table in the database\n";
            getline(cin, cmd);
            return;
        }
        cout << "Table " << cmd << " deleted\n";
        tables.erase(cmd);
    }
    
    void create() {
        string cmd;
        cin >> cmd;
        
        if (tables.find(cmd) != tables.end()) {
            cout << "Error: Cannot create already existing table " << cmd << "\n";
            getline(cin, cmd);
            return;
        }
        Table t;
        tables.emplace(make_pair(cmd, t));
        
        auto it = tables.find(cmd);
        it->second.table_create(cmd);
    }
    
    
    
    
    
};
