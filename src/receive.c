/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:48:00 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/23 16:50:09 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "receive.h"
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

void	receive_init(t_receive_state *data)
{
	data->buf = NULL;
	receive_reset(data);
}

void	receive_reset(t_receive_state *data)
{
	free(data->buf);
	*data = (t_receive_state){0};
}

void	receive_bit(t_receive_state *data, bool bit)
{
	if (data->len_bit_pos < sizeof(data->len) * CHAR_BIT)
	{
		data->len = (data->len << 1) | bit;
		data->len_bit_pos++;
		if (data->len_bit_pos == sizeof(data->len) * CHAR_BIT)
			data->buf = malloc(data->len);
		return ;
	}
	if (data->byte_pos < data->len && data->buf)
	{
		data->buf[data->byte_pos] = (data->buf[data->byte_pos] << 1) | bit;
		data->bit_pos++;
		if (data->bit_pos == CHAR_BIT)
		{
			data->byte_pos++;
			data->bit_pos = 0;
		}
	}
	if (data->byte_pos == data->len)
	{
		write(STDOUT_FILENO, data->buf, data->len);
		receive_reset(data);
	}
}
