/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:50 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/23 16:49:40 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include "receive.h"
#include "signals.h"

int	main(void)
{
	t_signal_data	sig_data;
	t_receive_state	receive_state;

	printf("%d\n", getpid());
	receive_init(&receive_state);
	set_signal_handler();
	while (1)
	{
		sig_data = wait_for_signal_data();
		receive_bit(&receive_state, sig_data.bit);
		send_bit(sig_data.sender, 0);
	}
}
