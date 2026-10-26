/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mromao-s <mromao-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 12:55:52 by mromao-s          #+#    #+#             */
/*   Updated: 2026/10/22 12:55:52 by mromao-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/codexion.h"

void	map_dongles(t_props *props)
{
	int	i;

	i = -1;
	while (++i < props->nb_of_coders)
	{
		if (pthread_mutex_init(&props->dongles[i].dongle, 0) != 0)
		{
			printf("Error initing mutex on map dongles\n");
			return ;
		}
		props->dongles[i].dongle_id = i;
		props->dongles[i].is_free = 1;
	}
	return ;
}
