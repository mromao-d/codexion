/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mromao-s <mromao-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 13:07:59 by mromao-s          #+#    #+#             */
/*   Updated: 2026/10/22 13:07:59 by mromao-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/codexion.h"

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] && s2[i])
	{
		if (s1[i] != s2[i])
			break ;
		i++;
	}
	return (s1[i] - s2[i]);
}

time_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday() error\n", 22);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

int	is_dead(t_props *props)
{
	int	dead;

	pthread_mutex_lock(&props->scheduler_mutex);
	dead = props->dead;
	pthread_mutex_unlock(&props->scheduler_mutex);
	return (dead);
}

void	print_status(t_coders *coder, char *msg)
{
	pthread_mutex_lock(&coder->props->scheduler_mutex);
	if (!coder->props->dead)
	{
		pthread_mutex_lock(&coder->props->print);
		printf("%ld %d %s\n", get_current_time() - coder->props->start_time,
			coder->coder_id, msg);
		pthread_mutex_unlock(&coder->props->print);
	}
	pthread_mutex_unlock(&coder->props->scheduler_mutex);
}
