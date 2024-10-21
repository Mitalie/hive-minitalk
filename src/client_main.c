/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:47 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/21 18:57:01 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t	g_can_send = 1;

static void	signal_handler(int sig, siginfo_t *info, void *ucontext)
{
	(void)ucontext;
	(void)info;
	if (sig == SIGUSR1)
		g_can_send = 1;
}

static void	send_len_bit(pid_t server, char *len_bit, size_t len)
{
	(*len_bit)--;
	if (len & (1l << *len_bit))
		kill(server, SIGUSR2);
	else
		kill(server, SIGUSR1);
}

static void	send_message_bit(pid_t server, char *bit, char **msg, size_t *len)
{
	(*bit)--;
	if (**msg & (1 << *bit))
		kill(server, SIGUSR2);
	else
		kill(server, SIGUSR1);
	if (*bit == 0)
	{
		*bit = CHAR_BIT;
		(*msg)++;
		(*len)--;
	}
}

static void	send_message(pid_t server, char *msg, size_t msg_len)
{
	char	len_bit;
	char	bit;

	len_bit = sizeof(msg_len) * CHAR_BIT;
	bit = CHAR_BIT;
	while (msg_len)
	{
		if (!g_can_send)
		{
			usleep(1);
			continue ;
		}
		g_can_send = 0;
		if (len_bit)
			send_len_bit(server, &len_bit, msg_len);
		else
			send_message_bit(server, &bit, &msg, &msg_len);
	}
}

int	main(int argc, char *argv[])
{
	pid_t				server;
	char				*msg;
	size_t				msg_len;
	struct sigaction	act;

	if (argc < 3)
		return (1);
	server = atoi(argv[1]);
	msg = argv[2];
	msg_len = strlen(msg);
	act = (struct sigaction){0};
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = signal_handler;
	sigaction(SIGUSR1, &act, NULL);
	send_message(server, msg, msg_len);
}
