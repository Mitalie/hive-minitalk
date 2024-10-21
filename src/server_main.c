/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:50 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/21 18:29:40 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "server_data.h"

volatile sig_atomic_t	g_sig_data;

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

static void	set_signal_handler(void)
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

int	main(void)
{
	sig_atomic_t	sig_data;
	t_server_data	server_data;

	printf("%d\n", getpid());
	server_data_init(&server_data);
	set_signal_handler();
	while (1)
	{
		sig_data = g_sig_data;
		if (sig_data > 0)
		{
			g_sig_data = 0;
			server_data_receive(&server_data, 0);
			kill(sig_data, SIGUSR1);
		}
		if (sig_data < 0)
		{
			g_sig_data = 0;
			server_data_receive(&server_data, 1);
			kill(-sig_data, SIGUSR1);
		}
		usleep(1);
	}
}
