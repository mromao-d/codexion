/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mromao-s <mromao-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 13:06:44 by mromao-s          #+#    #+#             */
/*   Updated: 2026/10/22 13:07:10 by mromao-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/codexion.h"

static time_t	get_deadline(t_props *props, int coder_id)
{
	return (props->coders[coder_id - 1].last_compile_start
		+ props->time_to_burnout / 1000);
}

static int	has_priority(t_props *props, int a, int b)
{
	time_t	deadline_a;
	time_t	deadline_b;

	if (ft_strcmp(props->scheduler, "edf") == 0)
	{
		deadline_a = get_deadline(props, a);
		deadline_b = get_deadline(props, b);
		if (deadline_a != deadline_b)
			return (deadline_a < deadline_b);
		return (a < b);
	}
	return (props->coders[a - 1].arrival_seq
		< props->coders[b - 1].arrival_seq);
}

static void	sift_up(t_props *props, int i)
{
	int	parent;
	int	tmp;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!has_priority(props, props->queue[i], props->queue[parent]))
			break ;
		tmp = props->queue[parent];
		props->queue[parent] = props->queue[i];
		props->queue[i] = tmp;
		i = parent;
	}
}

static void	sift_down(t_props *props, int i)
{
	int	left;
	int	right;
	int	smallest;
	int	tmp;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < props->queue_size
			&& has_priority(props, props->queue[left], props->queue[smallest]))
			smallest = left;
		if (right < props->queue_size
			&& has_priority(props, props->queue[right], props->queue[smallest]))
			smallest = right;
		if (smallest == i)
			break ;
		tmp = props->queue[smallest];
		props->queue[smallest] = props->queue[i];
		props->queue[i] = tmp;
		i = smallest;
	}
}

void	add_coder_queue(t_coders *coder)
{
	int	i;

	i = 0;
	while (i < coder->props->queue_size)
	{
		if (coder->props->queue[i] == coder->coder_id)
			return ;
		i++;
	}
	coder->arrival_seq = coder->props->seq_counter++;
	coder->props->queue[coder->props->queue_size] = coder->coder_id;
	coder->props->queue_size++;
	sift_up(coder->props, coder->props->queue_size - 1);
}

void	remove_coder_queue(t_coders *coder)
{
	t_props	*props;
	int		i;
	int		last;

	props = coder->props;
	i = 0;
	while (i < props->queue_size && props->queue[i] != coder->coder_id)
		i++;
	if (i == props->queue_size)
		return ;
	last = props->queue_size - 1;
	props->queue[i] = props->queue[last];
	props->queue_size--;
	if (i < props->queue_size)
	{
		sift_down(props, i);
		sift_up(props, i);
	}
}

static int	coder_waiting_with_priority(t_props *props, int rival_id, int me_id)
{
	int	i;

	i = 0;
	while (i < props->queue_size)
	{
		if (props->queue[i] == rival_id)
			return (has_priority(props, rival_id, me_id));
		i++;
	}
	return (0);
}

static int	neighbor_id(t_props *props, int coder_id, int offset)
{
	int	n;

	n = (coder_id - 1 + offset + props->nb_of_coders) % props->nb_of_coders;
	return (n + 1);
}

int	is_blocked_by_rival(t_coders *coder)
{
	int	left;
	int	right;

	if (coder->props->nb_of_coders < 2)
		return (0);
	left = neighbor_id(coder->props, coder->coder_id, -1);
	right = neighbor_id(coder->props, coder->coder_id, 1);
	if (coder_waiting_with_priority(coder->props, left, coder->coder_id))
		return (1);
	if (right != left
		&& coder_waiting_with_priority(coder->props, right, coder->coder_id))
		return (1);
	return (0);
}
