
#include <rocksdb/db.h>
#include <rocksdb/filter_policy.h>

#include <chrono>  // For timing
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>  // For smart pointer management
#include <string>

using namespace std;
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

// Scope guard for automatic cleanup
class DBGuard {
   public:
    ~DBGuard() {
        if (db) {
            delete db;
            db = nullptr;
            cout << "Database connection closed by scope guard.\n";
        }
    }
};

int main() {
    // Register signal handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // Ensure database is closed if main exits unexpectedly
    DBGuard dbGuard;

    // Open RocksDB database
    rocksdb::Options options;
    options.create_if_missing = true;

    // Enable Bloom filter policy
    // const rocksdb::FilterPolicy* filter_policy =
    //     rocksdb::NewBloomFilterPolicy(10);
    // options.filter_policy = filter_policy;
    options.compression = rocksdb::kNoCompression;

    rocksdb::Status status = rocksdb::DB::Open(options, "DB/50M", &db);
    if (!status.ok()) {
        cerr << "Unable to open database: " << status.ToString() << endl;
        return 1;
    }

    string value;
    string key = "";

    cout << "Enter \"QUIT\" to end the search!" << endl << endl;

    while (true) {
        cout << "Enter a key : ";
        cin >> key;
        if (key == "QUIT") {
            break;
        }

        // Start measuring time for each Get operation
        auto start = high_resolution_clock::now();

        status = db->Get(rocksdb::ReadOptions(), key, &value);

        // Stop measuring time
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        if (!status.ok()) {
            cerr << "Key does not exist: " << status.ToString() << endl << endl;
            cout << "Time taken: " << duration.count() << " microseconds"
                 << endl
                 << endl;
        } else {
            cout << "Value for key: " << key << " is: " << value << endl;
            cout << "Time taken: " << duration.count() << " microseconds"
                 << endl
                 << endl;
        }
    }

    // Close the database explicitly
    if (db) {
        delete db;
        db = nullptr;
        cout << "Database connection closed.\n";
    }

    // Clean up filter policy
    // delete filter_policy;

    cout << "Program terminated gracefully.\n";
    return 0;
}

apps - fileview.texmex_20241121 .01_p0 GET.cpp Displaying GET.cpp.