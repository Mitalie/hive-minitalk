/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:37:42 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/24 17:25:18 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "send.h"
#include <limits.h>

void	send_init(t_send_state *state, const unsigned char *buf, size_t len)
{
	state->buf = buf;
	state->len = len;
	state->len_bit_remaining = sizeof(state->len) * CHAR_BIT;
	state->byte_idx = 0;
	state->byte_bit_remaining = CHAR_BIT;
}

bool	send_get_bit(t_send_state *state)
{
	bool	bit;

	bit = 0;
	if (state->len_bit_remaining > 0)
	{
		state->len_bit_remaining--;
		bit = (state->len >> state->len_bit_remaining) & 1;
	}
	else if (state->byte_idx < state->len)
	{
		state->byte_bit_remaining--;
		bit = (state->buf[state->byte_idx] >> state->byte_bit_remaining) & 1;
		if (state->byte_bit_remaining == 0)
		{
			state->byte_idx++;
			state->byte_bit_remaining = CHAR_BIT;
		}
	}
	return (bit);
}

bool	send_done(t_send_state *state)
{
	return (state->len_bit_remaining == 0
		&& state->byte_idx == state->len);
}
