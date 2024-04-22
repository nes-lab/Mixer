# Mixer

Mixer is a fast and reliable communication primitive for broadcasting data in dynamic wireless mesh networks.
It efficiently supports the spectrum from one-to-all to all-to-all traffic by integrating random linear network coding (RLNC) with synchronous transmissions in a smart way.
As a result, Mixer approaches the order-optimal scaling in the number of required communication slots as the number of messages to be exchanged increases.

With these qualities, Mixer is well suited for a variety of emerging applications in the area of Cyber-Physical Systems (CPS) and the Industrial Internet of Things (IIoT) that rely on a fast and reliable many-to-all exchange of messages across dynamic wireless mesh networks.
For example, while distributed closed-loop control and multi-agent coordination require fast feedback to keep up with the dynamics of physical processes, the installation of sensors, actuators, and controllers on rotating, flying, or otherwise mobile entities may cause rapidly changing network topologies.
Mixer addresses this challenging combination of application requirements through a novel, elegant design, and efficient implementation.

To learn more about Mixer, please check out https://mixer.nes-lab.org


## Code Layout

`/src/mixer` the source code of Mixer

`/src/gpi` generic platform interface (internally used by Mixer)

`/tutorial` tutorial projects


## Building and Running Mixer

Mixer encapsulates a number of critical timing constraints and exploits hardware-specific features for efficiency.
As a consequence, it contains a certain amount of low-level code and requires platform-specific adaptions.
Right now we support TelosB-compliant devices such as the Tmote Sky as well as Nordic Semiconductor's nRF52840 DK development boards powered by the ARM Cortex-M4 based nRF52840.

As a starting point, we provide a tutorial project for each supported platform that includes a simple demo application.
After installing the corresponding toolchain, it runs out-of-the-box and provides a quick understanding of the main functions and settings needed to use Mixer.
See the corresponding readme files ([TelosB](tutorial/tmote/README.md), [nRF52840](tutorial/nRF52840/README.md)) for more information.


## Optional Features

### Weak Zeroes

The distribution of messages in Mixer (i.e., which node will send a message) is statically defined in mixer_config.h (```payload_distribution``` array). Therefore, all nodes know who will send a message, or in other words, from which node to expect a message during a Mixer round. When a node receives all messages, it starts a smart shutdown behavior and turns off soon to save energy. However, when nodes don't want to send their message (e.g., because there is no new measurement), the other nodes in the network will never go into smart shutdown as this one expected message is missing. As a result, the nodes will transmit and receive until the hard deadline of the Mixer round (```MX_ROUND_LENGTH```) is reached, thus wasting energy.

The "weak zero" feature addresses this situation and allows nodes to mark their message(s) as unused. This information is propagated through the network without packet overhead (the information alternates with other information in the packet header). With this, nodes know which statically allocated messages will not be sent and can go into smart shutdown as soon as possible, saving valuable energy.

Weak zeroes are activated by
```c
// File: mixer_config.h
#define MX_WEAK_ZEROS		1
```

In addition, a ```WEAK_RELEASE_SLOT``` can be defined to delay the processing of weak zero information. This can be useful in case a node changes its mind; for example, the node initially marked its message as unused but wants to use it after the start of the Mixer round. Nodes could also use weak messages allocated to other nodes, however, one has to ensure no conflicting contents are transmitted!

### Warmstart

Mixer supports dynamic network topologies and merely keeps a short history of recently received nodes (neighbors) to make better transmit decisions. This neighborhood information is reset at the beginning of each round, so each node starts each Mixer round from scratch. However, suppose the topology does not really change between two Mixer rounds (e.g., when Mixer rounds are started back to back). In that case, the neighborhood information are likely valid and could be used to improve Mixer's startup behavior.

The warmstart feature realizes this and allows to carry over neighborhood information from one round to another. It can be activated with  
```c
// File: mixer_config.h
#define MX_WARMSTART		1
```
Moreover, ```WARMSTART_HISTORY``` allows to define how long neighborhood information are valid. For example, a value of 5 will keep a received node in the list of neighbors over 5 rounds. Every packet reception of this node will refresh the counter.

## Research

Mixer has been developed in the [Networked Embedded Systems Lab](https://nes-lab.org) at [TU Dresden](https://tu-dresden.de). Please visit https://mixer.nes-lab.org to find publications and presentation slides containing details about the concepts behind Mixer, its design, and excellent performance in real-world experiments.
