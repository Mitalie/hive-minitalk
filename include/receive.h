/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:40:04 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/23 14:33:49 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RECEIVE_H
# define RECEIVE_H

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
