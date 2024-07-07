# AEDMaps - Project Overview

## Introduction

AEDMaps is the final project of the Algorithms and Data Structures course. There are 4 different variants that will be explained further. The program processes input files with problems and maps, and generates solutions according to the variant chosen.

## Problem Description

The objective is to find paths between locations on a map, adhering to constraints defined by hypothetical travelers. The primary goal is to determine the cheapest path according to a given metric, but additional requirements may be imposed, such as a little detour to a 3rd point in map.

## Compilation

Firstly, let's compile the program. Simply type make on your terminal in the project's file directory.

```sh
make
```

## Program Execution

### Invocation

The AEDMaps program is executed from the command line using the following format:

```sh
./aedmaps [OPTION] [PROBLEMS] [MAPS]
```
- `aedmaps`: The executable name.
- `OPTION`: A string starting with '-' indicating the mode of operation.
- `PROBLEMS`: A `.prbs` file containing problem headers to be solved.
- `MAPS`: A `.maps` file containing maps.

### Options

The program accepts four options:
- `-oo`: Solve the first problem for the first map.
- `-oa`: Solve the first problem for all maps.
- `-ao`: Solve all problems for the first map.
- `-aa`: Solve all problems for all maps.

### Input Files

- **Problems File (`.prbs`)**: Contains headers of problems to be solved.
- **Maps File (`.maps`)**: Contains map data.

### Output Format

The output consists of:
1. A header line indicating the arguments used to invoke the program.
2. Solution headers for each problem, followed by the solution itself, which includes a list of edges that define the path.

Each solution header and edge are formatted specifically according to the problem variant.

## Problem Variants

The AEDMaps program supports four variants of problems:

### Variant A1
Find the shortest path between two vertices.

### Variant B1
Determine the shortest path that between two vertices and passse through a third middle point. If this constraint leads to impossible path, the problem has no solution.

### Variant C1
Find the shortest path avoiding a specified location (unusable vertex).

### Variant D1
Find the shortest path avoiding a specified route (unusable path between two vertices).

## Example

If a user runs the following command:

```sh
./aedmaps -oa some.problems.prbs some.maps.maps
```

The first line of the output file will be:

-oa some.problems.prbs some.maps.maps

Subsequent lines will contain headers and solutions for the problems.

3 additional files will be added to with problems, maps and solution (`.pbrs`, `.maps`, `.routes`) for anyone who wants to test the program. These files were given by the our teachers.

## Conclusion

The AEDMaps project applies algorithms to solve real-world pathfinding problems, demonstrating the practical use of data structures and algorithmic techniques. It is a good introduction to the Dijkstra's algorithm.
