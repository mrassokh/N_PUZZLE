/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrassokh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/07 12:03:08 by mrassokh          #+#    #+#             */
/*   Updated: 2018/07/07 12:03:10 by mrassokh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATE_HPP
# define STATE_HPP

#include <map>
#include <algorithm>
#include <array>
#include <vector>
#include <ctime>
#include <iostream>
#include <sstream>
#include <deque>

typedef std::vector<int> 	vecInt;

typedef enum SWAP
{
	LEFT,
	RIGHT,
	UP,
	DOWN
}			eSwap;

class State {
public:

	State(int N, std::vector<int> gameField, int heuristicCoeff, int pathCoeff);
	State(State *predecessor);
	State(State & rhs) = default;
	State & operator = (State & rhs) = default;
	virtual ~State();

	int						getN() const;
	int						getSize() const;

	vecInt					*getGameFieldPtr();
	int						getEmptyPos() const;
	State 					*getPredecessor() const;
	int						getPathLength() const;
	int 					getHeuristic() const;
	int						getTotalPredict() const;
	int						getPathCoeff() const;
	int						getHeuristicCoeff() const;

	void 					setEmptyPos(int const & empty);
	void 					setPredecessor(State * predecessor);

	void 					setPathLenght(int const & pathLength);
	void 					setGameField(std::vector<int> gameField);
	void 					setHeuristic(int const & heuristic);

	int 					findEmpty();
	int 					swapPuzzle(const int& pos_1, const int& pos_2);

	void 					swapEmptyRight();
	void					swapEmptyLeft();
	void 					swapEmptyUp();
	void 					swapEmptyDown();

	void 					printGameField() const;

private:
	State();

	int 					m_n;//size of square
	int 					m_size;//size of gamefield array
	vecInt					m_gameField;//array of game puzzles
	int						m_emptyPos;//pozition of empty element in array
	State 					*m_predecessor;//pointer to the predecessor state;
	int						m_pathLength;//length of path from start to this state
	int						m_heuristic;//heuristic length of path from this state to goal
	int						m_heuristicCoeff;
	int						m_pathCoeff;
};

typedef State *					StatePtr;
typedef	std::vector<State *>	vecState;
typedef std::deque<State *>		deqState;

struct StateHash {
	size_t operator()(const StatePtr state) const;
};

struct StateCmp {
	bool operator()(const StatePtr left, const StatePtr right) const;
};

struct StateIsEqual {
	bool operator()(const StatePtr left, const StatePtr right) const;
};

#endif
