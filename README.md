# Final-Project-5G-AR
## ECE 4400/6400 | Clemson University
Collaborators: Zach Clements, Justin Langley, Ryan McDonnell, Adam Patyk, Andrew Stogner, Pavlo Triantafyllides

### Motivation
AR and VR demand high bandwidth and low latency to provide an immersive and seamless experience. Current networks do not meet the streaming requirements needed to provide this experience. However, emerging 5G technologies provide higher data rates, lower latency, and increased capacity than existing networks. Therefore 5G, and specifically high-band 5G, has the ability to stream AR and VR experiences in real time. One downside to high-band 5G that fulfills these needs is that it only works well over short distances. Due to this limitation, an efficient handoff/switching system is crucial in providing a seamless experience.

### Project Description
In this project, we investigate mobile-controlled handoff (MCHO) associated with high-band 5G cellular networks with a focus on an AR headset. In this situation, the AR device will determine which tower provides the best signal (lowest connection latency). This device will then control handoff between towers to maintain the best connection and cohesive AR experience.

## Implementation

Our software models the connection of an AR headset to a server through multiple nodes. We focused on monitoring each connection to determine which “tower” would give the fastest connection to the device. This was implemented through a multithreaded program that runs on the device VM. This code was based off of the client and server code in [Beej’s guide](https://beej.us/guide/bgnet/). The code was modified to create multiple socket connections using threads as well as record the round trip time (RTT) for each connection. One thread is created for each tower, and packets are sent at set intervals so that the round trip time can be measured and stored in a global variable. In the main thread, this global variable can be monitored and the best connection can be found. On the server VM, a program continuously runs that establishes socket connections for incoming messages. These connections are managed in separate processes to ensure there is no variation in response time. The continuous monitoring of RTT models the trigger for handoff between 5G towers. In order to model movement of the headset through a city, we created a script that runs on each of the towers which randomly adds delays through the Ethernet interfaces.
