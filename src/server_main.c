/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:50 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/24 16:47:01 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "receive.h"
#include "signals.h"
#include "util.h"

int	main(void)
{
	t_signal_data	sig_data;
	t_receive_state	receive_state;

	util_write_int(STDOUT_FILENO, getpid());
	write(STDOUT_FILENO, "\n", 1);
	receive_init(&receive_state);
	signals_set_handler();
	while (1)
	{
		sig_data = signals_wait_for_data();
		receive_bit(&receive_state, sig_data.bit);
		if (receive_done(&receive_state))
		{
			write(STDOUT_FILENO, receive_state.buf, receive_state.len);
			receive_reset(&receive_state);
		}
		signals_send_bit(sig_data.sender, 0);
	}
}
