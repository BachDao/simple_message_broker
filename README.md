Simple Message Broker
------

Simple message broker is ....

Overview
------


Contents
------

* [Overview](#overview)

How to build
------


Examples
------


Message format
------

The Message has a maximum 1MB in size, consists of header, and (optional) server_topic and payload:

- Mandatory header includes: **message_type and client_id**
- Optional is: **topic_value and payload**

<div style="text-align: center;">

| message_type | client_id | server_topic (optional) | payload (optional) | 
|:------------:|:---------:|:----------------:|:------------------:|

</div>

<br/>
Message is one of: Control and Normal:

- Control message:
    + CONNECT, CONNECT_ACK, DISCONNECT: only have mandatory header.
    + TOPIC_SUBSCRIBE, TOPIC_UNSUBSCRIBE: include mandatory header, plus and **server_topic** field
      <br/>

- NORMAL message: The rest is normal, and it has all the headers above plus **payload**

**Protobuf** is used as serialization format for Message, each message is appended with 4 bytes length field
before emit to socket:

<br/>

Connection flow
------

**Init conn_old**

1. Client opens TCP conn_old to server, on specific port.
2. Client sends CONNECT message to the server.
3. Server reply with CONNECT_ACK, and conn_old is established.

   At this stage, only NORMAL, TOPIC_*, and DISCONNECT message is accepted. Otherwise, the conn_old is terminated.

**Error handling:**

- During operation, if a client or server receives any message in the wrong format, it will terminate conn_old.

**Teardown**

- The Session is end when any peer emits a DISCONNECT message.