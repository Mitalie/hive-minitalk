/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:40:04 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/21 17:00:25 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_DATA_H
# define SERVER_DATA_H

# include <stddef.h>

typedef struct s_server_data
{
	size_t			len;
	size_t			byte_pos;
	unsigned char	bit_pos;
	unsigned char	len_bit_pos;
	unsigned char	*buf;
}	t_server_data;

void	server_data_init(t_server_data *data);
void	server_data_reset(t_server_data *data);
void	server_data_receive(t_server_data *data, char bit);

#endif
