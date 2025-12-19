#include <leveldb/db.h>
#include <iostream>
#include <string>
#include <sstream>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <dbpath>" << std::endl;
        return 1;
    }

    std::string dbpath = argv[1];
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    
    leveldb::Status status = leveldb::DB::Open(options, dbpath, &db);
    if (!status.ok()) {
        std::cerr << "Failed to open database: " << status.ToString() << std::endl;
        return 1;
    }
    
    std::cout << "Database opened successfully: " << dbpath << std::endl;
    std::cout << "Available commands: set <key> <value>, get <key>, delete <key>, list, exit" << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        if (command == "exit" || command == "quit") {
            break;
        }
        else if (command == "set") {
            std::string key, value;
            if (!(iss >> key >> value)) {
                std::cerr << "Usage: set <key> <value>" << std::endl;
                continue;
            }
            status = db->Put(leveldb::WriteOptions(), key, value);
            if (status.ok()) {
                std::cout << "OK" << std::endl;
            } else {
                std::cerr << "Error: " << status.ToString() << std::endl;
            }
        }
        else if (command == "get") {
            std::string key;
            if (!(iss >> key)) {
                std::cerr << "Usage: get <key>" << std::endl;
                continue;
            }
            std::string value;
            status = db->Get(leveldb::ReadOptions(), key, &value);
            if (status.ok()) {
                std::cout << value << std::endl;
            } else if (status.IsNotFound()) {
                std::cerr << "Not found" << std::endl;
            } else {
                std::cerr << "Error: " << status.ToString() << std::endl;
            }
        }
        else if (command == "delete") {
            std::string key;
            if (!(iss >> key)) {
                std::cerr << "Usage: delete <key>" << std::endl;
                continue;
            }
            status = db->Delete(leveldb::WriteOptions(), key);
            if (status.ok()) {
                std::cout << "OK" << std::endl;
            } else {
                std::cerr << "Error: " << status.ToString() << std::endl;
            }
        }
        else if (command == "list") {
            leveldb::ReadOptions read_opts;
            read_opts.verify_checksums = false;
            leveldb::Iterator* it = db->NewIterator(read_opts);
            int count = 0;
            for (it->SeekToFirst(); it->Valid(); it->Next()) {
                std::cout << it->key().ToString() << " => " << it->value().ToString() << std::endl;
                count++;
            }
            if (!it->status().ok()) {
                std::cerr << "Iterator error: " << it->status().ToString() << std::endl;
            } else {
                std::cout << "Total entries: " << count << std::endl;
            }
            delete it;
        }
        else if (!command.empty()) {
            std::cerr << "Unknown command: " << command << std::endl;
        }
    }
    
    delete db;
    std::cout << "Database closed." << std::endl;
    return 0;
}