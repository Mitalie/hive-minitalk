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

### TODO: what if the repeat or opposite signal is lost?

## Approach B: data signal and control signal

### TODO

