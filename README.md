# Zephyr TCP Chat Router

A simple firmware project built using Zephyr RTOS that demonstrates core embedded networking and real-time operating system concepts. The application implements a TCP-based chat router where two clients connect on separate ports and exchange messages through a centralized routing thread.

This project is intended as a beginner-friendly reference for developers starting with firmware development and Zephyr RTOS. It provides hands-on exposure to socket programming, multithreading, and inter-thread communication while maintaining a simple and easy-to-follow code structure.

## Features

* TCP server implementation using Zephyr BSD sockets
* Multi-threaded application design using Zephyr kernel threads
* Message routing between connected clients
* Inter-thread communication using message queues (`k_msgq`)
* Concurrent handling of multiple network connections
* Lightweight and easy-to-understand firmware architecture
* Suitable for learning embedded networking fundamentals

## Architecture

The firmware consists of three primary threads:

* **User A Thread** – Creates a TCP server on Port 9000 and receives messages from Client A.
* **User B Thread** – Creates a TCP server on Port 9001 and receives messages from Client B.
* **Router Thread** – Retrieves messages from a kernel message queue and forwards them to the appropriate client.

Messages received from either client are first placed into a shared message queue and then processed by the router thread, demonstrating a common producer-consumer design pattern used in embedded systems.

## Concepts Demonstrated

* Zephyr RTOS kernel threads
* BSD socket programming
* TCP client-server communication
* Message queue based IPC
* Producer-consumer architecture
* Concurrent task execution in embedded systems

## Build and Flash

```bash
west build -b <board>
west flash
```

## Purpose

The goal of this project is to provide a practical example of networking and task communication in Zephyr RTOS, making it a useful learning resource for students, embedded developers, and anyone beginning their firmware development journey.

