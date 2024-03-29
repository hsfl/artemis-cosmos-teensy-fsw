@page Multithreading Understanding Multithreading

Although the Teensy is a powerful microcontroller, it does have limitations.
In particular, it only has a single CPU to execute instructions. Despite
this, we want the Teensy (and Artemis) to do multiple things at the same time
(known as multitasking).

To accomplish this, we use a technique known as multithreading. In
multithreading, a program (such as the FSW) creates multiple smaller programs
(called "threads") that are switched between in order to execute different
parts of a program nearly simultaneously.

The specific implementation that the FSW uses is based on the [TeensyThreads
library](https://github.com/ftrias/TeensyThreads). In the FSW, we call
threads "channels", and we switch between these channels as the program
proceeds. The main channel is the channel that is started when the Teensy is
powered on. It creates a channel for each functional part of Artemis, and
switches between these channels (and itself) based on the computing time
assigned to each channel.

@subsection Mutexes Mutual Exclusions (Mutexes)

Each channel is defined in its own file, and essentially behaves as its own
independent Arduino sketch. In theory, this is all that is needed to
implement multitasking. However, it is not this simple. Just because the
channel is the currently active one, does not mean that it has exclusive
access to all of the Teensy's hardware.

Consider this example. Channel A uses the Teensy's I2C connection to
communicate with a serial device (say, a radio). I2C is a bus-based system,
where all devices are connected to the same data bus, and it is up to the
Teensy to specify which device on the bus it wishes to communicate with. Now
Channel A initiates a connection to the radio, indicating that it will send
data to the radio to transmit. However, Channel A's computing time runs out
before it can actually send the data that it wants to transmit.

Channel B is up next, and it polls a temperature sensor over the same I2C
connection. The Teensy keeps the I2C connection active and sends a request
addressed to the sensor. However, the radio interprets the data in its
entirety to be the data to transmit, and incorrectly transmits the request.
In reality, there are other safeguards against this, but this illustrates
the problem.

This type of error is due to collisions in shared resources. These resources
could be pins on the Teensy, serial connections, shared memory, etc. To avoid
these collisions, we use mutual exclusions (mutexes).

A mutex is essentially a token that ensures that a given channel (thread) is
the only one that has access to a resource. It is "locked" when a channel
claims access to a resource, and is "unlocked" when the channel is done using
the resource. Importantly, if one channel has a mutex locked, it will retain
exclusive access to that resource even if that channel is not being actively
executed. Thus, if another channel wants to lock that mutex, it will either
fail outright or wait until the mutex is no longer locked (depending on the
specific implementation).

@subsection MultithreadingAnalogy An Analogy

To illustrate this, consider this analogy. You have multiple homework
assignments for different classes (say Calculus, Circuits, Physics and
English). You procrastinated, so they're all due at the same time and there's
no time to complete all of them. You could do each individually and in
sequence. That is, you complete all Calculus problems, then Circuits, etc.
This would allow you to complete all of the Calculus, Circuits, and Physics
problems, but you wouldn't have any time to even start the English paper.

You decide to implement a multitasking approach based on multithreading. You
set an alarm to go off every 20 minutes, and work on one assignment at a time
in that interval. If the alarm goes off, you switch to the next assignment,
even if you are in the middle of a problem.

This approach works well and you get a little bit of each assignment done.
However, you're working on a Calculus problem, using your calculator, when
the alarm goes off! The next assignment is a Circuits program that also
requires a calculator. You can't reset or clear the calculator (it has the
values you were working on for Calculus), so you skip to the next Circuits
problem, noting that you should return to the previous problem when you have
access to the calculator again.

You skip forward to Physics. It's a short assignment, so you complete it
quickly. You can take the rest of the time you allocated to Physics and idle
(do nothing). You then do some of the English paper, then return to Calculus.
You complete the problem and release the calculator. You then switch to
Physics, now able to use the calculator now that it is free for your use.

This gives a simplified view of what the Teensy's processor is doing when
using multithreading. As you can see, computers are much better at
multitasking than humans. Even so, just as a human requires time to "get up
to speed" on each transition from one task to another, a computer has a
performance penalty for switching between threads. This performance hit is
negligible for our purposes, but it does exist. For those interested in
computer architecture, the processor must flush and refill its cache when
switching between threads.
