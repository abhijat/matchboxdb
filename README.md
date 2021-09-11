An experiment to create a database system from scratch, using minimum third party dependencies.

### SQL support

The following SQL constructs are loosely supported:

1. Create table
2. Insert rows
3. Update rows
4. Delete rows
5. Drop tables

### Supported column types

The following data types are supported for columns:

1. String
2. Unsigned int
3. boolean

### Operating model

The current execution model starts up by scanning the current working directory for our preferred table file extension (".mbx")
and registering each file as a table. The tables contain metadata for the schema that each tuple follows and the data itself is
divided into pages.

Each SQL statement is parsed and sent to a command executor which applies the action via a page cache. The page cache is an LRU
cache which keeps pages in memory to minimize disk reads.

### Dependencies

The following dependencies are used:

1. [linenoise](https://github.com/antirez/linenoise) for the repl
2. [googletest](https://github.com/google/googletest) for unit testing
