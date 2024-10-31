/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_num_to_str.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:03:44 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/31 17:45:06 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "util.h"

int	util_utoa_arr(unsigned int n, char *arr)
{
	int	len;

	len = 0;
	if (n >= 10)
		len += util_utoa_arr(n / 10, arr);
	arr[len] = '0' + n % 10;
	return (len + 1);
}
