# C++ Order Book & Matching Engine

A high-performance C++ simulation of a financial limit order book from a legacy Visual Studio project, now modernized with a cross-platform CMake build system and verified on macOS.

This project implements a thread-safe, single-instrument matching engine that processes market orders according to standard exchange rules, including price-time priority and various order execution policies.

## Key Features

  * **Price-Time Priority Matching:** Orders are correctly prioritized first by best price, then by time of arrival.
  * **Multiple Order Types:** Supports a variety of standard order types:
      * `GoodTillCancel` (GTC)
      * `Market`
      * `FillOrKill` (FOK)
      * `FillAndKill` (Immediate-or-Cancel, IOC)
      * `GoodForDay` (GFD)
  * **High-Performance Design:** Achieves highly efficient operations by using optimal data structures:
      * **O(log P)** for order insertion and cancellation (where P is the number of price levels)
      * **O(1)** for direct order lookup by ID and for finding the best bid/ask.
  * **Partial Fill Logic:** Correctly handles trades where order quantities do not match, leaving the remainder of the larger order on the book.
  * **Thread-Safe:** Uses a `std::mutex` to protect shared data, allowing for safe concurrent operations. Includes a background thread for expiring `GoodForDay` orders.
  * **Verified Correctness:** Logic is rigorously verified by a suite of test cases using the Google Test framework.

## Technology Stack

  * **Language:** C++20
  * **Build System:** CMake
  * **Testing Framework:** Google Test
  * **Core Libraries:** C++ Standard Library (`<map>`, `<unordered_map>`, `<list>`, `<thread>`, etc.)

## Getting Started (Build Instructions for macOS)

Follow these steps to build and run the project on a macOS machine (including Apple Silicon).

### 1\. Prerequisites

  * **Xcode Command Line Tools:** Provides the C++ compiler (Clang). To install, open your terminal and run:
    ```bash
    xcode-select --install
    ```
  * **Homebrew:** A package manager for macOS. If you don't have it, install it from [brew.sh](https://brew.sh).

### 2\. Clone and Install Dependencies

1.  **Clone the repository:**
    ```bash
    git clone [your-repository-url]
    cd [your-repository-name]
    ```
2.  **Install CMake and Google Test using Homebrew:**
    ```bash
    brew install cmake googletest
    ```

### 3\. Build the Project

The project is built using CMake. All commands should be run from the project's root directory.

1.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```
2.  **Configure the project with CMake:**
    ```bash
    cmake ..
    ```
3.  **Compile the code with Make:**
    ```bash
    make
    ```
    This will create two executables inside the `build` directory: `run_tests` and `present`.

## How to Run

All commands must be run from the **project's root directory**, not from inside the `build` directory.

### 1\. Running the Automated Test Suite

To verify the correctness of the entire system, run the `run_tests` executable. This will execute all the pre-defined test scenarios.

```bash
./build/run_tests
```

You should see output from Google Test indicating that all 7 tests have passed.

### 2\. Running the Interactive Demonstration Tool

The `present` executable is designed for demonstrating individual scenarios one at a time. It takes one argument: the path to a scenario file. A collection of demonstration scenarios is located in the `Scenarios` folder.

```bash
# Example: Run the "Partial Fill" demonstration
./build/present Scenarios/2_PartialFill.txt
```

## Scenario Input File Format

The simulation is driven by text files with simple commands.

| Command | Format                                       | Description                               |
| :------ | :------------------------------------------- | :---------------------------------------- |
| `A`     | `A [Side] [OrderType] [Price] [Quantity] [ID]` | **Add** a new order to the book.          |
| `M`     | `M [ID] [Side] [Price] [Quantity]`             | **Modify** an existing order with the given ID. |
| `C`     | `C [ID]`                                     | **Cancel** an existing order with the given ID.   |

  * Lines starting with `#` are ignored as comments.
  * **Example:** `A B GoodTillCancel 100 20 101` adds a Good-Till-Cancel buy order for 20 units at a price of 100 with an ID of 101.

## License

This project is licensed under the MIT License. See the LICENSE.txt file for details.
