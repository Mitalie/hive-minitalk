# Minitalk

Assignment requires client to send a string to a server with specified PID using
SIGUSR1 and SIGUSR2. The server has to print the string and (for bonus) confirm
the reception with a signal to the client.

Signals are delivered to the receiving process asynchronously and without
queuing. Multiple signals with the same signal number might execute the handler
only once, and multiple different signals may execute their handlers in any
order. Timed delays between sending signals can make signals consistent most of
the time, but there is no guaranteed deadline to ensure the receiving process
has handled the signal. The only reliable solution requires sending a signal
back to confirm the reception.

Presence of multiple clients further complicates the matter. Server knows which
process ID sent the signal being handled, but if multiple clients send the same
signal the server's signal handler might run only once and lose the additional
signals. We can minimize the chance of this happening by only allowing one
client to send at a time and have the rest wait for permission from the server,
but each client must send at least one unsolicited signal to make the server
aware of it.

There are ways to detect a possibly lost signal - timeout on client waiting for
confirmation, timeout on server waiting for the next signal after confirming the
previous one, or detection on server when receiving a signal from a different
client than expected. None of these mean the signal is lost for certain - it
might still arrive after the detection. Therefore there needs to be a recovery
procedure that can distinguish or discard the possibly-lost signal if it arrives
after all, and ensure the client and server return to a consistent state for
normal operation.

## Approach A: bit signals with lost signal recovery

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
clients with `kill(pid, 0)`.

### Summary and states

* Client starts in HELLO state
* HELLO state
   - Periodically send SIGUSR1
   - SIGUSR1 transitions to SEND state
   - SIGUSR2 transitions to QUEUE state
* QUEUE state
   - Periodically check server is alive, exit if not
   - SIGUSR1 transitions to SEND state
* SEND state
   - Send current bit
   - SIGUSR1 moves to next bit and remaining in SEND state, sending it
   - SIGUSR2 transitions to RETRY state
* RETRY state
   - Repeat last sent bit
   - SIGUSR1 transitions to RECOVER state
   - SIGUSR2 remaining in RETRY state, repeating again
* RECOVER state
   - Send opposite of last sent bit
   - SIGUSR1 moves to next bit and transitions to SEND state, sending it
   - SIGUSR2 transitions to RETRY state, repeating the "opposite" bit

* Server starts in IDLE state
* IDLE state
   - Signal introduces a new client, marks it active, and transitions to
      RECEIVE state
* RECEIVE state:
   - Send SIGUSR1 to active client
   - Signal from active client
      - Interpret as bit
      - Remain in RECEIVE state, sending another SIGUSR1
   - Signal from other senders
      - Respond with SIGUSR2 and add new client to queue
      - Transition to CONFLICT state
* CONFLICT state:
   - Send SIGUSR2 to active client
   - Signal from active client
      - Interpret as bit
      - Transition to FLUSH state
   - Signal from other senders
      - Respond with SIGUSR2 and add new client to queue
      - Remain in CONFLICT state, sending another SIGUSR2
* FLUSH state:
   - Send SIGUSR1 to active client
   - Signal from active client
      - Ignore if same as last signal from active client
      - Transition to RECEIVE state if different from last bit (ensure no more
         pending signals)
   - Signal from other senders
      - Respond with SIGUSR2 and add new client to queue
      - Transition to CONFLICT state
* If active client is detected dead or bitstream is finished, remove the client
  and select new active client, transiton to RECEIVE state.

## Approach B: data signal and control signal

### TODO

