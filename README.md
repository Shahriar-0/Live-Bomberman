# CN CA1 - Bomberman Game with Socket Programming in QT

Welcome to the CN CA1 repository! This project is part of the Computer Networks course and serves as a practical assignment where students develop a 2-player game similar to Bomberman using the QT framework. The project requires the implementation of both UDP and TCP protocols for communication between the game clients and server.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [TCP](#tcp)
  - [Constructor](#constructor)
  - [Destructor](#destructor)
  - [initialize](#initialize)
  - [setupServer](#setupserver)
  - [setupClient](#setupclient)
  - [onNewConnection](#onnewconnection)
  - [onReadyRead](#onreadyread)
  - [onDisconnected](#ondisconnected)
  - [sendData](#senddata)
  - [stop](#stop)
- [UDP](#udp)
  - [Constructor](#constructor-1)
  - [Destructor](#destructor-1)
  - [initialize](#initialize-1)
  - [onReadyRead](#onreadyread-1)
  - [sendData](#senddata-1)
  - [stop](#stop-1)
- [Game](#game)
  - [Constructor](#constructor-2)
  - [setupNetwork](#setupnetwork)
  - [onDataReceived](#ondatareceived)
  - [onConnectionStatusChanged](#onconnectionstatuschanged)
  - [errorOccurred](#erroroccurred)
  - [connectPlayerSignals (playerDied, playerMoved, playerPlacedBomb)](#connectplayersignals-playerdied-playermoved-playerplacedbomb)
  - [update](#update)
- [Questions](#questions)
  - [1. What is a socket, and what role does it play in network communication?](#1-what-is-a-socket-and-what-role-does-it-play-in-network-communication)
  - [2. What are the differences between TCP and UDP in terms of connection management and data delivery guarantees?](#2-what-are-the-differences-between-tcp-and-udp-in-terms-of-connection-management-and-data-delivery-guarantees)
  - [3. In what situations is TCP a better choice than UDP, and vice versa?](#3-in-what-situations-is-tcp-a-better-choice-than-udp-and-vice-versa)
  - [4. Why is UDP a common choice for multiplayer games like Bomberman?](#4-why-is-udp-a-common-choice-for-multiplayer-games-like-bomberman)

## Introduction

This project serves as a practical assignment for the **Computer Networks** course. The goal is to develop a **2-player game** similar to Bomberman using the **QT framework** and implement both **UDP and TCP protocols** for communication between the game clients and the server. Students will experience how real-time network communication works in multiplayer games and learn how different protocols can be used to achieve communication goals in gaming environments.

## Features

- **Multiplayer Game**: A real-time multiplayer game similar to **Bomberman**.
- **Protocol Implementation**: Both **UDP** and **TCP** protocols are implemented for communication.
- **Real-Time Communication**: The game supports real-time communication between clients and the server.
- **User Interface**: The game utilizes the **QT framework** to create a simple and user-friendly interface.

## Requirements

To run this project, you will need the following:

- **QT Framework** (version 5.15 or later)
- **C++ Compiler**
- **CMake** (optional, for building the project)

## Overview of Classes and Their Communication

In our implementation, we created several key classes that work together to facilitate communication and gameplay in the game.

1. **NetworkManager (Base Class)**
   - This class serves as the base class for both **TCPManager** and **UDPManager**. It provides a common interface for handling network communication, such as sending and receiving data, error handling, and connection management. Both the **TCPManager** and **UDPManager** inherit from this class to implement protocol-specific functionality.

2. **TCPManager**
   - This class manages the TCP connection and communication for the game. It is responsible for establishing the server-client connection, handling incoming and outgoing messages, and managing connection events.

3. **UDPManager**
   - This class is responsible for handling the **UDP** communication in the game. UDP is used to send quick, real-time data without the overhead of connection management, which is ideal for fast-paced games.
   - **UDPManager** uses **QUdpSocket** to send and receive data packets between the server and the client.

4. **GameNetworkManager**
   - This class is the central hub for the game's network communication. It interacts with the **NetworkManager** (either **TCPManager** or **UDPManager**) to send and receive data, such as player movements, bomb placements, and game state updates. 
   - **GameNetworkManager** acts as the bridge between the network layer and the game layer. It listens for signals from the **Player** class and then sends appropriate data to the network manager. It also handles incoming data and updates the game state accordingly by emitting signals for the **Game** class to process.
   - It communicates with the **Game** class via **signals and slots**. When network events occur , **GameNetworkManager** emits signals, which the **Game** class listens to in order to update the game state.

### 5. **Player**
   - The **Player** class manages the player's **state** and **actions**. We **extended** this class by adding new signals to facilitate network communication. These signals notify the game when important actions occur:
     - `playerMoved` when the player moves, to update the other player’s position over the network.
     - `playerDied` when the player dies, allowing the game to notify other players about the player's death.
     - `playerPlacedBomb` when the player places a bomb, ensuring that the action is transmitted to the other player in a multiplayer game.
   
   - **Communication**: In the updated version, the **Player** class **emits signals** for the actions that occur within the game. These signals are connected to **GameNetworkManager**, which manages the network communication. When a player moves, the **playerMoved** signal is emitted, and **GameNetworkManager** sends the movement data to the other player over the network (via **UDP** or **TCP**). Similarly, other player actions, like placing bombs or dying, trigger the corresponding signals, ensuring that the state is updated across all clients in real-time.

6. **Game**
   - This class manages the main game loop and logic. It creates the game environment, handles player actions, and updates the game state. The **Game** class integrates with **GameNetworkManager** to synchronize the game state between multiple players.
   - This class processes input from the player and updates the game world. It also listens for signals from **GameNetworkManager** to apply network updates such as player movement, health changes, or game-over.

---

### **Communication Flow Between Classes:**

1. **Player Class**: The player performs actions such as movement, placing bombs, or dying. These actions trigger signals like `playerMoved`, `playerPlacedBomb`, or `playerDied`.
   
2. **GameNetworkManager Class**: Upon receiving a signal from the **Player** class, **GameNetworkManager** processes the action (such as movement or bomb placement) and sends the data over the network to the other player using **TCPManager** or **UDPManager**.

3. **Game Class**: **GameNetworkManager** also listens for incoming data about the other player's actions (such as movements or bomb placements) from the network. When this data is received, it triggers signals that the **Game** class listens for to update the game state. For example, the game updates the other player's position or places a bomb at the correct coordinates.

4. **Network Managers (TCPManager and UDPManager)**: These classes handle the actual network communication. They listen for incoming data, parse the messages, and send them to the appropriate destination. They also handle connection management (e.g., establishing and maintaining connections for **TCP**, or managing the sending/receiving of packets for **UDP**).

## TCP

### Constructor

**Purpose**: Initializes the TCP manager by setting up the necessary network components based on whether the role is **client** or **server**.

### Destructor

**Purpose**: Cleans up network resources by stopping connections and safely terminating the network thread.

### initialize

**Purpose**: Sets up the TCP connection based on the role (server or client) and uses the provided IP address and port for the connection.

**Challenge**: Handling role-specific initialization dynamically.
**Solution**: A lambda function is used within `QMetaObject::invokeMethod()` to decide whether to initialize a **server** (`setupServer()`) or **client** (`setupClient()`).

### setupServer

**Purpose**: Initializes and starts a **QTcpServer** to listen for incoming connections.

**Challenge**: Handling server startup failures (e.g., port already in use).
**Solution**: It checks the result of the `listen()` method and, if it fails, emits an `errorOccurred` signal with the error message.

### setupClient

**Purpose**: Establishes a client connection to the specified server address and port.

**Challenge**: Handling connection failures.
**Solution**: The `QTcpSocket` signals are connected to relevant slots to handle connection, reading, and disconnection events. The connection status is updated via `connectionStatusChanged`.

### onNewConnection

**Purpose**: Accepts incoming client connections on the server side and sets up necessary event listeners for the connection.

**Challenge**: Managing multiple client connections safely.
**Solution**: The client socket is stored from `nextPendingConnection()` and connected to relevant slots. It only accepts one connection at a time, and resource handling is managed safely.

### onReadyRead

**Purpose**: Reads incoming data from the socket and emits it as a JSON object.

**Challenge**: Handling partial or malformed data.
**Solution**: Converts incoming data to `QJsonDocument`, validates it, and emits valid JSON data.

### onDisconnected

**Purpose**: Handles disconnection events by cleaning up resources and notifying the application.

**Challenge**: Preventing memory leaks when a client disconnects from the server.
**Solution**: The socket is deleted using `deleteLater()` and set to `nullptr` to avoid dangling pointers.

### sendData

**Purpose**: Sends a JSON object over the TCP connection.

**Challenge**: Ensuring data is sent only when the socket is in a connected state.
**Solution**: The socket's state is checked before writing data, and `flush()` is used to ensure immediate transmission.

### stop

**Purpose**: Stops the TCP connection by closing the server or disconnecting the client.

**Challenge**: Handling proper shutdown for both server and client.
**Solution**: Calls `close()` for the server and `disconnectFromHost()` for the client to ensure a graceful shutdown.

## UDP

### Constructor

**Purpose**: Initializes the UDP manager and prepares it for communication.

### Destructor

**Purpose**: Cleans up resources by stopping the UDP socket and safely deallocating memory.

### initialize

**Purpose**: Sets up the UDP socket for communication, either as a server or client, using the provided IP address and port.

**Challenge**: Correctly binding the socket for both roles.
**Solution**: The server binds to `QHostAddress::Any` on the specified port to listen for incoming messages, while the client binds to an available port for responses. Errors trigger an `errorOccurred` signal.

### onReadyRead

**Purpose**: Handles incoming data from the UDP socket, processes it, and emits it as JSON.

**Challenge**: Correctly extracting and parsing datagrams.
**Solution**: It processes incoming datagrams, stores the sender’s address when the server is involved, and converts the data into a `QJsonDocument` before emitting it.

### sendData

**Purpose**: Sends a JSON object over UDP.

**Challenge**: Ensuring data is sent reliably to the correct target address.
**Solution**: If acting as a server, it sends data to the last known client. If acting as a client, it sends data to the predefined server address. The target address and port are validated before sending.

### stop

**Purpose**: Stops the UDP communication by closing the socket and releasing resources.

**Challenge**: Ensuring proper cleanup.
**Solution**: The socket is closed, deleted using `deleteLater()`, and set to `nullptr` to avoid dangling pointers.

## Game

### Constructor

### setupNetwork

### onDataReceived

### onConnectionStatusChanged

### errorOccurred

### connectPlayerSignals (playerDied, playerMoved, playerPlacedBomb)

### update

## Questions

### 1. What is a socket, and what role does it play in network communication?

A **socket** is an endpoint for communication between two machines over a network. It allows applications to send and receive data using protocols such as **TCP** and **UDP**. In this project, sockets are used to establish communication between game clients and the server, facilitating real-time multiplayer interactions.

### 2. What are the differences between TCP and UDP in terms of connection management and data delivery guarantees?

- **TCP** (Transmission Control Protocol) is **connection-oriented**. It guarantees ordered, error-free delivery using acknowledgments and retransmissions, but it comes with higher overhead, making it slower. It's ideal for applications requiring reliable communication (e.g., web browsing, file transfers).

- **UDP** (User Datagram Protocol) is **connection-less** and doesn't guarantee delivery or order. It is faster and has lower overhead but sacrifices reliability. UDP is ideal for real-time applications like gaming, where speed is more important than perfect delivery.

### 3. In what situations is TCP a better choice than UDP, and vice versa?

- **TCP** is better when **reliability** is critical, such as for web browsing, email, and file transfers.
- **UDP** is better for **real-time applications** where low latency is important, such as gaming, live video streaming, and VoIP.

### 4. Why is UDP a common choice for multiplayer games like Bomberman?

UDP is ideal for multiplayer games like **Bomberman** because it offers low latency and fast communication. In real-time games, **slight packet loss** is often acceptable as it does not significantly affect gameplay. UDP's ability to quickly transmit data without the overhead of error correction and ordering makes it perfect for fast-paced multiplayer games.

