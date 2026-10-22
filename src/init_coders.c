/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mromao-s <mromao-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 12:55:06 by mromao-s          #+#    #+#             */
/*   Updated: 2026/10/22 12:55:06 by mromao-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/codexion.h"

void	join_coders(t_props *props)
{
	int	i;

	i = -1;
	while (++i < props->nb_of_coders)
		pthread_join(props->coders[i].coder, NULL);
}

int	map_coders(t_props *props)
{
	int	i;
	int	j;

	i = 0;
	while (i < props->nb_of_coders)
	{
		if (props->nb_of_coders == 1)
			props->coders[i].dongles = malloc(sizeof(t_dongles *));
		else
			props->coders[i].dongles = malloc(sizeof(t_dongles *) * 2);
		if (!props->coders[i].dongles)
		{
			printf("Malloc error on props->coders[i].dongles\n");
			j = i;
			while (j < props->nb_of_coders)
			{
				props->coders[j].dongles = NULL;
				j++;
			}
			return (1);
		}
		props->coders[i].coder_id = i + 1;
		props->coders[i].props = props;
		props->coders[i].nb_comp = 0;
		props->coders[i].arrival_seq = 0;
		props->coders[i].last_compile_start = 0;
		i++;
	}
	assign_dongles(props);
	return (0);
}
