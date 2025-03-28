# CN CA1 - Bomberman Game with Socket Programming in QT

This project serves as a practical assignment for the Computer Networks course. The objective is to develop a 2-player game similar to Bomberman using the Qt framework and implement both UDP and TCP protocols for communication between the game clients and the server. This hands-on project aims to help us understand how real-time network communication works in multiplayer games and how different protocols can be used to achieve communication goals in gaming environments.

## Table of Contents

- [Overview of Classes and Their Communication](#overview-of-classes-and-their-communication)
  - [NetworkManager (Base Class)](#networkmanager-base-class)
  - [TCPManager](#tcpmanager)
  - [UDPManager](#udpmanager)
  - [GameNetworkManager](#gamenetworkmanager)
  - [Player](#player)
  - [Game](#game)
- [Communication Flow Between Classes](#communication-flow-between-classes)
  - [Player Class](#player-class)
  - [GameNetworkManager Class](#gamenetworkmanager-class)
  - [Game Class](#game-class)
  - [Network Managers (TCPManager and UDPManager)](#network-managers-tcpmanager-and-udpmanager)
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

5. **Player**
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

### **TCPManager:**

#### **Fields:**
1. **m_server**:
   - A pointer to a `QTcpServer` that listens for incoming connections from clients when the `NetworkManager` role is set to **Server**.

2. **m_socket**:
   - A pointer to a `QTcpSocket` used by both the server and client. On the server side, it represents the connection to the client, and on the client side, it is used to establish a connection to the server.

#### **Methods:**
1. **TCPManager(QObject* parent = nullptr)**:
   - The constructor of `TCPManager`, which initializes the base class `NetworkManager` and the `m_server` and `m_socket` pointers to `nullptr`.

2. **~TCPManager()**:
   - The destructor cleans up by calling `stop()` to disconnect and delete the server or client socket.

3. **initialize(Role role, const QString& address, quint16 port)**:
   - Initializes the TCP manager by setting the role, address, and port. It invokes `setupServer()` or `setupClient()` based on the role.

4. **setupServer()**:
   - Creates a `QTcpServer` instance, sets it to listen on the provided address and port, and connects the new connection signal to the `onNewConnection()` slot.

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
   - The constructor initializes the base class `NetworkManager` and sets the `m_socket` pointer to `nullptr`.

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
   - A pointer to the `NetworkManager` base class that is responsible for sending and receiving data over the network.

2. **protocol**:
   - The selected communication protocol (`"TCP"` or `"UDP"`).

3. **selectedPlayer**:
   - ID of the currently selected player in the game. It is used to filter network events and ensure that only relevant data is processed for the selected player.

4. **updateSequenceNumber**:
   - The sequence number used for sending player state updates. This helps to keep track of the updates and prevent the use of outdated state information.

5. **bombSequenceNumber**:
   - An integer used specifically for bomb placements. This sequence number ensures that bomb placement messages are sent multiple times (to handle packet loss) and that each bomb placement is uniquely identified.

6. **receivedBombSequenceNumbers**:
   - A set that stores the sequence numbers of received bomb placement messages. It is used to track the sequence numbers of the bombs placed by the other player and prevent processing the same bomb placement message more than once.

#### **Methods:**
1. **GameNetworkManager(int selectedPlayer, const QString& protocol, QObject* parent = nullptr)**:
   - The constructor initializes the network manager, setting the `selectedPlayer` and `protocol` based on the arguments. It also initializes the `m_networkManager` pointer to `nullptr`.

2. **setup()**:
   - This method sets up the **NetworkManager** based on the provided protocol. It also attempts to initialize the network manager, either as a **server** or **client**, and connects relevant signals for network events.

3. **connectNetworkSignals()**:
   - This method connects the necessary signals and slots for handling network events, such as connection status changes, data reception, and errors.

4. **onPlayerDied(int playerId)**:
   - This slot is triggered when a player dies. It sends a message containing the player’s ID over the network using the `m_networkManager` to notify the other player about the death.

5. **onPlayerMoved(int playerId, Qt::Key key, bool isPressed)**:
   - This slot is triggered when a player moves. It sends the movement data (player ID, key press state) to the other player over the network.

6. **onPlayerPlacedBomb(int playerId)**:
   - This slot is triggered when a player places a bomb. It sends the bomb placement data over the network. It also adds a sequence number for the bomb placement and sends the message **three times** (in case of packet loss). This ensures that the bomb placement is reliably communicated to the other player.

7. **sendUpdatedPlayerState(int playerId, qreal x, qreal y, int health)**:
   - This slot sends the updated player state (position and health) over the network. It increments the `updateSequenceNumber` to ensure that each state update has a unique sequence number.

8. **onDataReceived(const QJsonObject& data)**:
   - This slot is triggered when data is received from the network. It processes the incoming data, extracts the relevant information and emits the corresponding signals to update the game state.
   - It also handles **duplicate bomb placement messages** by checking the received sequence number against `receivedBombSequenceNumbers`. If the bomb placement has already been processed, the message is ignored.

9. **onConnectionStatusChanged(bool connected)**:
   - This slot is triggered when the connection status changes. It logs the connection status and sends a **connection status message** to the peer when the client is connected.

10. **onErrorOccurred(const QString& message)**:
    - This slot is triggered when an error occurs in the network manager. It logs the error message for debugging purposes.

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
   - It is responsible for sending and receiving game state data.

2. **lastReceivedSequenceNumber**:
    - An integer that tracks the last sequence number of the received player state updates. This is used to handle out-of-order packets and ensure that the most recent game state is applied.

3. **stateUpdateTimer**:
    - A `QTimer` object that periodically triggers the sending of the player’s state over the network. This ensures the game state is continuously updated.

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
   - This method handles the game-over scenario. It identifies the winner based on the player who survived, displays a message box announcing the winner, and then quits the game after a brief delay.

6. **handlePlayerDied(int playerId)**:
    - This method is called when a player dies. It updates the player's state to show the death animation and triggers the game over logic if required.

7. **handlePlayerMoved(int playerId, int key, bool isPressed)**:
    - This method is called when a player moves. It updates the player’s movement state and applies the changes to their position in the game.

8. **handlePlayerPlacedBomb(int playerId)**:
    - This method is called when a player places a bomb. It triggers the appropriate bomb placement logic in the game and sends the bomb placement information over the network.

9. **updatePlayerState(int playerId, qreal x, qreal y, int health)**:
    - This method updates the state of a specific player by setting their position (`x`, `y`) and health. It is used when the game receives state updates from the network.

10. **handleStateUpdateReceived(int sequenceNumber)**:
    - This method is called when a state update is received from the network. It checks whether the received state update has a higher sequence number than the last one, ensuring that outdated state updates are ignored.

11. **emitPlayerState()**:
    - This method is triggered by the `stateUpdateTimer`. It emits a signal with the current player state for sending over the network to other players.

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
