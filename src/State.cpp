/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrassokh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/07 12:03:23 by mrassokh          #+#    #+#             */
/*   Updated: 2018/07/07 12:03:24 by mrassokh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "State.hpp"

State::State()
{

}

State::State(State *predecessor)
{
	m_n = predecessor->getN();
	m_size = predecessor->getSize();
	m_gameField = *(predecessor->getGameFieldPtr());
	m_emptyPos = predecessor->getEmptyPos();
	m_predecessor = predecessor;
	m_pathCoeff = predecessor->getPathCoeff();
	m_heuristicCoeff = predecessor->getHeuristicCoeff();
	m_pathLength = predecessor->getPathLength() + m_pathCoeff * 1;
	m_heuristic = predecessor->getHeuristic();
}

State::State(int N, std::vector<int> gameField,
				int heuristicCoeff, int pathCoeff) : m_n(N > 0 ? N : 1),
				 									m_gameField(gameField),
													m_pathLength(0)
{
	m_size = m_n * m_n;
	m_emptyPos = findEmpty();
	m_heuristicCoeff = heuristicCoeff;
	m_pathCoeff = pathCoeff;
}


State::~State()
{

}

int				State::getN() const
{
	return m_n;
}

int				State::getEmptyPos() const
{
	return m_emptyPos;
}

State 			*State::getPredecessor() const
{
	return m_predecessor;
}

int				State::getTotalPredict() const
{
	return  m_pathLength + m_heuristicCoeff * m_heuristic;
}

int				State::getPathLength() const
{
	return m_pathLength;
}

int 			State::getHeuristic() const
{
	return m_heuristic;
}

int				State::getPathCoeff() const
{
	return m_pathCoeff;
}

int				State::getHeuristicCoeff() const
{
	return m_heuristicCoeff;
}

void 			State::setGameField(std::vector<int> gameField)
{
	m_gameField = gameField;
}

vecInt			*State::getGameFieldPtr()
{
	return &m_gameField;
}

void 			State::setEmptyPos(int const & empty)
{
	if (empty > m_size - 1)
		return;
	else
		m_emptyPos = empty;
}

void 			State::setPredecessor(State * predecessor)
{
	m_predecessor = predecessor;
}

void 			State::setPathLenght(int const & pathLength)
{
	if (pathLength < 0)
		m_pathLength = 0;
	m_pathLength = pathLength;
}

void 			State::setHeuristic(int const & heuristic)
{
	if (heuristic < 0)
		m_heuristic = 0;
	m_heuristic = heuristic;
}


int				State::getSize() const
{
	return m_size;
}


void 			State::printGameField() const
{
	int counter = 0;
	for ( auto i = m_gameField.begin(); i != m_gameField.end(); i++ ) {
		std::ostringstream strs;
		*i < 10 ? (strs << " " << *i) : strs << *i;
		if ((counter + 1) % m_n){
			std::cout << strs.str() << ",";
		}
		else
			std::cout << strs.str() << ";" << std::endl;
		counter++;
	}
}

int 			State::swapPuzzle(const int& pos_1, const int& pos_2)
{
	if (pos_1 < 0 || pos_1 > m_size - 1)
		return (0);
	if (pos_2 < 0 || pos_2 > m_size - 1)
		return (0);

	int tmp = m_gameField[pos_1];
	m_gameField[pos_1] = m_gameField[pos_2];
	m_gameField[pos_2] = tmp;
	return (1);
}

void 			State::swapEmptyRight()
{
	swapPuzzle(m_emptyPos, m_emptyPos + 1);
	m_emptyPos = m_emptyPos + 1;
}

void 			State::swapEmptyLeft()
{
	swapPuzzle(m_emptyPos, m_emptyPos - 1);
	m_emptyPos = m_emptyPos - 1;
}

void 			State::swapEmptyUp()
{
	swapPuzzle(m_emptyPos, m_emptyPos - m_n);
	m_emptyPos = m_emptyPos - m_n;
}

void			State::swapEmptyDown()
{
	swapPuzzle(m_emptyPos, m_emptyPos + m_n);
	m_emptyPos = m_emptyPos + m_n;
}

int 			State::findEmpty()
{
	auto it = find (m_gameField.begin(), m_gameField.end(), 0);
	return (it - m_gameField.begin());
}


size_t StateHash::operator()(const StatePtr state) const
{

	const vecInt *field = state->getGameFieldPtr();
	const int	size = state->getSize();
	size_t		seed = size;

	for (int i = 0; i < size; i++) {
		seed ^= (*field)[i] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return (seed);
}

bool StateCmp::operator()(const StatePtr left, const StatePtr right) const
{
	if (left->getTotalPredict() == right->getTotalPredict()){
		return left->getHeuristic() > right->getHeuristic();
	}
	 return left->getTotalPredict() > right->getTotalPredict();
}

bool StateIsEqual::operator()(const StatePtr left, const StatePtr right) const
{
	return *(left->getGameFieldPtr()) == *(right->getGameFieldPtr());
}
