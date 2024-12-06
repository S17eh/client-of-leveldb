#include <rocksdb/db.h>

#include <chrono>  // For timing
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;
using json = nlohmann::json;
using namespace std::chrono;

// Global pointer to the database instance
rocksdb::DB* db = nullptr;

// Signal handler function
void signalHandler(int signum) {
    cout << "\nInterrupt signal (" << signum << ") received.\n";

    // Perform cleanup
    if (db) {
        delete db;
        db = nullptr;
        cout << "Database connection closed.\n";
    }

    // Terminate program
    exit(signum);
}

int main() {
    // Register signal handler
    signal(SIGINT, signalHandler);

    rocksdb::Options options;
    options.create_if_missing = true;
    options.compression = rocksdb::kNoCompression;

    // Open RocksDB instance
    rocksdb::Status status = rocksdb::DB::Open(options, "DB/5M", &db);
    if (!status.ok()) {
        cerr << "Unable to open database: " << status.ToString() << endl;
        return 1;
    }

    int num_keys = 0;

    ifstream jsonFilestream("5M.json");
    json jsonData;
    jsonFilestream >> jsonData;

    // Start measuring time
    auto start = high_resolution_clock::now();

    num_keys += jsonData.size();
    for (auto& [key, value] : jsonData.items()) {
        // Insert key-value pair into the database
        status = db->Put(rocksdb::WriteOptions(), key, value.dump());
        if (!status.ok()) {
            cerr << "Error while writing to database: " << status.ToString()
                 << endl;
            delete db;
            return 1;
        }
    }

    // Stop measuring time
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Successfully wrote " << num_keys << " key-value pairs to RocksDB."
         << endl
         << endl;
    cout << "Time taken: " << duration.count() << " milliseconds" << endl
         << endl;

    // Close the database
    if (db) {
        delete db;
        db = nullptr;
        cout << "Database connection closed.\n";
    }

    cout << "Program terminated gracefully.\n";
    return 0;
}