/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 14:29:57 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/23 16:37:58 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t	g_sig_data;

/*
	`pid_t` is specified to be a signed integer type, but a valid process ID
	can't be zero or negative, so we can safely use the negative half of the
	range to differentiate two different signals.
 
	We also rely on `pid_t` fitting into `sig_atomic_t`. POSIX doesn't guarantee
	it,	but on the school Linux systems both are defined as `int`.
*/
static void	signal_handler(int sig, siginfo_t *info, void *ucontext)
{
	(void)ucontext;
	if (g_sig_data != 0)
		return ;
	if (sig == SIGUSR1)
		g_sig_data = info->si_pid;
	else if (sig == SIGUSR2)
		g_sig_data = -info->si_pid;
}

void	set_signal_handler(void)
{
	struct sigaction	act;

	act = (struct sigaction){0};
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = signal_handler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGUSR1);
	sigaddset(&act.sa_mask, SIGUSR2);
	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGUSR2, &act, NULL);
}

/*
	We can't use pause() because signal might arrive after checking `g_sig_data`
	but before calling pause(), which then sleeps indefinitely. Not sleeping at
	all would ensure the signal is noticed as quickly as possible but also waste
	a lot of CPU time doing nothing. A timed sleep reduces CPU usage while also
	ensuring the process eventually wakes up and re-checks `g_sig_data`.

	The shortest sleep we can do is usleep(1) as nanosleep is not allowed for
	the project. On school computers it works out to be 50-60 microseconds due
	to timer granularity limits or various overheads, and makes the process
	consume 17% of a CPU core at idle clocks and 5% at full speed. Longer sleep
	time could reduce CPU usage further but causes longer delays whenever the
	signal happens to arrive before the sleep call.

	There are better tools for this than an asynchronous signal handler plus
	sleep, (e.g. block the signal, then use sigsuspend or sigtimedwait to handle
	it), but the project doesn't allow the necessary functions.

	---

	Use a local variable to read the global variable only once per try. While
	our signal handler doesn't change the variable if it's already non-zero, in
	general it'd be a bad idea to check an asynchronously changing variable and
	then re-read it while expecting it to stay the same.

	Only set the global variable back to zero after reading a non-zero value,
	otherwise a signal arriving between the read and the write will get lost.
*/

sig_atomic_t	wait_for_signal_data(void)
{
	sig_atomic_t	data;

	data = g_sig_data;
	while (data == 0)
	{
		usleep(1);
		data = g_sig_data;
	}
	g_sig_data = 0;
	return (data);
}

void	send_bit(pid_t recipient, bool bit)
{
	if (bit)
		kill(recipient, SIGUSR2);
	else
		kill(recipient, SIGUSR1);
}
