/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:40:04 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/29 14:18:34 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RECEIVE_H
# define RECEIVE_H

# include <stdbool.h>
# include <stddef.h>

typedef struct s_receive_state
{
	size_t			len;
	size_t			byte_idx;
	unsigned char	byte_bit_remaining;
	unsigned char	len_bit_remaining;
	unsigned char	*buf;
}	t_receive_state;

void	receive_init(t_receive_state *state);
void	receive_reset(t_receive_state *state);
bool	receive_add_bit(t_receive_state *state, bool bit);
bool	receive_done(t_receive_state *state);

#endif
