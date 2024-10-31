/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:47 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/31 17:27:20 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <unistd.h>
#include "send.h"
#include "signals.h"
#include "util.h"

typedef enum e_client_status
{
	MT_CLIENT_SUCCESS,
	MT_CLIENT_ARGC,
	MT_CLIENT_INVALID_PID,
	MT_CLIENT_SEND_ERROR,
	MT_CLIENT_TIMEOUT,
	MT_CLIENT_UNEXPECTED_SIGNAL,
	MT_CLIENT_SERVER_ERROR,
	MT_NUM_CLIENT_STATUS,
}	t_client_status;

static const char	*g_status_msgs[MT_NUM_CLIENT_STATUS] = {
	"Message delivered successfully",
	"Expected 2 arguments, usage: ./client server_pid message",
	"Invalid server PID",
	"Failed to send signal to server",
	"Timeout, no response from server",
	"Unexpected SIGUSR1/2, not from server",
	"Server reported error",
};

static int	status_msg(t_client_status status)
{
	const char	*msg;

	msg = g_status_msgs[status];
	write(STDERR_FILENO, msg, util_strlen(msg));
	write(STDERR_FILENO, "\n", 1);
	return (status);
}

int	main(int argc, char *argv[])
{
	pid_t			server;
	t_send_state	send_state;
	t_signal_data	signal_data;
	bool			bit;

	if (argc != 3)
		return (status_msg(MT_CLIENT_ARGC));
	if (!util_parse_int(argv[1], &server) || server <= 0)
		return (status_msg(MT_CLIENT_INVALID_PID));
	send_init(&send_state, (unsigned char *)argv[2], util_strlen(argv[2]));
	signals_set_handler();
	while (!send_done(&send_state))
	{
		bit = send_get_bit(&send_state);
		if (!signals_send_bit(server, bit))
			return (status_msg(MT_CLIENT_SEND_ERROR));
		signal_data = signals_wait_for_data();
		if (signal_data.timeout)
			return (status_msg(MT_CLIENT_TIMEOUT));
		if (signal_data.sender != server)
			return (status_msg(MT_CLIENT_UNEXPECTED_SIGNAL));
		if (signal_data.bit == 1)
			return (status_msg(MT_CLIENT_SERVER_ERROR));
	}
	return (status_msg(MT_CLIENT_SUCCESS));
}
