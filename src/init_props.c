/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_props.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mromao-s <mromao-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 12:56:22 by mromao-s          #+#    #+#             */
/*   Updated: 2026/10/22 12:56:22 by mromao-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/codexion.h"

static int	ms_to_us(const char *str)
{
	int	ms;

	ms = ft_atoi(str);
	if (ms < 0 || ms > MAX_INT / 1000)
		return (-1);
	return (ms * 1000);
}

static void	*map_fail(t_props *props, char *msg)
{
	printf("%s\n", msg);
	free(props->coders);
	free(props->dongles);
	free(props->queue);
	free(props);
	return (NULL);
}

void	assign_args(t_props	*props, char *args[])
{
	(*props).nb_of_coders = ft_atoi(args[1]);
	(*props).time_to_burnout = ms_to_us(args[2]);
	(*props).time_to_compile = ms_to_us(args[3]);
	(*props).time_to_debug = ms_to_us(args[4]);
	(*props).time_to_refactor = ms_to_us(args[5]);
	(*props).number_of_compiles_required = ft_atoi(args[6]);
	(*props).dongle_cooldown = ms_to_us(args[7]);
	(*props).scheduler = args[8];
	(*props).queue_size = 0;
	(*props).seq_counter = 0;
	(*props).start = 0;
	(*props).dead = 0;
}

void	no_coders_props(t_props	*props)
{
	(*props).coders = NULL;
	(*props).dongles = NULL;
	(*props).queue = NULL;
}

t_props	*map_args(char *args[])
{
	t_props	*props;

	props = malloc(sizeof(t_props));
	if (!props)
		return (NULL);
	assign_args(props, args);
	no_coders_props(props);
	if ((*props).nb_of_coders > 0 && (*props).nb_of_coders <= MAX_CODERS)
	{
		(*props).coders = malloc(sizeof(t_coders) * (*props).nb_of_coders);
		(*props).dongles = malloc(sizeof(t_dongles) * (*props).nb_of_coders);
		(*props).queue = malloc(sizeof(int) * (*props).nb_of_coders);
		if (!(*props).coders || !(*props).dongles || !(*props).queue)
			return (map_fail(props, "malloc error"));
	}
	if ((pthread_mutex_init(&props->print, 0) != 0)
		|| (pthread_mutex_init(&props->scheduler_mutex, 0) != 0)
		|| (pthread_cond_init(&props->scheduler_cond, 0) != 0))
		return (map_fail(props, "pthread init error"));
	return (props);
}

void	free_invalid_args(t_props *props)
{
	if (!props)
		return ;
	free(props->coders);
	free(props->dongles);
	free(props->queue);
	pthread_mutex_destroy(&props->print);
	pthread_mutex_destroy(&props->scheduler_mutex);
	pthread_cond_destroy(&props->scheduler_cond);
	free(props);
}

int	validate_args(t_props **props)
{
	if (((*props)->nb_of_coders < 1)
		|| ((*props)->nb_of_coders > MAX_CODERS)
		|| ((*props)->time_to_burnout == -1)
		|| ((*props)->time_to_compile == -1) || ((*props)->time_to_debug == -1)
		|| ((*props)->time_to_refactor == -1)
		|| ((*props)->number_of_compiles_required == -1)
		|| ((*props)->dongle_cooldown == -1))
		return (1);
	if (ft_strcmp((*props)->scheduler, "fifo") != 0
		&& ft_strcmp((*props)->scheduler, "edf") != 0)
		return (1);
	return (0);
}
