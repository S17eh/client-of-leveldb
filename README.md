# client-of-leveldb

This project presents a detailed analysis of LevelDB, a fast and lightweight key-value storage library developed by Google. Here, we implement a client application that utilises LevelDB by incorporating its header files and implementing the PUT and GET functions to insert key-value pairs and retrieve values based on specific keys.

## Steps to Complie leveldb:

### Cloning the leveldb repo:
git clone --recurse-submodules https://github.com/google/leveldb.git

### For cmake:
```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```

## For compiling a cpp file which uses levelDB:
While compiling, we need to link to this static library libleveldb.a using -lleveldb
```
g++-11 <file_name> -I<path to leveldb/include> -L<path to leveldb/build> -lleveldb
```
More detials can be found in the report of this study.

Find our in depth report on the basic idea, structure and internal working of LSM tree here : https://shorturl.at/utli2

Source code of LevelDB : https://github.com/google/leveldb
