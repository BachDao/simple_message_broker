Simple Message Broker
------

Overview
------
Implementation of simple message broker that supports very basic features:

- Single server
- Multiple clients can connect to a server, subscribe and consume a message
  from a specific topic.

Contents
------

* [Overview](#overview)
* [How to build](#how-to-build)
* [Usages](#Usages)
* [Message format](#message-format)
* [Connection flow](#connection-flow)
* [Design notes](#design-notes)
  
  
How to build
------
Build tool: CMake, Ninja
Dependencies management: vcpkg

When configuring CMake, it is set to automatically retrieve vcpkg, set it up, and build the vcpkg binary. 
Vcpkg, in turn, will download and build all the necessary dependencies (include boost and protobuf), eliminating the 
need for any pre-installation on your machine

```cmake
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -G Ninja -S ./simple_message_broker -B ./simple_message_broker/cmake-build-debug

cmake --build /simple_message_broker/cmake-build-debug 
--target [put-target-name-here] -j 8
```

**Notes**
+ Dockerfile is also provided for easier deployment, but currently it's seem have problem with Vcpkg when using on Alpine
image. I will try to solve it and put docker image here later.

Usages
------
Server is created simple by:

````c++
auto port = 3000;
msg_broker::server::server s(3000);
````

Client first needs to connect to server, after that, each topic instance is
created by simple calling subscribe function on connected object:

```c++
auto ptrClient = client::connect_to("127.0.0.1", 3000, "client_id");
auto ptrTopic = ptrClient->subscribe("topic_name");
```

With topic, a client can install handler for an incoming message.

````c++
ptrTopic->on_message([](const std::string& msg) {
    std::cout << "receive message" << std::endl;
});
````

or, publish a message to another client that subscribed to this topic:

```c++
ptrTopic->send_message("hi guys");
```

and say goodbye:

```c++
ptrTopic->unsubscribe();
```

Currently, clients can only send and receive a string-message, but any object that supports serialize/deserialize
to a byte array also transfers easily.

Message format
------

The Message has a maximum 1MB in size, consists of header, and (optional) server_topic and payload:

- Mandatory header includes: **message_type and client_id**
- Optional is: **topic_value and payload**

<div style="text-align: center;">

| message_type | client_id | server_topic (optional) | payload (optional) | 
|:------------:|:---------:|:-----------------------:|:------------------:|

</div>

<br/>
Message can be Control or Normal:

- Control message:
    + CONNECT, CONNECT_ACK, DISCONNECT: only have mandatory header.
    + TOPIC_SUBSCRIBE, TOPIC_UNSUBSCRIBE: include mandatory header, plus and **server_topic** field
      <br/>

- NORMAL message: user-facing message type, and it has all the headers above plus **payload**. The **payload**
  carry user-data
  <br/>

Connection flow
------

**Init connection**

1. Client opens TCP connection to server, on specific port.
2. Client sends CONNECT message to the server.
3. Server reply with CONNECT_ACK, and connection is established.

   At this stage, only NORMAL, TOPIC_*, and DISCONNECT message is accepted. Otherwise, the connection is terminated.

**Error handling:**

- During operation, if a client or server receives any message in the wrong format, or the message that
  does not be appropriate, it will terminate the connection.

**Teardown**

- The Session is end when any peer emits a DISCONNECT message.

Design notes:
------
We use Boost-Asio for socket polling, Protobuf for Message serialize/deserialize.

##### Connection object
At its core, each client is represented by a Connection object, comprising:

+ A socket
+ A Preallocate Buffer

The Connection object encapsulates all network-related functionality and provides a clean and simple interface to the 
upper layers. It facilitates the sending and receiving of individual messages in one piece.
##### Message handler

Built on top of the Connection object is the MessageHandler, responsible for processing different message types
and guiding the Connection object regarding the continuation of the client's interaction.

The Message Handler also routes non-controlling messages to the appropriate **Topic** using the help of the 
**TopicManager**. Each Topic object holds Weak-References to **Connection** objects that have subscribed to that topic. 
This ensures that messages are dispatched correctly to all relevant connections and **Connections** that are no longer 
needed are released appropriately.

#### Limitations:
_Memory management_:
+ **Fixed-size buffers**: Each Connection object holds a fixed-size buffer, which may result in inefficient memory 
utilization if clients primarily transfer small messages. To address this, consider dynamically allocating buffer 
sizes based on message size or implementing buffer resizing strategies.

 
+ **Heap allocation**: When a Connection receives a complete message, it currently allocates a new block of heap memory 
to store the message. This can lead to unnecessary memory allocations. To optimize memory usage, consider implementing a 
custom allocator or reusing existing memory blocks when possible.
  

+ **Protobuf**: While Protobuf provides convenience for rapid development, if you require fine-grained control 
over memory management or partial message passing, it may be beneficial to implement custom serialize/deserialize code


_Fixed Number of Worker Threads_:
+ The current design relies on a fixed number of worker threads. However, depending on the workload, this configuration 
may not be optimal



