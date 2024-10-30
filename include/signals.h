/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 14:29:23 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/30 16:34:21 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# include <stdbool.h>
# include <sys/types.h>

typedef struct s_signal_data
{
	bool	timeout;
	bool	bit;
	pid_t	sender;
}	t_signal_data;

void			signals_set_handler(void);
t_signal_data	signals_wait_for_data(void);
bool			signals_send_bit(pid_t recipient, bool bit);

#endif
