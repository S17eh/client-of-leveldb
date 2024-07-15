#include <chrono>  // For timing
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <nlohmann/json.hpp>
#include <random>
#include <string>

using namespace std;
using json = nlohmann::json;
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

  leveldb::Options options;
  options.create_if_missing = true;
  options.write_buffer_size = 2 * 1024 * 1024;
  options.block_size = 1024;
  options.compression = leveldb::CompressionType::kSnappyCompression;

  // Enable Bloom filter policy
  // const leveldb::FilterPolicy* filter_policy =
  //     leveldb::NewBloomFilterPolicy(10);
  // options.filter_policy = filter_policy;

  leveldb::Status status = leveldb::DB::Open(options, "/tmp/leveldb", &db);
  if (!status.ok()) {
    cerr << "Unable to open database: " << status.ToString() << endl;
    return 1;
  }

  // Read JSON files
  vector<string> jsonFiles = {"data1.json", "data2.json"};
  int num_keys = 0;

  // Start measuring time
  auto start = high_resolution_clock::now();

  for (const auto& jsonFile : jsonFiles) {
    ifstream jsonFilestream(jsonFile);
    json jsonData;
    jsonFilestream >> jsonData;

    num_keys += jsonData.size();
    for (auto& [key, value] : jsonData.items()) {
      // Insert key-value pair into the database
      status = db->Put(leveldb::WriteOptions(), key, (leveldb::Slice)value);
      if (!status.ok()) {
        cerr << "Error while writing to database: " << status.ToString()
             << endl;
        delete db;
        return 1;
      }
    }
  }

  // Stop measuring time
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);

  cout << "Successfully wrote " << num_keys << " key-value pairs to LevelDB."
       << endl
       << endl;
  cout << "Time taken: " << duration.count() << " microseconds" << endl << endl;

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
  // delete filter_policy;

  cout << "Program terminated gracefully.\n";
  return 0;
}
