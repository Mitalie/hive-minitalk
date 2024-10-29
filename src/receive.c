/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:48:00 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/29 14:19:16 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "receive.h"
#include <limits.h>
#include <stdlib.h>

void	receive_init(t_receive_state *state)
{
	state->buf = NULL;
	receive_reset(state);
}

void	receive_reset(t_receive_state *state)
{
	free(state->buf);
	state->buf = NULL;
	state->len = 0;
	state->len_bit_remaining = sizeof(state->len) * CHAR_BIT;
	state->byte_idx = 0;
	state->byte_bit_remaining = CHAR_BIT;
}

/*
	Returns true on success, false on malloc error.
*/
bool	receive_add_bit(t_receive_state *state, bool bit)
{
	if (state->len_bit_remaining > 0)
	{
		state->len = (state->len << 1) | bit;
		state->len_bit_remaining--;
		if (state->len_bit_remaining == 0)
		{
			state->buf = malloc(state->len);
			return (state->buf != NULL);
		}
	}
	else if (state->byte_idx < state->len)
	{
		if (state->buf)
			state->buf[state->byte_idx]
				= (state->buf[state->byte_idx] << 1) | bit;
		state->byte_bit_remaining--;
		if (state->byte_bit_remaining == 0)
		{
			state->byte_idx++;
			state->byte_bit_remaining = CHAR_BIT;
		}
	}
	return (true);
}

bool	receive_done(t_receive_state *state)
{
	return (state->len_bit_remaining == 0
		&& state->byte_idx == state->len);
}
