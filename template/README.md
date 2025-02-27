# CN CA1 - Bomberman Game with Socket Programming in QT

Welcome to the CN CA1 repository! This project is designed for teaching assistants and aims to guide students through the process of implementing socket programming in QT. The project focuses on a game similar to Bomberman, where students are required to implement both UDP and TCP communication protocols.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project is a part of the Computer Networks course and serves as a practical assignment for students. The goal is to develop a multiplayer game similar to Bomberman using QT framework and implement both UDP and TCP protocols for communication between the game clients and server.

## Features

- Multiplayer game similar to Bomberman
- Implementation of UDP and TCP protocols
- Real-time communication between clients and server
- User-friendly interface using QT framework

## Requirements

To run this project, you will need the following:

- QT framework (version 5.15 or later)
- C++ compiler
- CMake (optional, for building the project)

## Installation

Follow these steps to set up the project on your local machine:

1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/CN-CA1-Bomberman.git
    ```
2. Navigate to the project directory:
    ```bash
    cd CN-CA1-Bomberman
    ```
3. Open the project in QT Creator or build it using CMake:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

## Usage

1. Run the server:
    ```bash
    ./server
    ```
2. Run the client:
    ```bash
    ./client
    ```

## Project Structure

```
CN-CA1-Bomberman/
├── src/                # Source files for the project
│   ├── client/         # Client-side code
│   ├── server/         # Server-side code
│   ├── common/         # Common code for both client and server
├── include/            # Header files
├── assets/             # Game assets (images, sounds, etc.)
├── CMakeLists.txt      # CMake build script
└── README.md           # Project README file
```

## Contributing

We welcome contributions from everyone. If you have any suggestions or improvements, feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.
