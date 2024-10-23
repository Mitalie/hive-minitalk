/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:40:04 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/23 16:49:40 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RECEIVE_H
# define RECEIVE_H

# include <stdbool.h>
# include <stddef.h>

typedef struct s_receive_state
{
	size_t			len;
	size_t			byte_pos;
	unsigned char	bit_pos;
	unsigned char	len_bit_pos;
	unsigned char	*buf;
}	t_receive_state;

void	receive_init(t_receive_state *data);
void	receive_reset(t_receive_state *data);
void	receive_bit(t_receive_state *data, bool bit);

#endif
