/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 14:29:23 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/23 16:37:55 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# include <signal.h>
# include <stdbool.h>

void			set_signal_handler(void);
sig_atomic_t	wait_for_signal_data(void);
void			send_bit(pid_t recipient, bool bit);

#endif
