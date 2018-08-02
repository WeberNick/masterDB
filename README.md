# masterDB

Implementation of a buffer manager based database system. The development of the system was carried out over the duration of one year as part of a university project at the [Chair of Applied Computer Science III, University of Mannheim](http://lspi3.informatik.uni-mannheim.de/en/home/). 

Table of Contents
=================

  * [Prerequisites](#prerequisites)
     * [System Requirements](#system-requirements)
  * [Project Structure](#project-structure)
     * [src/buffer](#buffer)
     * [src/cli](#cli)
     * [src/infra](#infra)
     * [src/interpreter](#interpreter)
     * [src/main](#main)
     * [src/partition](#partition)
     * [src/segment](#segment)
     * [src/threading](#threading)
  * [Getting Started](#getting-started)
     * [Quick Start Guide](#quick-start-guide)
  * [Project Status](#project-status)

## Prerequisites
There are several tools you will need to install and execute the application. In the following is a list with all required tools and technologies required for installing and running the system:
 * _UNIX-like_ operating system: All development and testing was carried out on UNIX-like operating systems. As various _UNIX_ system calls are used throughout the program, the system will not run under _Windows_. 
 * [GNU Make](https://www.gnu.org/software/make/): As development and testing was carried out on UNIX-like operating systems, the de-facto standard build and install system _GNU Make_ is used.
 * [_C++17_ compatible compiler](https://gcc.gnu.org): As _C++17_ features are frequently used and all tests were carried out with _GCC 7.3.0_, we recommend to use this compiler or a never version
 * _Intel Processor_: As we use various [_Intel intrinsics_](https://software.intel.com/sites/landingpage/IntrinsicsGuide/), the system is currently only able to run on Intel processors
 * _Little Endian Architecture_

## Project Structure

### buffer
Implementation of the buffer pool
For further informations, take a look at [src/buffer/README.md](/src/buffer/README.md)
### cli
Implementation of a command line interface. By executing queries via the command line interface, the system can be controlled.
For further informations, take a look at [src/cli/README.md](/src/cli/README.md)
### infra
Various helper files and functionalities.
For further informations, take a look at [src/infra/README.md](/src/infra/README.md)
### interpreter
Implementation of various low level page interpreter working on the bits and bytes of pages.
For further informations, take a look at [src/interpreter/README.md](/src/interpreter/README.md)
### main
Implementation of the database instance manager and main-method.
For further informations, take a look at [src/main/README.md](/src/main/README.md)
### partition
Implementation of raw partitions (partitions for raw/block devices), file partitions (working on files in the file system) and the partition manager (managing all partitions).
For further informations, take a look at [src/partition/README.md](/src/partition/README.md)
### segment
Implementation of free space management segments (segments with a free space management of pages) and segments for the N-ary storage model (NSM)
For further informations, take a look at [src/segment/README.md](/src/segment/README.md)
### threading
Implementation of the threadpool.
For further informations, take a look at [src/threading/README.md](/src/threading/README.md)

## Getting Started
The build and installation process will be described in the following. Follow the [Quick Start Guide](#quick-start-guide) for a fast installation and get the system running.

### Quick Start Guide

 * Make sure all the [Requirements](#prerequisites) are satisfied

 * Clone the source with `git` :
```
git clone https://github.com/WeberNick/masterDB.git
```

 * Build and install:
```
cd masterDB/src
make
```

 * Run
```
./main/main
```

 * Getting Help
```
./main/main --help
```

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
* Command Line Interpreter: The system is controlled via a command line interface

### Todo

* Implement higher level functionalities such as a query execution engine
* Implement a lock manager and transaction manager
* Implement index structures
* ...
