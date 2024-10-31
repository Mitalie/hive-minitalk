/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 14:29:23 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/31 19:59:26 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# include <stdbool.h>
# include <sys/types.h>

typedef struct s_signal_data
{
	unsigned int	tries_left;
	bool			bit;
	pid_t			sender;
}	t_signal_data;

# define WAIT_TRIES 2000

void			signals_set_handler(void);
t_signal_data	signals_wait_for_data(unsigned int tries);
bool			signals_send_bit(pid_t recipient, bool bit);

#endif
