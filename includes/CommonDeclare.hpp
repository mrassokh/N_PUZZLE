/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommonDeclare.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrassokh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 14:12:14 by mrassokh          #+#    #+#             */
/*   Updated: 2018/08/11 14:12:16 by mrassokh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_DECLARE_HPP
# define COMMON_DECLARE_HPP

# include "CustomExeptions.hpp"

typedef enum 	Heuristic
{
	MANHATTAN,
	MANHATTAN_AND_LC,
	HAMMING
}				eHeuristic;

typedef enum 	Algorithm
{
	A_STAR,
	A_STAR_WEIGHTED,
	GREEDY,
	UNIFORM_COST
}				eAlgorithm;

typedef	struct				parsedInform
{
	std::vector<int>		gameField;
	bool					fromFile;
	int 					n;
	eHeuristic				heuristic;
	int						heuristicWeight;
	eAlgorithm				algorithm;
}							t_parsedInform;

# endif
