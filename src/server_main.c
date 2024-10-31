/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:50 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/31 19:58:00 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "receive.h"
#include "signals.h"
#include "util.h"

typedef enum e_server_status
{
	MT_SERVER_LISTENING,
	MT_SERVER_SUCCESS,
	MT_SERVER_TIMEOUT,
	MT_SERVER_NEW_CLIENT,
	MT_SERVER_CHANGED_CLIENT,
	MT_SERVER_MALLOC_ERROR,
	MT_SERVER_SEND_ERROR,
	MT_NUM_SERVER_STATUS,
}	t_server_status;

static const char	*g_status_msgs[MT_NUM_SERVER_STATUS] = {
	"Server ready, PID",
	"Message received successfully from client",
	"Timeout, no response from client, dropping client",
	"Receiving message from client",
	"Signal from a different client, dropping client",
	"Memory allocation failed, dropping client",
	"Sending signal failed, dropping client",
};

/*
	This should only be called with a real PID (positive).
	Make sure buf is long enough for the longest message:
		strlen(msg) + 12 (space + positive int + newline).
*/
static void	status_msg(t_server_status status, pid_t pid)
{
	const char		*msg;
	char			buf[80];
	size_t			len;

	msg = g_status_msgs[status];
	len = 0;
	while (msg[len])
	{
		buf[len] = msg[len];
		len++;
	}
	buf[len++] = ' ';
	len += util_utoa_arr(pid, buf + len);
	buf[len++] = '\n';
	write_all(STDERR_FILENO, buf, len);
}

static bool	check_timeout(pid_t *client, bool timeout, t_receive_state *state)
{
	if (!timeout)
		return (false);
	if (*client)
	{
		status_msg(MT_SERVER_TIMEOUT, *client);
		receive_reset(state);
		signals_send_bit(*client, 1);
		*client = 0;
	}
	return (true);
}

/*
	The previous client was already sent bit 0 and will send another data bit to
	us. Wait for it and discard it before proceeding with the new client. Any
	further conflicting senders are rejected immediately. Wait may end up
	slightly shorter than expected as signals interrupt the sleep, but we can't
	do better without access to clocks, and there are bigger problems if there
	are enough conflicting signals and client slowdown that this matters.
*/
static void	check_sender(pid_t *client, pid_t sender, t_receive_state *state)
{
	t_signal_data	sig_data;
	unsigned int	wait_tries_left;

	if (sender != *client)
	{
		if (*client != 0)
		{
			status_msg(MT_SERVER_CHANGED_CLIENT, *client);
			receive_reset(state);
			wait_tries_left = WAIT_TRIES;
			while (1)
			{
				sig_data = signals_wait_for_data(wait_tries_left);
				if (sig_data.tries_left == 0 || sig_data.sender == *client)
					break ;
				wait_tries_left = sig_data.tries_left;
				signals_send_bit(sig_data.sender, 1);
			}
			signals_send_bit(*client, 1);
		}
		status_msg(MT_SERVER_NEW_CLIENT, sender);
		*client = sender;
	}
}

static void	try_receive_bit(pid_t *client, bool bit, t_receive_state *state)
{
	if (!receive_add_bit(state, bit))
	{
		receive_reset(state);
		signals_send_bit(*client, 1);
		status_msg(MT_SERVER_TIMEOUT, *client);
		*client = 0;
		return ;
	}
	if (receive_done(state))
	{
		write_all(STDOUT_FILENO, state->buf, state->len);
		status_msg(MT_SERVER_SUCCESS, *client);
		receive_reset(state);
		signals_send_bit(*client, 0);
		*client = 0;
		return ;
	}
	if (!signals_send_bit(*client, 0))
	{
		status_msg(MT_SERVER_SEND_ERROR, *client);
		receive_reset(state);
		*client = 0;
	}
}

int	main(void)
{
	t_signal_data	sig_data;
	t_receive_state	receive_state;
	pid_t			client;

	receive_init(&receive_state);
	signals_set_handler();
	status_msg(MT_SERVER_LISTENING, getpid());
	client = 0;
	while (1)
	{
		sig_data = signals_wait_for_data(WAIT_TRIES);
		if (check_timeout(&client, sig_data.tries_left == 0, &receive_state))
			continue ;
		check_sender(&client, sig_data.sender, &receive_state);
		try_receive_bit(&client, sig_data.bit, &receive_state);
	}
}
