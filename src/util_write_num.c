/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_write_num.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:03:44 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/24 16:13:17 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "util.h"
#include <unistd.h>

static int	util_utoa_arr(unsigned int n, char *arr)
{
	int	len;

	len = 0;
	if (n >= 10)
		len += util_utoa_arr(n / 10, arr);
	arr[len] = '0' + n % 10;
	return (len + 1);
}

void	util_write_int(int fd, int n)
{
	char	arr[11];
	int		len;

	if (n < 0)
	{
		arr[0] = '-';
		len = 1 + util_utoa_arr(-(unsigned int)n, arr + 1);
	}
	else
		len = util_utoa_arr(n, arr);
	write(fd, arr, len);
}
