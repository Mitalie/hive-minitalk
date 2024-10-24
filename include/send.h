/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:16:27 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/24 16:53:30 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SEND_H
# define SEND_H

# include <stdbool.h>
# include <stddef.h>

typedef struct s_send_state
{
	size_t				len;
	size_t				byte_idx;
	unsigned char		bit_idx;
	unsigned char		len_bit_remaining;
	const unsigned char	*buf;
}	t_send_state;

void	send_init(t_send_state *state, const unsigned char *buf, size_t len);
bool	send_get_bit(t_send_state *state);
bool	send_done(t_send_state *state);

#endif
