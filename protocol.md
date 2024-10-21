# Minitalk

Assignment requires a client program to send a string to a server process with
specified PID using Unix signals SIGUSR1 and SIGUSR2. The server has to print
the string and (for bonus) confirm the reception with a signal to the client.

## Signal considerations

### Signal handler safety

Signal handlers interrupt the normal flow of execution to run the handler code.
This can happen *at any time*, including in the middle of a library function, in
the middle of access to complex variable, and even during another signal handler
if the new signal is not blocked. Signal handlers must be written carefully to
avoid corrupting the program state by accessing resources at the wrong time.

POSIX specifies undefined behavior when calling library functions other than
those listed as async-signal-safe or when referring to any global or static
objects except `errno` and writes to objects of type `volatile sig_atomic_t`.
Non-exotic implementations usually allow more freedom with functions when no
unsafe function is being interrupted, and don't restrict memory access, but
leave it to the programmer to ensure safety and consistency.

### Signal reliability with rapid sending

External signals are delivered to the receiving process asynchronously and
without queuing. Multiple signals with the same signal number might execute the
handler only once, and multiple different signals may execute their handlers in
any order. Timed delays between sent signals can make delivery seem consistent,
but there is no guaranteed deadline to ensure the receiving process has handled
the signal - it could block inside the signal handler, or be in uninterruptible
sleep, or simply have to wait for CPU time in a busy system. The only reliable
method is waiting for confirmation or permission before sending the next signal.

### Signal reliability with multiple senders

Presence of multiple senders further complicates the matter. The receiving
process can check the process ID that sent the signal, but if different senders
send the same signal the signal handler might run only once and not know about
the other senders. We can minimize the chance of this happening by only allowing
one client to send at a time and have the rest wait for permission from the
server, but each client must send at least one unsolicited signal to make the
server aware of it.

There are ways to detect a possibly lost signal - timeout on the client waiting
for a confirmation, timeout on the server waiting for a signal after requesting
one, or assuming loss when receiving a signal from a different sender than
expected. None of these mean the signal is lost for certain - it might arrive
and be handled after the detection was triggered. There needs to be a recovery
process that can distinguish or discard the possibly-lost signal if it actually
arrives and ensure client and server return to a consistent state for normal
operation.

## Restrictions

The assignment specifies that each program can use only one global variable, and
for strict POSIX conformance it must be of type `volatile sig_atomic_t`.  POSIX
allows `sig_atomic_t` to be smaller than `pid_t`, making it impossible to
communicate the process ID of the signal sender out of the signal handler with a
single variable unless an array is acceptable. This means we have to respond to
each signal within the handler. However, we can't process the incoming data in
the handler due to not being able to read a buffer with the previously received
bits, and must do that in the normal flow of execution. This creates a timing
window where the client sends next signal and the handler runs again before the
normal flow of execution can process the previous one. We have to loosen at
least one restriction to make a reliable solution for even a single client:

* Use an array of `volatile sig_atomic_t` to communicate signal info to normal
  flow of execution, making it possible to handle received data before replying.
* Instead of strict POSIX, assume PID can be stored in `sig_atomic_t` and use
  the sign bit to distinguish SIGUSR1 and SIGUSR2, achieving same as above.
* Rely on potentially unreliable timed sending to transmit the client PID in the
  datastream, allowing the server's normal code to learn it without
  communicating it out of the signal handler.
* Instead of strict POSIX, assume an implementation that doesn't restrict memory
  access in signal handlers, allowing us to either process data in the handler
  or communicate all the necessary information out of it.

Processing data entirely within the signal handler requires memory allocation,
which is unsafe in a complex program as it risks corrupting the allocator's
static data structures. In this project the server does nothing besides handling
signals so we could ensure that the signal handler doesn't interrupt allocator
functions, but passing data out of the handler is more generally applicable.

## Minimal solution

Simultaneous clients or external disturbances are not taken into account, so
SIGUSR1 and SIGUSR2 can be considered reliable as long as client waits for
confirmation. Server's signal handlers will store the PID of the sender in a
global variable, negated if the signal was SIGUSR2. Server's normal code reads
the variable, and if non-zero, resets it to zero, interprets the received signal
as a bit, and replies to the sender with SIGUSR1. Server sleeps for a short time
and repeats checking the variable.

To avoid ugly reallocs on the server as more of the message is received, the
bitstream begins with the message length as `size_t`, most significant bit
first. After receiving the length, the server allocates a buffer for the
message. The length is followed by the message contents as a sequence of `char`,
most significant bit first. No null terminator is included. After the entire
message is received, server prints it, releases the buffer, and is ready to
receive another length-prefixed message, possibly from a different client.

## Additional ideas

These ideas might be useful for supporting multiple simultaneous clients, or for
a more flexible data transfer protocol than single message of known length, but
they aren't fully applicable within the limits of the assignment.

<details>
<summary>Signal protocol, approach A</summary>

## Signal protocol, approach A: bit signals with lost signal recovery

Having two signals available naturally suggests sending one or the other
depending on the bit to be sent, e.g. SIGUSR1 for bit 0 and SIGUSR2 for bit 1.
For normal operation, the server requests next bit from the client with SIGUSR1.
If the server receives a signal from a different client, it informs the expected
client of a possibly lost signal with SIGUSR2. This puts the two processes in
one of several situations:

1. The client has sent the bit signal, but it was lost and will never arrive.
2. The client has sent the bit signal, and it will arrive eventually.
3. The client has not sent the bit signal by the time it handles the SIGUSR2.

Case 3 is transformed into either case 1 or case 2 by choosing to cancel or not
cancel the bit signal when client receives the SIGUSR2. As in the single-client
case, simply waiting for a time can not distinguish beetween cases 1 and 2, as
the signal could eventually arrive after an arbitrarily long wait. Therefore the
client must send another signal to avoid getting stuck in case 1. This leads us
to another set of cases:

1. Only one signal is delivered, because either the bit signal was lost, or the
   bit signal was still pending and the matching recovery signal gets lost.
2. Bit signal is delivered, followed by the recovery signal.
3. Both signals become pending and are delivered in unspecified order.

Case 3 can be excluded by having the client send a repeat of the bit signal as
the recovery signal. Once the server receives one signal, it knows which bit the
potentially-lost bit was, but it doesn't know if a repeat signal is still in
flight. This can be solved by requiring the client send the opposite signal and
ignoring the repeat if any arrives. This gives is the following cases:

1. Repeat signal is delivered first or not at all, followed by the opposite
   signal.
2. Both signals become pending and are delivered in unspecified order.

Case 2 can be collapsed onto case 1 by not having the server proceed inside the
signal handler but only in the normal execution flow after the signals have been
handled. Once the opposite signal has been handled and normal execution resumes,
no signals can be in flight and the server can request the next bit.

The recovery signals towards server (repeat signal and opposite signal) can also
be lost if the server receives additional signals from unexpected clients. If a
conflicting signal arrives while the server is waiting for a repeat, the server
can simply request additional repeats. This increases the number of signals in
flight, but as long as one of them arrives the server can ignore the rest and
wait for the opposite signal. If a conflicting signal arrives while server is
waiting for the opposite signal, server can ask for another recovery, but since
there may now be multiple opposite signals in flight, the recovery needs to be
finished with opposite of those.

If both signals are unreliable, the above can lead into a situation where no
progress can be made even though some signals are received from the expected
client, because conflicting signals arriving in the second half of recovery
process effectively restart the recovery. We can avoid this by requiring that
new clients always introduce themselves with the same signal, e.g. SIGUSR1. That
way, SIGUSR2 should never conflict. SIGUSR2 bit signals don't need recovery in
the first place, and SIGUSR1 recovery can be finished with reliable SIGUSR2 once
at least one SIGUSR1 makes it through.

The introduction signal can also be lost. Server doesn't know about new clients
yet, so it can't request a retransmit. Instead, the server can confirm the
initial signal with SIGUSR2, telling the client to wait for SIGUSR1. If the
confirmation doesn't arrive, client can retry after a timeout. When confirmation
is received, client simply waits for permission to start. It can periodically
check with `kill(pid, 0)` that the server hasn't been terminated.

This doesn't leave the client a way to tell the server that it is finished.
That needs to be communicated within the bitstream. Server can check for dead
clients with `kill(pid, 0)` if no signal is received for a time. Frozen clients
that don't transmit can be dropped after a very long timeout - long enough that
slow but functional clients aren't affected.

### State machines

#### Client

* HELLO state (start)
  * Periodically send SIGUSR1, exit if server not alive
  * SIGUSR1: transition to SEND
  * SIGUSR2: transition to QUEUE
* QUEUE state
  * Periodically check if server is alive, exit if not
  * SIGUSR1: transition to START
  * SIGUSR2: ignore
* START state
  * Send SIGUSR2, wait for signal
  * SIGUSR1: transition to SEND
  * SIGUSR2: transition to RETRY
* SEND state
  * Send next bit, wait for signal, repeat
  * SIGUSR1: ignore
  * SIGUSR2: transition to RETRY
* RETRY state
  * Resend last bit, wait for signal, repeat
  * SIGUSR1: transition to RECOVER
  * SIGUSR2: ignore
* RECOVER state
  * Send opposite of last bit, wait for signal
  * SIGUSR1: transition to SEND
  * SIGUSR2: transition to RETRY

Server normally doesn't send SIGUSR2 to a client in QUEUE state, but it can
happen if server sent SIGUSR1 and then detected conflict before the client
handled it.

#### Server

* IDLE state (start)
  * Wait for signal
  * SIGUSR1: set sender as active client, transition to FLUSH1
  * SIGUSR2: ignore
* RECEIVE state
  * Send SIGUSR1 to active client, wait for signal, repeat
  * SIGUSR1/2 from active client: interpret as data bit
* CONFLICT state
  * Send SIGUSR2 to active client, wait for signal, repeat
  * SIGUSR1 from active client: interpret as data bit, transition to FLUSH1
  * SIGUSR2 from active client: interpret as data bit, transition to FLUSH2
* FLUSH1 state
  * Send SIGUSR1 to active client, wait for signal
  * SIGUSR1 from active client: ignore
  * SIGUSR2 from active client: transition to RECEIVE
* FLUSH2 state
  * Send SIGUSR1 to active client, wait for signal
  * SIGUSR1 from active client: transition to RECEIVE
  * SIGUSR2 from active client: ignore
* Signal from sender other than active client, all states except IDLE
  * SIGUSR1: reply with SIGUSR2, add client to queue, transition to CONFLICT
  * SIGUSR2: transition to CONFLICT
* If active client is not alive or bitstream is finished, select a new active
  client from queue, transition to FLUSH1. If queue is empty, transition to
  IDLE.

The client may have sent multiple SIGUSR1 before it receives the first reply
from the server. To ensure these signals aren't confused for data bits, the
server will start in FLUSH1 state and client will send SIGUSR2 before the first
data bit.

SIGUSR2 from any other sender than the active client can't be a valid client for
this protocol so the sender is ignored. Active client is still informed of
possible lost signal and goes through recovery procedure.

</details>

<details>
<summary>Signal protocol, approach B</summary>

## Signal protocol, approach B: data signal and control signal

With the restriction that new clients introduce themselves with SIGUSR1, making
SIGUSR2 reliable, we can come up with an alternative approach that avoids the
complex recovery state machine. We can communicate bits (and optionally other
symbols like end-of-transmission) by sending different numbers of SIGUSR2,
delimited by SIGUSR1. Just like approach A, each signal sent by the client must
be confirmed by the server with SIGUSR1 before the client can send the next one,
and the hello signal may be confirmed with SIGUSR2 to tell the client that it is
placed in a queue until the server is free and requests start of transmission
with SIGUSR1.

If the server receives SIGUSR1 from another client, it informs the active client
of potential conflict with SIGUSR2. If the client is waiting for confirmation of
SIGUSR1, it will re-send SIGUSR1 just in case the previous one got lost.
Otherwise, the client doesn't need to do anything.

If the server detected potential conflict, there may now be duplicate SIGUSR1
incoming from the client. As soon as one of them arrives, server confirms the
reception to the client. Any duplicates without intervening SIGUSR2 must then be
ignored. The client can send SIGUSR2 as soon as it receives the confirmation,
even though there are still pending SIGUSR1 on the server. Because order of
handling multiple pending signals is unspecified, the server must ignore any
additional SIGUSR1 and not confirm the received SIGUSR2 until all pending
signals are handled and normal execution continues.

This approach is much simpler, but downside is that it needs 2.5 signals per bit
(1 or 2 SIGUSR2, delimited by 1 SIGUSR1) even when there are no conflicts, while
approach A only needs 1 signal per bit if no conflict/recovery is triggered.
Sending two bits at a time with 1-4 SIGUSR2 could bring the average down to 3.5
signals per two bits or 1.75 signals per bit, but longer groups offer no further
improvement (three bits: 5.5 s / 3 b = 1.8333 s/b).

</details>

<details>
<summary>Bitstream protocol</summary>

## Bitstream protocol

The signal protocols above can transmit a sequence of bits. The data we want to
transfer from is a sequence of bytes, which we must break down into individual
bits and reconstruct again on the server. Signal protocol A also requires an
in-band end-of-transmission marker.

The actual requirement of the assignment is passing a single message which the
client knows in its entirety before sending, and which the server should receive
in its entirety before outputting. The message is passed to the client as
argument, which means it can't contain embedded null characters. We can simply
send each byte of the message in a specific bit order, and the null character at
the end of the string can mark end of transmission.

It is also possible to design a much more interesting protocol. Sending length
before a block of data makes the protocol agnostic to embedded null characters.
Simplest way to send length is to send fixed number of bits in a specific bit
order, with N bits allowing for maximum message length of 2^N (or 2^N-1 if zero
length is allowed). A variable-length encoding could be used to avoid imposing a
maximum message length. Because we transport a bitstream, the encoding itself is
not limited to byte-sized units.

The protocol also doesn't need to implicitly end transmission after one block.
By prepeding each block with a control symbol, we can transmit multiple blocks
one after another, reducing or removing the need for arbitrarily large blocks.
This could allow the system to handle potentially infinite streams of data
without needing to see the entire message first, e.g. transporting from stdin of
client to stdout of server. Separate control symbol is defined for closing the
connection, and another can be defined for yielding to other clients and
resuming later.

With signal protocol B, control symbols can be defined on that level instead,
although only a limited number is reasonable as each additional symbol needs
linearly more signals to transmit due to being limited to SIGUSR2 only. Symbols
at the bitstream level can use both 0 and 1 bits, so their length grows
logarithmically.

</details>
