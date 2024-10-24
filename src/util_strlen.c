/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_strlen.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 17:38:59 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/24 17:41:42 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "util.h"

size_t	util_strlen(const char *str)
{
	size_t	len;

	len = 0;
	while (*str++)
		len++;
	return (len);
}
