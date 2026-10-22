/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atoi.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mromao-s <mromao-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 12:46:43 by mromao-s          #+#    #+#             */
/*   Updated: 2026/10/22 12:46:43 by mromao-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/codexion.h"

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int	ft_atoi(const char *nptr)
{
	int	i;
	int	out;
	int	has_digit;

	i = 0;
	out = 0;
	has_digit = 0;
	while (nptr[i] == ' ' || nptr[i] == '\t' || nptr[i] == '\n'
		|| nptr[i] == '\v' || nptr[i] == '\f' || nptr[i] == '\r')
		i++;
	if (nptr[i] == '-')
		return (-1);
	else if (nptr[i] == '+')
		i++;
	while (ft_isdigit(nptr[i]))
	{
		out = out * 10 + nptr[i++] - 48;
		has_digit = 1;
	}
	if (!has_digit)
		return (-1);
	if (nptr[i] && !ft_isdigit(nptr[i]))
		return (-1);
	return (out);
}
