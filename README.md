# How to build
This project consists of 3 different applications: encoder, decoder and visualizer, as so, each of them are handled in their own folder.
We are using CMake for making the applications, so in order to build them you have to follow the next steps
1. Clone the repository
2. cd to your desired application to be built
3. Run:
```
mkdir build
cd build
cmake ..
make 
```

After this, you will have a working project if no error was found.

# How to test
We are using cmocka in order to test some of the functionalities of the project, in order to run the tests you will have to follow these steps:
1. Clone the repository recursively
2. cd to test/ directory
3. Build with cmake:
```
mkdir build
cd build
cmake ..
make
```
