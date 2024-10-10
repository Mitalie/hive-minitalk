/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:47 by amakinen          #+#    #+#             */
/*   Updated: 2024/09/26 13:40:15 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile sig_atomic_t	g_sigusr1;

void	handler(int sig, siginfo_t *info, void *ucontext)
{
	(void)ucontext;
	if (sig == SIGUSR1 && info->si_code == SI_USER)
		g_sigusr1 = 1;
}

int	main(int argc, char *argv[])
{
	pid_t				server;
	int					counter;
	struct sigaction	act;

	if (argc < 3)
		return (1);
	server = atoi(argv[1]);
	counter = atoi(argv[2]);
	act = (struct sigaction){0};
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = handler;
	sigaction(SIGUSR1, &act, NULL);
	g_sigusr1 = 1;
	while (counter)
	{
		if (g_sigusr1)
		{
			g_sigusr1 = 0;
			counter--;
			kill(server, SIGUSR1);
		}
		else
			pause();
	}
}
