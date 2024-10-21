/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:48:00 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/21 17:08:51 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_data.h"
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

void	server_data_init(t_server_data *data)
{
	data->buf = NULL;
	server_data_reset(data);
}

void	server_data_reset(t_server_data *data)
{
	free(data->buf);
	*data = (t_server_data){0};
}

void	server_data_receive(t_server_data *data, char bit)
{
	if (data->len_bit_pos < sizeof(data->len) * CHAR_BIT)
	{
		data->len = (data->len << 1) | !!bit;
		data->len_bit_pos++;
		if (data->len_bit_pos == sizeof(data->len) * CHAR_BIT)
			data->buf = malloc(data->len);
		return ;
	}
	if (data->byte_pos < data->len && data->buf)
	{
		data->buf[data->byte_pos] = (data->buf[data->byte_pos] << 1) | !!bit;
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
		server_data_reset(data);
	}
}
