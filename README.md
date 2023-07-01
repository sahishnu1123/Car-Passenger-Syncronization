# Car-Passenger-Syncronization

This project was done as part of course **CS-3523** which was organized by **Dr. Sathya Peri** which aims to develop a **Synchronisation program using Semaphores**

## About the project

In this project I have used **Semaphores** to implement Synchronisation of the classic **Jurassic Park problem** in **Galvin Textbook** 

### Overview of the question

We need to syncronize 'n' passengers each willing to ride k times and 'm' cars that can only carry one person at a time. Semaphores have been used to ensure syncronization between the passengers and the cars. Passengers wait for a car to be available, while the cars wait for a person to take on a ride. The project aims to understand what happens when the number of passengers and the number of cars are varied. Graphs have also been plotted for the same.


## Authors

- [@sahishnu1123](https://www.github.com/sahishnu1123)


## Tech Stack

**Command Line:** C++
## Prerequisite

Check if a suitable compiler exists for compiling the C++ code

for g++ compiler:

    g++ --version

for clang compiler:

    clang --version

If there isn't any compiler installed, install it using the documentation for the compiler of C++ based on your OS.

## Install and Usage

### Installing project

Clone from repo:

    git clone https://github.com/sahishnu1123/Car-Passenger-Syncronization/

### Running the Program

After cloning is done redirect control to project folder using:

    cd ./Car-Passenger-Syncronization

For compiling the program run:

    g++ -std=c++20 -o CPsync.out Car-Passenger.cpp -lpthread

For executing the program:

    ./CPsync.out	
    
### Input to the Program

Input to the program is read from the file **input.txt**. The input should be given in the format:

P C &lambda;(P) &lambda;(C) K

- **P:** Number of passenger threads
- **C:** Number of car threads
- **&lambda;(P):** Parameter for wandering time of a passenger between successive rides
- **&lambda;(C):** Parameter for travelling time of a car
- **K:** Total number of rides a passenger must ride for the passenger to exit the park

### Output of the Program 

The output of the program is printed to **output.txt** file. Logs for all the rides are written to this file for each ride taken.



## Documentation

The explanation and report for the whole project can be found in **Report.pdf**
