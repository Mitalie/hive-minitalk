/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_write.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 17:43:35 by amakinen          #+#    #+#             */
/*   Updated: 2024/10/31 18:10:13 by amakinen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "util.h"
#include <errno.h>
#include <unistd.h>

/*
	Write the entire buffer into given file descriptor, retrying interrupted
	system calls and short writes. Returns true on success and false on error.
*/
bool	write_all(int fd, const void *buf, size_t len)
{
	ssize_t	ret;

	while (len)
	{
		ret = write(fd, buf, len);
		if (ret < 0 && errno == EINTR)
			continue ;
		else if (ret < 0)
			return (false);
		buf = (char *)buf + ret;
		len -= ret;
	}
	return (true);
}
