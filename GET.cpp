#include <chrono>  // For timing
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <string>

using namespace std;
using namespace std::chrono;

// Global pointer to the database instance
leveldb::DB* db = nullptr;

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

  // Open LevelDB database
  leveldb::Options options;
  options.create_if_missing = true;
  // Enable Bloom filter policy
  const leveldb::FilterPolicy* filter_policy =
      leveldb::NewBloomFilterPolicy(10);
  options.filter_policy = filter_policy;
  options.compression = leveldb::CompressionType::kNoCompression;

  leveldb::Status status = leveldb::DB::Open(options, "/tmp/leveldb", &db);
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

    status = db->Get(leveldb::ReadOptions(), key, &value);

    // Stop measuring time
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    if (!status.ok()) {
      cerr << "Key does not exist: " << status.ToString() << endl << endl;
      cout << "Time taken: " << duration.count() << " microseconds" << endl
           << endl;

    } else {
      cout << "Value for key: " << key << " is: " << value << endl;
      cout << "Time taken: " << duration.count() << " microseconds" << endl
           << endl;
    }
  }

  // Close the database
  if (db) {
    delete db;
    db = nullptr;
    cout << "Database connection closed.\n";
  }

  // Clean up filter policy
  delete filter_policy;

  cout << "Program terminated gracefully.\n";
  return 0;
}
