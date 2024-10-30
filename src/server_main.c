/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:50 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/30 17:39:09 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "receive.h"
#include "signals.h"
#include "util.h"

static bool	check_timeout(pid_t *client, bool timeout, t_receive_state *state)
{
	if (!timeout)
		return (false);
	if (*client)
	{
		receive_reset(state);
		signals_send_bit(*client, 1);
		*client = 0;
	}
	return (true);
}

static void	check_sender(pid_t *client, pid_t sender, t_receive_state *state)
{
	if (*client != 0 && sender != *client)
	{
		receive_reset(state);
		signals_send_bit(*client, 1);
	}
	*client = sender;
}

static void	try_receive_bit(pid_t *client, bool bit, t_receive_state *state)
{
	if (!receive_add_bit(state, bit))
	{
		receive_reset(state);
		signals_send_bit(*client, 1);
		*client = 0;
		return ;
	}
	if (receive_done(state))
	{
		write(STDOUT_FILENO, state->buf, state->len);
		receive_reset(state);
		signals_send_bit(*client, 0);
		*client = 0;
		return ;
	}
	if (!signals_send_bit(*client, 0))
	{
		receive_reset(state);
		*client = 0;
	}
}

int	main(void)
{
	t_signal_data	sig_data;
	t_receive_state	receive_state;
	pid_t			client;

	util_write_int(STDOUT_FILENO, getpid());
	write(STDOUT_FILENO, "\n", 1);
	receive_init(&receive_state);
	signals_set_handler();
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
