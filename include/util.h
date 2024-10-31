/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:04:01 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/31 18:07:33 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTIL_H
# define UTIL_H

# include <stdbool.h>
# include <stddef.h>

bool	util_parse_int(const char *str, int *value_out);
size_t	util_strlen(const char *str);
int		util_utoa_arr(unsigned int n, char *arr);
bool	write_all(int fd, const void *buf, size_t len);

#endif
