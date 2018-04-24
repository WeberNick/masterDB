# PLACEHOLDER

Implementation of a buffer manager based database system. The development of the system was carried out over the duration of one year as part of a university project at the [Chair of Applied Computer Science III, University of Mannheim](http://lspi3.informatik.uni-mannheim.de/en/home/). 

## Getting Started

### Prerequisites

To build and run, you will need the Build-Management-Tool _GNU MAKE_ and a _C++17_ compatible compiler (only tested with _GCC 7.3.0_)

### Clone

```
git clone https://github.com/WeberNick/MasterTeamProjekt.git
```

### Build

In the git repository directory (_MasterTeamProjekt_) go to directory '_src_' and run _make_:
```
cd src
make
```

### Install (Quickstart guide)
After building the executable, the system can be used. Several command line arguments are needed to start the system. To get a list of possible arguments (from the main directory) type  

```
./main --help
```


When starting the system for the first time, it must create some files for internal usage and to store session information. Therefore, run the following command when starting the system for the first time:

```
./main --install --master /path/to/directory/
```

Note: The flag _--master_ expects a parameter which must be a valid path in the file system. Pay attention that the trailing backslash is included in the path.

..todo..

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
