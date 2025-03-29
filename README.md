# CN CA1 - Bomberman Game with Socket Programming in QT

This project serves as a practical assignment for the Computer Networks course. The objective is to develop a 2-player game similar to Bomberman using the Qt framework and implement both UDP and TCP protocols for communication between the game clients and the server. This hands-on project aims to help us understand how real-time network communication works in multiplayer games and how different protocols can be used to achieve communication goals in gaming environments.

## Table of Contents

- [Overview of Classes and Their Communication](#overview-of-classes-and-their-communication)
  - [NetworkManager](#networkmanager-base-class)
  - [TCPManager](#tcpmanager)
  - [UDPManager](#udpmanager)
  - [GameNetworkManager](#gamenetworkmanager)
  - [Player](#player)
  - [Game](#game)
- [TCPManager](#tcpmanager)
  - [Fields](#fields)
  - [Methods](#methods)
- [UDPManager](#udpmanager)
  - [Fields](#fields-1)
  - [Methods](#methods-1)
- [GameNetworkManager](#gamenetworkmanager-1)
  - [Fields](#fields-2)
  - [Methods](#methods-2)
- [Game](#game-1)
  - [Fields](#fields-3)
  - [Methods](#methods-3)
- [Questions](#questions)
  - [1. What is a socket, and what role does it play in network communication?](#1-what-is-a-socket-and-what-role-does-it-play-in-network-communication)
  - [2. What are the differences between TCP and UDP in terms of connection management and data delivery guarantees?](#2-what-are-the-differences-between-tcp-and-udp-in-terms-of-connection-management-and-data-delivery-guarantees)
  - [3. In what situations is TCP a better choice than UDP, and vice versa?](#3-in-what-situations-is-tcp-a-better-choice-than-udp-and-vice-versa)
  - [4. Why is UDP a common choice for multiplayer games like Bomberman?](#4-why-is-udp-a-common-choice-for-multiplayer-games-like-bomberman)

## Overview of Classes and Their Communication

We added the following classes into the existing code to implement networking features:

1. **NetworkManager**  
   This class offers a shared interface for basic network operations like sending and receiving data, managing errors, and handling connections. Both **TCPManager** and **UDPManager** extend this class to add their protocol-specific features.

2. **TCPManager**
   - This class manages the TCP connection and communication for the game. It is responsible for establishing the server-client connection, handling incoming and outgoing messages, and managing connection events.

3. **UDPManager**
   - This class is responsible for handling the **UDP** communication in the game.

4. **GameNetworkManager**
   - This is the main class for handling the game's network communication. It uses the **NetworkManager**, which can be TCPManager or UDPManager, to send and receive data, such as player movements, bomb placements, and game state updates. 

   - It acts as the bridge between the network layer and the game layer and its purpose is to isolate the networking stuff from game logic and adhere single responsibility principle. It listens for signals from the Player class and then sends appropriate data to the network manager. It also handles incoming data and updates the game state by emitting signals for the game class. The parts that are related to network, like checking duplicated sequence numbers, are done in this class but the game logic is handled in Game.

5. **Player**
   - We **extended** this class by adding signals for network communication. These signals notify the game and its network manager when below actions occur:
     - `playerMoved`
     - `playerDied`
     - `playerPlacedBomb`
   
   These signals are connected to **GameNetworkManager**. When a player moves, the **playerMoved** signal is emitted, and **GameNetworkManager** sends the movement data to the other player over the network.

6. **Game**
   - The **Game** class integrates with **GameNetworkManager** to synchronize the game state between multiple players.
   - This class processes input from the player and updates the game world. It listens for signals from **GameNetworkManager** to apply network updates such as player movement, health changes, or game-over.
   - The network related logic is moved to a new thread to prevent it from introducing lag to the game.

---

## **Class Explanation**

### **TCPManager:**

#### **Fields:**
1. **m_server**:
   - A pointer to a `QTcpServer` that listens for incoming connections from clients when the `NetworkManager` role is set to **Server**.

2. **m_socket**:
   - A pointer to a `QTcpSocket` used by both the server and client. On the server side, it represents the connection to the client, and on the client side, it is used to establish a connection to the server.

#### **Methods:**
1. **TCPManager(QObject* parent = nullptr)**:
   - Constructor

2. **~TCPManager()**:
   - The destructor cleans up by calling `stop()` to disconnect and delete the server or client socket.

3. **initialize(Role role, const QString& address, quint16 port)**:
   - Initializes the TCP manager by setting the role, address, and port. It invokes `setupServer()` or `setupClient()` based on the role.
   - The first player that joins the game will be server and the other one is client.

4. **setupServer()**:
   - Creates a `QTcpServer` instance, sets it to listen on the address and port, and connects the new connection signal to the `onNewConnection()` slot.

5. **setupClient()**:
   - Creates a `QTcpSocket`, connects its signals for connection, error, data reading, and disconnection, and tries to connect to the server using the provided address and port.

6. **onNewConnection()**:
   - Called when a new connection is received by the server. It accepts the connection and sets up the `m_socket` with the client’s socket for communication.

7. **onReadyRead()**:
   - Reads data from the socket when available. It converts the incoming data into a `QJsonDocument` and emits the `dataReceived()` signal.

8. **onDisconnected()**:
   - Called when the client disconnects. It cleans up the socket and emits the `connectionStatusChanged(false)` signal.

9. **sendData(const QJsonObject& data)**:
   - Sends a `QJsonObject` over the established connection. It checks if the socket is in a connected state and writes the data to the socket.

10. **stop()**:
    - Stops the server or client connection by closing the socket and releasing resources.

---

### **UDPManager**

#### **Fields:**
1. **m_socket**:
   - A pointer to a `QUdpSocket`. This is used for the actual UDP communication. It is responsible for sending and receiving data via UDP.
   
2. **m_peerAddress**:
   - A `QHostAddress` object that stores the address of the peer to which the UDP data will be sent. This field is used on the server side to keep track of the last known client address to send data back to.

3. **m_peerPort**:
   - It is used alongside **m_peerAddress** to send UDP data back to the correct peer.

#### **Methods:**
1. **UDPManager(QObject* parent = nullptr)**:
   - Constructor

2. **~UDPManager()**:
   - The destructor stops the UDP communication by calling `stop()` to ensure proper cleanup.

3. **initialize(Role role, const QString& address, quint16 port)**:
   - This method initializes the **UDPManager**. It sets the role, address, and port. 
   - If the role is **Server**, it binds the `m_socket` to the specified address and port (`QHostAddress::Any` for the server to accept connections from any client).
   - If the role is **Client**, it binds the `m_socket` to an available port to listen for incoming responses.

4. **onReadyRead()**:
   - This slot is called when data is available for reading from the socket. It checks for pending datagrams and reads them into a `QByteArray`.
   - If the role is **Server**, it stores the sender's address and port into `m_peerAddress` and `m_peerPort` for future communication.
   - The received data is then converted into a `QJsonDocument`, and if it's valid, the `dataReceived()` signal is emitted with the data.

5. **sendData(const QJsonObject& data)**:
   - This method sends the data over UDP to the appropriate peer address. The target address and port depend on whether the **UDPManager** is acting as the **Server** or **Client**:
     - If the **Server**, it sends data to the last known client’s address and port.
     - If the **Client**, it sends data to the server's address and port.
   - The `QJsonObject` is first converted into a `QJsonDocument`, then into a `QByteArray` and sent via `writeDatagram()`.

6. **stop()**:
   - This method closes the UDP socket and deletes it to free resources.

### **GameNetworkManager**

#### **Fields:**
1. **m_networkManager**:
   - Pointer to network manager for sending messages over the network.

2. **protocol**:
   - The selected protocol.

3. **selectedPlayer**:
   - Player Id.

4. **updateSequenceNumber**:
   - The sequence number used for sending player state updates. The state updates are sent in fixed intervals to avoid divergense in game state caused by packet loss.

5. **bombSequenceNumber**:
   - The mechanism for handling packet loss for bomb placements is to send the message of a placement multiple times so we assign a unique seq number for each placement(all of its messages have the same seq number) so that each bomb effects the game state only once.

6. **receivedBombSequenceNumbers**:
   - A set to check if the received bomb sequence number is new or has been placed before.

7. **lastReceivedSequenceNumber**:
    - An integer that tracks the last sequence number of the received player state updates. This is used to handle out-of-order packets and ensure that the most recent game state is applied.

#### **Methods:**
1. **GameNetworkManager(int selectedPlayer, const QString& protocol, QObject* parent = nullptr)**:
   - Constructor

2. **setup()**:
   - This method sets up the **NetworkManager** based on the provided protocol. It also attempts to initialize the network manager, either as a **server** or **client**, and connects relevant signals for network events.

3. **connectNetworkSignals()**:
   - This method connects the necessary signals and slots for handling network events, such as connection status changes, data reception, and errors.

4. **onPlayerDied(int playerId)**:
   - This slot is triggered when a player dies. It sends a message containing the player’s ID over the network using the `m_networkManager` to notify the other player about the death.

5. **onPlayerMoved(int playerId, Qt::Key key, bool isPressed)**:
   - This slot is triggered when a player moves. It sends the movement data to the other player over the network.

6. **onPlayerPlacedBomb(int playerId)**:
   - This slot is triggered when a player places a bomb. It sends the bomb placement data over the network. It also adds a sequence number for the bomb placement and sends the message **three times**. This ensures that the bomb placement is reliably communicated to the other player.

7. **sendUpdatedPlayerState(int playerId, qreal x, qreal y, int health)**:
   - This slot sends the updated player state over the network. It increments the `updateSequenceNumber` to ensure that each state update has a unique sequence number.

8. **onDataReceived(const QJsonObject& data)**:
   - This slot is triggered when data is received from the network. It processes the incoming data, extracts the relevant information and emits the corresponding signals to update the game state.
   - It handles duplicate bomb placement messages by checking the received sequence number against `receivedBombSequenceNumbers`. If the bomb placement has already been processed, the message is ignored. It also handles sequence numbers for state updates and ignored outdated states that are received.

9. **onConnectionStatusChanged(bool connected)**:
   - This slot is triggered when the connection status changes.

10. **onErrorOccurred(const QString& message)**:
    - This slot is triggered when an error occurs in the network manager.

11. **messageTypeToString(MESSAGE_TYPE type)**:
    - Helper function

12. **messageFieldToString(MESSAGE_FIELD field)**:
    - Helper function

13. **stringToMessageType(const QString& type)**:
    - Helper function

14. **stringToMessageField(const QString& field)**:
    - Helper function

### **Game**

#### **Fields:**

1. **m_gameNetworkManager**:
   - Handles networking related stuff in the game.

2. **stateUpdateTimer**:
    - This timer periodically triggers the sending of the player’s state over the network. This ensures the game state is continuously updated.

#### **Methods:**
1. **Game(int selectedPlayer, const QString& protocol, QObject* parent = nullptr)**:
   - The constructor of the `Game` class. It initializes the game environment, including the view, network manager, timer, and other necessary components. It sets up the selected player and the chosen protocol.

2. **setFocusOnPlayer()**:
   - We modified this method to connect the player's signals.

3. **connectNetworkSignals()**:
   - Connects the necessary signals from `GameNetworkManager` to the appropriate slots in the `Game` class. These signals include player movements, bomb placements, player deaths, and state updates.

4. **connectPlayerSignals(QPointer<Player> player)**:
   - Connects the signals from the **Player** class to the relevant slots in the `GameNetworkManager`. These include signals like `playerDied`, `playerMoved`, and `playerPlacedBomb`.

5. **gameOver(int diedPlayerId)**:
   - This method handles the game-over scenario. It identifies the winner based on the player who survived, displays a message box announcing the winner, and then quits the game.

6. **handlePlayerDied(int playerId)**:
    - Slot to handle playerDied.

7. **handlePlayerMoved(int playerId, int key, bool isPressed)**:
    - Slot to handle playerMoved.

8. **handlePlayerPlacedBomb(int playerId)**:
    - Slot to handle playerPlacedBomb.

9. **updatePlayerState(int playerId, qreal x, qreal y, int health)**:
    - It is used when the game receives state updates from the network. The other player sends their position and health and here their position in the current player's view gets updated.

10. **emitPlayerState()**:
    - This method is triggered by the `stateUpdateTimer`. It emits a signal with the current player state for sending over the network to other players.

## Questions

### 1. What is a socket, and what role does it play in network communication?

A **socket** is an endpoint for communication between two machines over a network. It allows applications to send and receive data using protocols such as **TCP** and **UDP**. In this project, sockets are used to establish communication between game clients and the server, so that the two players can interact in real-time.

### 2. What are the differences between TCP and UDP in terms of connection management and data delivery guarantees?

- **TCP** is **connection-oriented**. It guarantees ordered, error-free delivery using acknowledgments and retransmissions, but it comes with higher overhead, making it slower. It's ideal for applications requiring reliable communication and when the order and delivery of the data are more important than transmission speed.

- Since **UDP** is **connection-less**, it doesn’t guarantee packet delivery or maintain order—but that’s exactly what makes it **faster** and **lighter** than TCP. For real-time games, speed matters more than perfect reliability (a few missed position updates won’t ruin the game, but lag will).  

To handle UDP’s shortcomings, these are some common mechanisms:  
- **Sequence Numbers**: Packets are tagged with incrementing IDs so the receiver can detect lost or out-of-order data.  
- **Acknowledgements**: It can be used only for critical events to ensure delivery.
- **Interpolation/Prediction**: For example in a game, if a packet is lost, the game briefly predicts missing player movements until the next update arrives.  
- **Redundancy**: Sending the same data in multiple packets to increase the probability of its delivery

- In this game we used a combination of sequence numbers and redundancy to handle lost packets.

### 3. In what situations is TCP a better choice than UDP, and vice versa?

- **TCP** is better when **reliability** is critical, such as for web browsing, email, and file transfers.
- **UDP** is better for **real-time applications** where low latency is important, such as gaming, live video streaming, and VoIP.

### 4. Why is UDP a common choice for multiplayer games like Bomberman?

UDP is ideal for multiplayer games like **Bomberman** because it offers low latency and fast communication. In real-time games, **slight packet loss** is often acceptable as it does not significantly affect gameplay. UDP's ability to quickly transmit data without the overhead of error correction and ordering makes it perfect for fast-paced multiplayer games.
