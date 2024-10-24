/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_parse_int.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 17:41:59 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/24 17:48:12 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "util.h"
#include <limits.h>

static bool	util_isspace(char c)
{
	return (c == '\t' || c == '\n' || c == '\v'
		|| c == '\f' || c == '\r' || c == ' ');
}

static bool	util_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

bool	util_parse_int(const char *str, int *value_out)
{
	int	sign;
	int	digit;
	int	value;

	while (util_isspace(*str))
		str++;
	sign = 1;
	if (*str == '-')
		sign = -1;
	if (*str == '-' || *str == '+')
		str++;
	value = 0;
	while (util_isdigit(*str))
	{
		digit = *str++ - '0';
		if ((sign > 0 && value > (INT_MAX - digit) / 10)
			|| (sign < 0 && value < (INT_MIN + digit) / 10))
			return (false);
		value = value * 10 + sign * digit;
	}
	while (util_isspace(*str))
		str++;
	if (*str == 0 && value_out)
		*value_out = value;
	return (*str == 0);
}
