/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:47 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/24 16:53:30 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "send.h"
#include "signals.h"

int	main(int argc, char *argv[])
{
	pid_t			server;
	t_send_state	send_state;
	bool			bit;

	if (argc < 3)
		return (1);
	server = atoi(argv[1]);
	send_init(&send_state, (unsigned char *)argv[2], strlen(argv[2]));
	signals_set_handler();
	while (!send_done(&send_state))
	{
		bit = send_get_bit(&send_state);
		signals_send_bit(server, bit);
		signals_wait_for_data();
	}
}
