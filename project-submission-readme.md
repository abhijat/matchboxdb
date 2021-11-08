## Overview

This project implements a full-featured database system. It contains the following features:

1. persistent storage to disk using a page based table structure
2. a custom SQL parser with support for common SQL statements
3. Introspection to view table schema
4. Three column types: string, int and boolean
5. autocomplete based on tab for SQL constructs (this is provided by third party)

No third party libraries are used for any of these features unless noted otherwise. The functionality is all written from scratch.

### Project structure

The following code structure is high level, it does not go into individual classes but tries to describe code units. 
It rougly corresponds to file tree:

* src: contains code
  * actions: contains database command action objects. each object wraps and performs a db action
  * dependencies: third party embedded libs, currently only linenoise
  * page: all code related to page abstractions
    * defragger: compacts data pages which have deleted entries to save space
    * metadata page: stores meta info about a table, one metadata page is maintained per table. it has table name, schema etc
    * page: the parent class for metadata and slotted_data page, provides common functionalities
    * page_cache: the central manager class to avoid disk io. all page loading happens from here. it is an LRU cache which caches pages read from disk and writes them back to disk. other areas of code use pointers handed out from here
    * page creator, scanner and visitors: utilities to make working with pages easier
    * slotted data page: the core of the storage, contains tuples of data and provides api to read and write these
  * sql_parser: a full sql parser which implements Pratt Parsing
    * node: the parent class for all parser constructs
    * statements: various sql statements which contain elements such as table name, clauses etc
    * parser: the main parser class, implements pratt parsing, maintains token and state, propagates errors
    * token: contains the low level token and constants that parser needs
  * storage: code for managing low level file I/O, table creation etc.
  * others: errors, logging, the repl and command executor
* tests: contains test cases

### Third party libraries

Two third party libraries are used

1. linenoise: A repl support library to provide autocomplete based on tab and history of commands
2. googletest: for the unit tests I have written

linenoise is embedded in code itself (src/dependencies) and googletest is downloaded by CMake.

### System Requirements

1. a sufficiently modern C++ compiler
2. CMake >= 3.16
3. Internet connection for cmake to download test libraries

### Instructions for building the project

This project uses C++20 features. It uses CMake as the build tool. For running test cases googletest is used.

The CMakeLists.txt file contains directives to download the googletest source, so an internet connection is needed.

The following commands should suffice to build the project in the project root: 

```shell
mkdir build && cd build && cmake .. && make
```

### Instructions for running the project

In the build dir run 

```shell
./repl
```

The shell which is started supports command history with the arrow keys. Tab completion is also supported for SQL keywords.

The SQL support is case sensitive. Semicolon to end statements is optional. Newline sends the statement to the db server.


### Rubrics satisfied:

* The project demonstrates an understanding of C++ functions and control structures.
  * control structures are used throughout the code base
* The project reads data from a file and process the data, or the program writes data to a file.
  * the storage system reads from and to binary files. Each table that the user creates is saved as binary file split into pages. 
  * IO is done by using a system of offsets into pages. 
* The project accepts user input and processes the input.
  * a repl is provided to read user input, parse it and execute the parsed commands on the db page cache
* The project uses Object Oriented Programming techniques
  * in multiple places OOP techniques are used, for example the sql parser uses a top level node which is inherited into various structures such as statements, identifiers and fields
  * pages are divided into two types, a metadata page and a slotted data page. they inherit from a common page class.
* Classes use appropriate access specifiers for class members.
  * all classes expose public methods and use protected access modifier for elements which should be read by child classes
* Class constructors utilize member initialization lists.
  * all constructors use the list based initialization
* Classes abstract implementation details from their interfaces.
  * the parent classes expose pure virtual methods for which concrete behavior is supplied by derived classes. Eg see the node class
  * visitor pattern is used for traversing class hierarchy for evaluating nodes, printing them to json etc
* Templates generalize functions in the project.
  * templates are used in multiple places where appropriate. in particular `logging.h` uses variading templates to enable a very flexible logging interface
* The project makes use of references in function declarations.
  * references are used in most places in code. as concrete example src/command_executor.cpp takes references to commands to be run.
* The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate.
  * in several places scoped lock guards are used to protect access which automatically release access once they go out of scope, eg see src/logging.h:27
* The project uses move semantics to move data, instead of copying it, where possible.
  * move semantics are used whereever possible. as concrete example src/page/page.cpp:95 - page stream and buffer are moved out in the move constructor.
* The project uses smart pointers instead of raw pointers.
  * the parser and page use unique_ptr everywhere for polymorphic implementation.
* The project uses multithreading.
  * a separate thread is started on repl launch for page defragmentation, this thread runs in background and compacts data pages from which a tuple has been deleted: src/page/defragger.[h/cpp]
  * it uses lock free versioning system to avoid blocking for long periods on expensive operation: src/page/defragger.cpp:30
* A mutex or lock is used in the project.
  * each data page has a mutex which is locked using scoped guard before changing the page: src/page/slotted_data_page.cpp:181 - scoped guards are also used for logging



### Usage hints

The following concrete sql statements are supported, with some examples given, these can be typed at the repl:

```sql
CREATE TABLE employees (name STRING, age UNSIGNED_INT, is_animal BOOLEAN)
```

creates a table named automobiles. The data types supported are STRING, UNSIGNED_INT, BOOLEAN.

this command will create a file in local dir called automobiles.mbx which will store the data once the repl is shut down.

```sql
INSERT INTO employee VALUES ("cujo", 7, true)
```

inserts row (tuple) into the table, also stores it into the table file.

```sql
SELECT name, age FROM employee

SELECT name, age FROM employee WHERE age <10 AND is_animal = true

SELECT age, name FROM employee

SELECT age, name, is_animal FROM employee
```

various combinations of select statements. will select rows from table if found. also note where
clauses which can be used (must evaluate to boolean). Where clauses can be combined freely

```sql
DESCRIBE TABLE employee
```

prints a description of the table schema

```sql
UPDATE employee SET name = "linoleum floors", age = 0 WHERE age > 5
```

updates the table, internally it removes the old rows and adds new rows. in future I may optimize this to do in place updates

```sql
DELETE FROM employee WHERE age > 5
```

deletes rows from table. it actually just marks tuples as deleted and later the defragmenter will remove them,
but deleted rows will not show up in queries any more

```sql
DROP TABLE automobiles
```

drops the table and deletes the file for table as well.


For further usage hints please see test cases.

