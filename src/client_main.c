/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:39:47 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/24 17:49:19 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include "send.h"
#include "signals.h"
#include "util.h"

int	main(int argc, char *argv[])
{
	pid_t			server;
	t_send_state	send_state;
	bool			bit;

	if (argc < 3)
		return (1);
	if (!util_parse_int(argv[1], &server))
		return (2);
	send_init(&send_state, (unsigned char *)argv[2], util_strlen(argv[2]));
	signals_set_handler();
	while (!send_done(&send_state))
	{
		bit = send_get_bit(&send_state);
		signals_send_bit(server, bit);
		signals_wait_for_data();
	}
}
