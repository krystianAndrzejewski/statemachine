## Table of contents
* [Description](#description)
* [Requirements](#requirements)
* [Setup](#setup)
* [Usage](#usage)

## Description
This project allows to create a deterministic or non-deterministic finite automaton. Non-deterministic finite automata can be transformed to deterministic finite automata that are optimized for a number of states. Optimization can be also applied to an initial description of state machine. It merges some states but the machine state works identically as before the optimization.
	
## Requirements
* programming language compiler: C++11
* build utils: Cmake

## Setup
To setup the project, use the followings commands (all dependencies are downloaded by Cmake):
```
$ cd <project_dir>
$ git clone https://github.com/krystianAndrzejewski/regular_expression
$ cd regular_expression
cmake -H. -Bbuild -G <generator_name:"Visual Studio 14 2015 Win64">
```

## Usage
An example of the project usage is defined in main.cpp. Non-deterministic finite automata are not useable for state machine application. It is only simplification for users to create state machine description and transform NFA to the proper DFA that can be used based on existing state transitions. 


