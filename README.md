# CN CA1 - Bomberman Game with Socket Programming in QT

Welcome to the CN CA1 repository! This project is designed for teaching assistants and aims to guide students through the process of implementing socket programming in QT. The project focuses on a game similar to Bomberman, where students are required to implement both UDP and TCP communication protocols.

## Table of Contents

- [CN CA1 - Bomberman Game with Socket Programming in QT](#cn-ca1---bomberman-game-with-socket-programming-in-qt)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Features](#features)
  - [Requirements](#requirements)
  - [TCP](#tcp)
    - [**Constructor (TCPManager::TCPManager)**](#constructor-tcpmanagertcpmanager)
    - [**Destructor (TCPManager::~TCPManager)**](#destructor-tcpmanagertcpmanager)
    - [**initialize**](#initialize)
    - [**setupServer**](#setupserver)
    - [**setupClient**](#setupclient)
    - [**onNewConnection**](#onnewconnection)
    - [**onReadyRead**](#onreadyread)
    - [**onDisconnected**](#ondisconnected)
    - [**sendData**](#senddata)
    - [**stop**](#stop)
  - [Questions](#questions)
    - [1. What is a socket, and what role does it play in network communication?](#1-what-is-a-socket-and-what-role-does-it-play-in-network-communication)
    - [2. What are the differences between TCP and UDP in terms of connection management and data delivery guarantees?](#2-what-are-the-differences-between-tcp-and-udp-in-terms-of-connection-management-and-data-delivery-guarantees)
    - [3. In what situations is TCP a better choice than UDP, and vice versa?](#3-in-what-situations-is-tcp-a-better-choice-than-udp-and-vice-versa)
    - [4. Why is UDP a common choice for multiplayer games like Bomberman?](#4-why-is-udp-a-common-choice-for-multiplayer-games-like-bomberman)

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

## TCP
<!-- TODO: mention duplicate data send -->

### Constructor (TCPManager::TCPManager)

**Purpose:** Initializes the TCP manager, setting up the necessary network components based on the specified role (client or server).

**Challenge:** Ensuring proper initialization of either the server or client without blocking the main thread.
**Solution:** Uses `QMetaObject::invokeMethod()` to defer setup execution, allowing the server or client to be initialized asynchronously.

### Destructor (TCPManager::~TCPManager)

**Purpose:** Cleans up resources by stopping the network connections and safely terminating the network thread.

**Challenge:** Ensuring proper shutdown of the network thread to prevent dangling operations.
**Solution:** Calls `stop()` to close active connections, followed by `quit()` and `wait()` on the network thread to ensure a clean exit.

### initialize

**Purpose:** Sets up the TCP connection based on the role (server or client), using the provided IP address and port.

**Challenge:** Handling role-specific initialization dynamically.
**Solution:** Uses a lambda inside `QMetaObject::invokeMethod()` to decide whether to initialize a server (`setupServer()`) or a client (`setupClient()`).

### setupServer

**Purpose:** Creates and starts a `QTcpServer` to listen for incoming connections.

**Challenge:** Handling server startup failures (e.g., port already in use).
**Solution:** Checks the result of `listen()`, and if it fails, emits an `errorOccurred` signal with the error message.

### setupClient

**Purpose:** Establishes a client connection to the specified server.

**Challenge:** Detecting connection failures and responding accordingly.
**Solution:** Connects `QTcpSocket` signals (`connected`, `readyRead`, `disconnected`) to relevant slots and emits `connectionStatusChanged` when connected.

### onNewConnection

**Purpose:** Accepts incoming client connections on the server side and sets up the necessary event listeners.

**Challenge:** Managing multiple client connections while ensuring safe resource handling.
**Solution:** Stores the client socket from `nextPendingConnection()`, connects it to relevant slots, and emits `connectionStatusChanged`.

### onReadyRead

**Purpose:** Reads incoming data from the socket and emits it as a JSON object.

**Challenge:** Handling partial or invalid JSON data.
**Solution:** Converts incoming data to `QJsonDocument`, verifies it, and only emits valid JSON objects.

### onDisconnected

**Purpose:** Handles disconnection events by cleaning up resources and notifying the application.

**Challenge:** Preventing memory leaks when a client disconnects from the server.
**Solution:** Deletes the socket using `deleteLater()` and sets it to `nullptr` to avoid dangling pointers.

### sendData

**Purpose:** Sends a JSON object over the TCP connection.

**Challenge:** Ensuring data is sent only when the socket is in a connected state.
**Solution:** Checks the socket state before writing data and uses `flush()` to ensure the data is transmitted immediately.

### stop

**Purpose:** Stops the TCP connection by closing the server or disconnecting the client.

**Challenge:** Handling both server and client shutdown properly.
**Solution:** Calls `close()` for the server and `disconnectFromHost()` for the client to ensure a graceful disconnection.

This report follows the same structured approach as the AudioInput report, highlighting each function's **purpose, challenges, and solutions** concisely while maintaining technical clarity. 🚀

## Questions

<!-- TODO: add project-related sample -->
### 1. What is a socket, and what role does it play in network communication?

A socket is a software endpoint that enables communication between devices over a network. It provides an interface for applications to send and receive data using protocols like TCP or UDP. Sockets help establish connections, manage data flow, and ensure proper communication between processes running on different machines. It acts as a bridge between software applications and the network protocol stack, allowing different processes, whether on the same machine or across different devices, to communicate over the internet or local networks. In our project, we use sockets to establish connections between the game clients and the server, enabling real-time communication between them.

### 2. What are the differences between TCP and UDP in terms of connection management and data delivery guarantees?

- TCP (Transmission Control Protocol) is connection-oriented, meaning it establishes a reliable connection before data transmission. It ensures ordered, error-free delivery with mechanisms like acknowledgments and retransmissions, but it's slower, due to overhead but ideal for applications that need accuracy. It's typically used for things like web browsing, file downloads, email, database access.

- UDP (User Datagram Protocol) is connection-less and does not guarantee delivery, order, or error correction. It simply sends data packets (datagrams) without ensuring they arrive at the destination or in sequence. UDP is faster but less reliable than TCP. It's ideal for things like online gaming, video streaming, VoIP, DNS queries.

### 3. In what situations is TCP a better choice than UDP, and vice versa?

- TCP: Data integrity and reliability are more important than speed or applications require ordered and complete data. Examples include web browsing (HTTP/HTTPS), email (SMTP, IMAP), and file transfers (FTP).

- UDP: Speed and real-time responsiveness are more important than reliability or minor packet loss is acceptable. Examples include online gaming, video streaming, VoIP, DNS queries.

### 4. Why is UDP a common choice for multiplayer games like Bomberman?

UDP is favored in multiplayer games because of its low latency and minimal overhead. Games like Bomberman require fast-paced, real-time communication where slight packet loss is preferable to delays caused by retransmissions. UDP ensures quick data transmission, keeping gameplay smooth and responsive. Also UDP supports sending data to multiple players simultaneously, making it ideal for multiplayer games.
