# Safari Rush Hour Solver

An optimized solver, verification tool, and benchmark suite for the sliding-block puzzle game **Safari Rush Hour**, implemented in C++17. The project features multiple search algorithms (A\*, IDA\*, BFS, Beam Search, RBFS), multi-tiered heuristic families (admissible obstacle-counting bitmasks, pattern database relaxations, and dynamic raycasting), 64-bit bitboard state representation, dual Zobrist hashing, and OpenMP multi-threading.

---

## Table of Contents

- [About Safari Rush Hour](#about-safari-rush-hour)
  - [Game Rules and Pieces](#game-rules-and-pieces)
  - [Board Layout and Exit](#board-layout-and-exit)
- [Algorithms and Techniques](#algorithms-and-techniques)
  - [Search Algorithms](#search-algorithms)
  - [Heuristics](#heuristics)
  - [Optimization Techniques](#optimization-techniques)
- [Building the Project](#building-the-project)
  - [Prerequisites (Linux / WSL)](#prerequisites-linux--wsl)
  - [Compilation Commands](#compilation-commands)
- [Executables and Usage](#executables-and-usage)
  - [1. `safari` (Main Solver)](#1-safari-main-solver)
  - [2. `test` (Automated Test Suite and Benchmark Runner)](#2-test-automated-test-suite-and-benchmark-runner)
  - [3. `verify` (Solution Validator)](#3-verify-solution-validator)
- [Pattern Database (PDB) / Relaxed Heuristic Files](#pattern-database-pdb--relaxed-heuristic-files)
- [Puzzle Datasets](#puzzle-datasets)
- [Disclaimer](#disclaimer)

---

## About Safari Rush Hour

**Safari Rush Hour** is an expansion of the classic *Rush Hour* sliding-block puzzle designed by ThinkFun. While traditional Rush Hour is played on a 6x6 grid with 1x2 and 1x3 cars and trucks that only slide along their primary axis, Safari Rush Hour introduces a larger **7x7 grid** and distinct multi-directional pieces.

### Game Rules and Pieces

1. **The Safari Rover (`x`)**:
   - A **2x2** square piece representing the player's safari vehicle.
   - Unlike conventional Rush Hour cars, the 2x2 Rover can slide **in all four orthogonal directions** (Up, Down, Left, Right).
2. **Large Animal Obstacles (`u`, `v`)**:
   - **2x2** square pieces (e.g., Rhinos, Elephants).
   - Can also move **in all four orthogonal directions** (Up, Down, Left, Right).
3. **Three-Cell Animals (`o`, `q`, `s`, `p`, `r`)**:
   - **3x1** rectangular pieces (e.g., Cheetahs, Lions).
   - Constrained to slide **only along their fixed orientation**:
     - Horizontal pieces move Left and Right.
     - Vertical pieces move Up and Down.
4. **Two-Cell Animals (`a`, `f`, `d`, `g`, `j`, `b`, `h`, `i`, `k`, `c`, `e`)**:
   - **2x1** rectangular pieces (e.g., Zebras, Impalas, Gazelles).
   - Constrained to slide **only along their fixed orientation** (Left/Right if horizontal, Up/Down if vertical).
5. **Movement Rules**:
   - Pieces slide into adjacent unoccupied cells without rotating or jumping over other pieces.
   - A single move can slide a piece by one or more squares in a single direction.

### Board Layout and Exit

The board is a 7x7 grid (rows 0 to 6, columns 0 to 6). The escape gate is located on the **right edge at rows 2 and 3** (column 7):

```text
       Columns:
       0  1  2  3  4  5  6
Row 0 [.  .  .  .  .  .  .]
Row 1 [.  .  .  .  .  .  .]
Row 2 [.  .  .  .  .  .  .] ===> EXIT GATE (Rows 2-3, Column 7)
Row 3 [.  .  .  .  .  .  .] ===>
Row 4 [.  .  .  .  .  .  .]
Row 5 [.  .  .  .  .  .  .]
Row 6 [.  .  .  .  .  .  .]
```

**Goal State**: The puzzle is solved when the 2x2 Rover (`x`) reaches the exit gate such that its upper-left corner is at `row 2, column 7` (the rover has cleared the board through rows 2 and 3).

---

## Algorithms and Techniques

### Search Algorithms

- **`IDAstar` (Iterative Deepening A\*) [Default]**: Depth-first iterative deepening guided by the evaluation function $f(n) = g(n) + h(n)$, combined with transposition table caching. Ideal for optimal solutions in memory-constrained environments.
- **`Astar` (A\*)**: Classic best-first search maintaining explicit open and closed lists, optimal with admissible heuristics.
- **`BFS` (Breadth-First Search)**: Systematic level-by-level search used to explore state-space structures and verify unguided solution depths.
- **`BeamSearch`**: Heuristic search retaining only the best $B$ candidates (controlled by `--beam`) at each level. Sacrifices optimality for extreme speed on hard instances.
- **`RBFS` (Recursive Best-First Search)**: Linear-space best-first search that tracks an alternative cost upper bound to backtrack when current paths exceed expectations.

### Heuristics

- **`zero`**: Uninformed baseline ($h = 0$, equivalent to uniform-cost search / Dijkstra).
- **`twoonezero`**: Basic Manhattan-style distance for the 2x2 Rover to align with and reach row 2.
- **`obstacles0` [Default]**: Checks the direct escape row to count distinct obstacles blocking the Rover's direct rightward path to the exit.
- **`obstacles1` to `obstacles4`**: Progressively deeper precomputed path bitmasks covering multi-step escape routes (up to 5-step maneuvers around obstacles) to find the minimum blocker count across candidate egress paths.
- **`relaxed1`, `relaxed2`, `relaxed3`**: Pattern Database (PDB) relaxation heuristics. These precompute exact optimal solution lengths for abstract board states consisting solely of the Rover plus 1, 2, or 3 other pieces, looked up from binary tables (`dataOne.bin`, `dataTwo.bin`, `dataThree.bin`).
- **`combo`**: Returns $\max(\text{obstacles4}, \text{relaxed2})$, leveraging both deep exit corridor analysis and multi-piece relaxation databases.
- **`raycast`**: A dynamic bottleneck-oriented heuristic that rapidly builds a 1D grid representation, identifies blocking pieces along the escape corridor, and calculates recursive clearance dependencies.

### Optimization Techniques

- **64-bit Bitboard Representation**: The 7x7 board is embedded into a 63-bit bitboard (7 rows $\times$ 9 bits per row, including edge sentinels). Intersections, collisions, and path clearances are computed with single-instruction bitwise operations (`&`, `|`, `^`, bit-shifts).
- **Dual Zobrist Hashing**: Two independent 64-bit pseudorandom hashes are maintained incrementally as pieces move, virtually eliminating hash collisions in transposition tables.
- **Transposition Tables (TT)**: Fast fixed-size transposition tables with replacement schemes to prune revisited search states. Also supports `std::unordered_map` via `--typett map`.
- **Solution & Component Caching**:
  - `--cluster`: Caches solutions across boards belonging to the same cluster.
  - `--ccomp`: Decomposes independent board partitions into connected components.
- **Parallel Execution**: OpenMP support enables multi-threaded test runs and batch benchmarking.

---

## Building the Project

### Prerequisites (Linux / WSL)

Ensure you have a C++17-capable GCC compiler, Make, OpenMP, and the Boost Program Options development library installed.

On Ubuntu or Debian (including WSL):

```bash
sudo apt update
sudo apt install -y build-essential libboost-program-options-dev
```

### Compilation Commands

Build all targets using `make`:

```bash
# Build all three executables (safari, verify, test)
make

# Or compile individual targets:
make safari   # Main puzzle solver
make verify   # Solution validator
make test     # Test suite & benchmark runner

# Build static library archive (safari.a):
make lib

# Clean all build artifacts:
make clean
```

The Makefile compiles with `-O3 -msse -msse2 -Wall -Wextra -std=c++17 -fopenmp -lboost_program_options`.

---

## Executables and Usage

### 1. `safari` (Main Solver)

The `safari` executable solves puzzles provided via standard input (`stdin`).

#### Command-Line Options (from `--help`)

```text
Allowed options:
  --help                               prints this help message
  -t [ --time ] arg (=60)              set timeout (in seconds)
  -h [ --heuristic ] arg (=obstacles0) set heuristic
  -a [ --algorithm ] arg (=IDAstar)    set algorithm
  -s [ --size ] arg (=10000000)        set TT size (in #entries, approximately 
                                       16 bytes per node)
  -l [ --length ] arg (=1000)          set maximum solution length to look for 
                                       in IDA*
  -g [ --generate-relaxed ] arg (=0)   generate relaxed heuristics (1: dataOne,
                                       2: dataOne & dataTwo)
  -b [ --beam ] arg (=100)             set beam width for BeamSearch
  -c [ --cluster ]                     Enable caching of solutions between 
                                       boards, for reuse in the same cluster
  --typett [=arg(=stdin)] (=normal)    set transposition table type (map = 
                                       std::unordered_map, normal = array)
  --ccomp                              Enable connected components
```

#### Input Format

Puzzles are fed through standard input. Each puzzle starts with a header line beginning with `P <name>`, followed by 7 lines of 7 characters representing the grid:
- `.` represents an empty square.
- `x` represents the 2x2 Safari Rover.
- Letters `u`, `v`, `o`, `q`, `s`, `p`, `r`, `a`, `f`, `d`, `g`, `j`, `b`, `h`, `i`, `k`, `c`, `e` denote individual animal pieces.

Example puzzle file (`J1`):
```text
P J1
.......
.rrrf..
hh..fq.
o.xx.q.
o.xx.q.
ob..ee.
.bppp..
```

#### Output Format

When a solution is found, `safari` prints the search statistics and the move sequence:
```text
statistics J1 Nodes: 102, average H: 2.65508, cut-off depth (min/average/max): 0/1.90119/4
solution J1 0.00185 5  fu1 xu1 el1 qd2 xr5
```

Each move in the solution specifies:
- Piece letter (e.g., `x`, `f`, `e`)
- Direction (`u` = up, `d` = down, `l` = left, `r` = right)
- Distance (number of cells to slide)

#### Usage Examples

```bash
# Solve J1 with default settings (IDA* and obstacles0 heuristic):
./safari < J1

# Solve J1 using A* with the raycast heuristic:
./safari -a Astar -h raycast < J1

# Solve J1 using Beam Search with a beam width of 50:
./safari -a BeamSearch -b 50 < J1

# Solve all puzzles in the Junior set with a 10-second timeout:
./safari -t 10 -h combo < Jpuzzles
```

---

### 2. `test` (Automated Test Suite and Benchmark Runner)

The `test` utility runs automated verification and benchmarks across puzzle sets. It validates that the generated solutions match known optimal lengths and ensures moves do not cause piece collisions.

#### Syntax

```bash
./test [options] [test_set]
```

#### Supported Options

- `test_set`: Name of a puzzle set or individual puzzle ID (default: `J1`):
  - Sets: `J` (Junior), `B` (Beginner), `I` (Intermediate), `A` (Advanced), `E` (Expert), `all` (All 40 puzzles).
  - Specific puzzles: e.g., `J1`, `J3`, `B9`, `A21`, `E40`.
- `--BFS`: Enables BFS testing alongside default algorithms.
- `--BeamSearch`: Enables Beam Search testing.
- `--RBFS`: Enables Recursive Best-First Search testing.
- `--quiet`: Minimal output mode; only prints summary test statuses.
- `--eval-heuristics`: Benchmarks and evaluates all available heuristics against optimal lengths.
- `-t <num>`, `--threads <num>`: Sets the number of OpenMP worker threads for parallel evaluation.
- `-f <file>`: Specifies a custom puzzle file to load.
- `-s <file>`: Specifies a custom solution file to compare against.

#### Usage Examples

```bash
# Run tests on the default puzzle (J1):
./test J1

# Run tests on all Junior puzzles (Jpuzzles):
./test J

# Benchmark all heuristics across all puzzles using 4 threads in quiet mode:
./test -t 4 --eval-heuristics --quiet all

# Test using a custom puzzle file and custom reference solution file:
./test -f custom_puzzles.txt -s custom_solutions.txt
```

---

### 3. `verify` (Solution Validator)

The `verify` executable independently checks solutions against puzzles step-by-step to confirm that:
1. Every move is physically legal according to the piece type and its orientation.
2. No piece overlaps or leaves the board boundaries.
3. The final configuration achieves the goal state (Rover at `row 2, column 7`).

#### Input Protocol

`verify` reads directly from standard input in the following format:
1. `P <name>`
2. 7 lines of the 7x7 board state
3. An empty line separator
4. Solution line: `solution <name> <time> <num_moves> <move1> <move2> ... <moveN>`

Outputs each intermediate board state and concludes with `OK!` if valid, or `BAD!` / `Formatting problem` if invalid.

#### Usage Examples

```bash
# Verify an existing solution file:
(cat puzzleLargo; echo ""; cat puzzleLargoSolution) | ./verify

# Pipe solver output directly into verify for end-to-end validation:
(cat J1; echo ""; ./safari < J1 | grep '^solution') | ./verify
```

---

## Pattern Database (PDB) / Relaxed Heuristic Files

The relaxed heuristics precompute optimal solutions for simplified abstractions of the game board:

- **`dataOne.bin`** (~14 KB): Precomputed 1-piece relaxation table (Rover + 1 animal piece).
- **`dataTwo.bin`** (~5.5 MB): Precomputed 2-piece relaxation table (Rover + 2 animal pieces).
- **`dataThree.bin`** (~2.1 GB): Precomputed 3-piece relaxation table (Rover + 3 animal pieces).
  - Stored in the repository as `dataThree.bin.bak`.
  - To enable the `relaxed3` heuristic in `safari` and `test`, simply rename or copy the file:
    ```bash
    cp dataThree.bin.bak dataThree.bin
    ```

### Generating Relaxed Databases

You can regenerate `dataOne.bin` and `dataTwo.bin` using `safari`:

```bash
# Generate dataOne.bin:
./safari -g 1

# Generate both dataOne.bin and dataTwo.bin:
./safari -g 2
```

---

## Puzzle Datasets

The repository includes standard benchmark puzzles organized by difficulty:

| Difficulty Level | Puzzle File | Reference Solution File |
| :--- | :--- | :--- |
| **Junior** | `Jpuzzles` | `JSolutions` |
| **Beginner** | `Bpuzzles` | `BSolutions` |
| **Intermediate** | `Ipuzzles` | `ISolutions` |
| **Advanced** | `Apuzzles` | `ASolutions` |
| **Expert** | `Epuzzles` | `ESolutions` |
| **Complete Suite** | `allPuzzles` | `allSolutions` |

Individual sample and test puzzles are also provided (e.g., `J1`, `J2`, `J3`, `B1`, `B9`, `I11`, `I13`, `A21`, `E31`, `E40`, `puzzleLargo`, `NoSolutionPuzzle`).

---

## Disclaimer

This README was generated with Gemini 3.8 Flash.