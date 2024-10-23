/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:47 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/23 16:38:17 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "signals.h"

static void	send_len_bit(pid_t server, char *len_bit, size_t len)
{
	(*len_bit)--;
	if (len & (1l << *len_bit))
		send_bit(server, 1);
	else
		send_bit(server, 0);
}

static void	send_message_bit(pid_t server, char *bit, char **msg, size_t *len)
{
	(*bit)--;
	if (**msg & (1 << *bit))
		send_bit(server, 1);
	else
		send_bit(server, 0);
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

	set_signal_handler();
	len_bit = sizeof(msg_len) * CHAR_BIT;
	bit = CHAR_BIT;
	while (msg_len)
	{
		if (len_bit)
			send_len_bit(server, &len_bit, msg_len);
		else
			send_message_bit(server, &bit, &msg, &msg_len);
		wait_for_signal_data();
	}
}

int	main(int argc, char *argv[])
{
	pid_t				server;
	char				*msg;
	size_t				msg_len;

	if (argc < 3)
		return (1);
	server = atoi(argv[1]);
	msg = argv[2];
	msg_len = strlen(msg);
	send_message(server, msg, msg_len);
}
