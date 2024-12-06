#include <wiredtiger.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem> // For handling file system operations
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

int
main()
{
    // Directory path for WiredTiger storage
    string wt_dir = "/tmp/WT_BTree";

    // Check if the directory exists, if not, create it
    if (!fs::exists(wt_dir)) {
        if (!fs::create_directories(wt_dir)) {
            cerr << "Error creating directory: " << wt_dir << endl;
            return -1;
        }
        cout << "Directory created: " << wt_dir << endl;
    } else {
        cout << "Directory exists: " << wt_dir << endl;
    }

    // Open the JSON file
    ifstream jsonFile("17_5M.json");
    if (!jsonFile.is_open()) {
        cerr << "Error opening JSON file" << endl;
        return -1;
    }

    // Parse the JSON data
    json jsonData;
    jsonFile >> jsonData;

    // WiredTiger Setup
    WT_CONNECTION *conn;
    WT_SESSION *session;
    WT_CURSOR *cursor;
    int ret;

    // Open a WiredTiger connection in the directory.
    ret = wiredtiger_open(wt_dir.c_str(), NULL, "create", &conn);
    if (ret != 0) {
        cerr << "Error opening WiredTiger" << endl;
        return ret;
    }

    // Open a session handle for the connection.
    conn->open_session(conn, NULL, NULL, &session);

    // Create a table using LSM (Log-Structured Merge) tree storage
    session->create(session, "table:mytable", "key_format=S,value_format=S,type=file");
    // session->create(session, "table:mytable", "key_format=S,value_format=S,type=lsm");

    // Open a cursor on the table.
    session->open_cursor(session, "table:mytable", NULL, NULL, &cursor);

    cout << "Testing on B+Tree Set up!" << endl;

    // Insertion process with timing in microseconds.
    auto start = chrono::high_resolution_clock::now();
    int numInserted = 0;

    for (auto &element : jsonData.items()) {
        string key = element.key();
        string value = element.value();

        cursor->set_key(cursor, key.c_str());
        cursor->set_value(cursor, value.c_str());
        cursor->insert(cursor);
        ++numInserted; // Increment the count of inserted key-value pairs
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Output time and number of inserted key-value pairs
    cout << "Inserted " << numInserted << " key-value pairs." << endl;
    cout << "Time taken to insert key-value pairs: " << duration.count() << " microseconds" << endl;

    // Close everything
    cursor->close(cursor);
    session->close(session, NULL);
    conn->close(conn, NULL);

    return 0;
}
