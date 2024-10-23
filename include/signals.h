/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 14:29:23 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/23 16:44:31 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# include <stdbool.h>
# include <sys/types.h>

typedef struct s_signal_data
{
	bool	bit;
	pid_t	sender;
}	t_signal_data;

void			set_signal_handler(void);
t_signal_data	wait_for_signal_data(void);
void			send_bit(pid_t recipient, bool bit);

#endif
