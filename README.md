# PLACEHOLDER

Implementation of a buffer manager based database system. The development of the system was carried out over the duration of one year as part of a university project at the [Chair of Applied Computer Science III, University of Mannheim](http://lspi3.informatik.uni-mannheim.de/en/home/). 

## Getting Started

### Prerequisites

To build and run, you will need the Build-Management-Tool _GNU MAKE_ and a _C++17_ compatible compiler (only tested with _GCC 7.3.0_)

### Cloning

```
git clone https://github.com/WeberNick/MasterTeamProjekt.git
```

### Installing

In the git repository directory (_MasterTeamProjekt_) go to directory '_src_' and run _make_:
```
cd src
make
```

### Running (Quickstart guide)
To execute the program, go to the build directory _src_ and type 

```
./main/main
```

or go in the _main_ directory and type

```
./main
```

To actually use the DBS, several command line arguments are needed. To get a list of possible arguements (from the main directory) type

```
./main --help
```

Note: Since this is still under construction, some tests may be disabled or commented out.

## Project Status

### Already implemented

* Partition Manager: Maintains all the different partitions of the database system
  * File Partitions: Using files managed by the filesystem
  * Raw Partitions: Using a raw device without any filesystem on it
* Segment Manager: Maintains all the different segments in the database system
  * Free Space Management (FSM) Segments: Segments with a free space management of their pages
  * Slotted Pages (SP) Segments: Segments with slotted pages
* Buffer Manager: Maintains the buffer pool of the system. On memory pressure, a random eviction policy is applied
* Threading: The system is multi threaded using a thread pool
* Interpreter: Various interpreter are implemented to work with different page layouts
* Tracing: Tracing can be activated by setting a command line argument

### Todo

* Implement higher level functionalities such as a query execution engine
* Implement a lock manager and transaction manager
* Implement index structures
* ...
