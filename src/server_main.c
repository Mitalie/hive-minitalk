/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:50 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/31 17:33:21 by amakinen         ###   ########.fr       */
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

static void	status_msg(t_server_status status, pid_t pid)
{
	const char	*msg;

	msg = g_status_msgs[status];
	write(STDERR_FILENO, msg, util_strlen(msg));
	write(STDERR_FILENO, " ", 1);
	util_write_int(STDERR_FILENO, pid);
	write(STDERR_FILENO, "\n", 1);
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

static void	check_sender(pid_t *client, pid_t sender, t_receive_state *state)
{
	if (sender != *client)
	{
		if (*client != 0)
		{
			status_msg(MT_SERVER_CHANGED_CLIENT, *client);
			receive_reset(state);
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
		write(STDOUT_FILENO, state->buf, state->len);
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
		sig_data = signals_wait_for_data();
		if (check_timeout(&client, sig_data.timeout, &receive_state))
			continue ;
		check_sender(&client, sig_data.sender, &receive_state);
		try_receive_bit(&client, sig_data.bit, &receive_state);
	}
}
