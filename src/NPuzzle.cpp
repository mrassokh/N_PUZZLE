/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NPuzzle.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrassokh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/08 13:10:16 by mrassokh          #+#    #+#             */
/*   Updated: 2018/07/08 13:10:17 by mrassokh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NPuzzle.hpp"
#include <unistd.h>

NPuzzle::NPuzzle(t_parsedInform *initInfo) : m_succes(0), m_stepCounter(0),
												m_timeComplexity(0), m_sizeComplexity(0)
{
	srand(time(NULL));
	initFunc();
	m_N = initInfo->n;
	m_size = m_N * m_N;
	m_heuristic = initInfo->heuristic;
	m_heuristicWeight = initInfo->heuristicWeight;
	m_algorithm = initInfo->algorithm;
	m_goal = makeGoalState(m_N, m_size, m_heuristicCoeff, m_pathCoeff);
	configureAlgorithm(m_algorithm);
	if (initInfo->fromFile){
		m_start = StatePtr(new State(m_N, initInfo->gameField, m_heuristicCoeff, m_pathCoeff));
	} else {
		m_start = makeRandomStartState(m_N, m_size, m_heuristicCoeff, m_pathCoeff);
	}
	initHeuristicPuzzle();
	CheckAndSettingStartState();

}

NPuzzle::~NPuzzle()
{

}

void 			NPuzzle::proccesing()
{
	try {
		AStar();
		printOutput();
	} catch (std::exception & ex) {
		std::cout << ex.what();
		exit(0);
	}

}

/*
	initialisation common functions and start/goal states
*/
void 			NPuzzle::initFunc()
{
	m_swapCheckFunc.push_back(&NPuzzle::checkEmptyLeftSwap);
	m_swapCheckFunc.push_back(&NPuzzle::checkEmptyRightSwap);
	m_swapCheckFunc.push_back(&NPuzzle::checkEmptyUpSwap);
	m_swapCheckFunc.push_back(&NPuzzle::checkEmptyDownSwap);

	m_getHeutisticFunc.push_back(&NPuzzle::getHeuristic);
	m_getHeutisticFunc.push_back(&NPuzzle::getHeuristicPlusLinearConflict);
	m_getHeutisticFunc.push_back(&NPuzzle::getHeuristic);

	m_configAlgorithmFunc.push_back(&NPuzzle::configAStar);
	m_configAlgorithmFunc.push_back(&NPuzzle::configAStarWeighted);
	m_configAlgorithmFunc.push_back(&NPuzzle::configGreedy);
	m_configAlgorithmFunc.push_back(&NPuzzle::configUniformCost);

	m_fillHeutisticPuzzleFunc.push_back(&NPuzzle::fillManhattanHeuristicPuzzle);
	m_fillHeutisticPuzzleFunc.push_back(&NPuzzle::fillManhattanHeuristicPuzzle);
	m_fillHeutisticPuzzleFunc.push_back(&NPuzzle::fillHummingHeuristicPuzzle);
}

StatePtr 		NPuzzle::makeGoalState(int N, int size, int heuristicCoeff, int pathCoeff)
{
	int cur = 1;
	int x = 0;
	int ix = 1;
	int y = 0;
	int iy = 0;
	vecInt gameField;

	for (int i = 0; i < size; i++){
		if (gameField.size() < static_cast<unsigned long>(size))
			gameField.push_back(-1);
	}
	while (1){
		gameField[x + y * N] = cur;
		if (cur == 0)
			break;
		cur++;
		if ((x + ix) == N || (x + ix) < 0 || (ix != 0 && gameField[x + ix + y * N] != -1)){
			iy = ix;
			ix = 0;
		} else if ((y + iy) == N || (y + iy) < 0 || (iy != 0 && gameField[x + (y + iy)* N] != -1)) {
			ix = -iy;
			iy = 0;
		}
		x += ix;
		y += iy;
		if (cur == size)
			cur = 0;
		}
	StatePtr goal = new State(N, gameField, heuristicCoeff, pathCoeff);
	goal->findEmpty();
	return goal;
}

StatePtr 		NPuzzle::makeRandomStartState(int N, int size, int heuristicCoeff, int pathCoeff)
{
	int emptyPos;
	int swap;
	int swapCounter = 0;
	StatePtr start = makeGoalState(N, size, heuristicCoeff, pathCoeff);
	while (swapCounter < 10 * size){
		 swap = rand() % 4;
		 emptyPos = start->getEmptyPos();
		 if ((this->*m_swapCheckFunc[swap])(emptyPos)){
			 swapCounter++;
			 switch (swap) {
 				case LEFT:
 					start->swapEmptyLeft();
 					break;
 				case RIGHT:
 					start->swapEmptyRight();
 					break;
 				case UP:
 					start->swapEmptyUp();
 					break;
 				case DOWN:
 					start->swapEmptyDown();
 					break;
 			}
		}
	}
	return start;
}

//define solvability functions
void 		NPuzzle::CheckAndSettingStartState()
{
	if (isSolvable(m_start->getGameFieldPtr(),m_goal->getGameFieldPtr(),
						m_start->getSize(),m_start->getEmptyPos())){
		std::cout << "Input gamefield is solvable" << std::endl;
	} else 	{
		throw CustomExeption("Input gamefield is not solvable!\n");
	}
	m_start->setHeuristic((this->*m_getHeutisticFunc[m_heuristic])(m_start));
	m_start->setPathLenght(0);
	m_start->setPredecessor(nullptr);
}

int			NPuzzle::resolvePuzzleInversion(unsigned long pos, vecInt gameField)
{
	if (pos >= gameField.size())
		return -1;
	int puzzle = gameField[pos];
	int inversions = 0;

	if (puzzle == 0)
		return 0;
	for (unsigned long i = pos + 1; i < gameField.size(); ++i) {
		if (puzzle > gameField[i])
			inversions++;
	}
	return inversions;
}

int			NPuzzle::resolveBoardInversion(vecInt gameField)
{
	int inversion = 0;
	for (unsigned long i = 0; i < gameField.size(); ++i){
		inversion +=resolvePuzzleInversion(i, gameField);
	}
	return inversion;
}

int			NPuzzle::isSolvable(vecInt *startField, vecInt *goalField, int size, int emptyPos)
{
	int startInvertions = resolveBoardInversion(*startField);
	int goalInvertions = resolveBoardInversion(*goalField);
	if (size % 2 == 0) {
		startInvertions += emptyPos / size;
		goalInvertions += m_goal->getEmptyPos() / size;
	}
	return (startInvertions % 2 == goalInvertions % 2);
}


//configure type of the solution
void 		NPuzzle::configureAlgorithm(eAlgorithm algorithm)
{
	(this->*m_configAlgorithmFunc[algorithm])();
}

//f = g + h
void 		NPuzzle::configAStar()
{
	m_heuristicCoeff = 1;
	m_pathCoeff = 1;
}

//f = g + w * h
void 		NPuzzle::configAStarWeighted()
{
	m_heuristicCoeff = m_heuristicWeight;
	m_pathCoeff = 1;
}

//f = h
void 		NPuzzle::configGreedy()
{
	m_heuristicCoeff = 1;
	m_pathCoeff = 0;
}

//f = g
void 		NPuzzle::configUniformCost()
{
	m_heuristicCoeff = 0;
	m_pathCoeff = 1;
}

//precalculate heuristic for separate puzzle
void 		NPuzzle::initHeuristicPuzzle()
{
	int heuristic;
	int linearConflictRow;
	int linearConflictColumn;
	vecInt* linearConflictForPositionRow;
	vecInt* linearConflictForPositionColumn;
	vecInt* heuristicForPosition;

	for (int pos = 0; pos < m_size; ++pos) {
		heuristicForPosition = new vecInt();
		if (m_heuristic == MANHATTAN_AND_LC){
			linearConflictForPositionRow = new vecInt();
			linearConflictForPositionColumn = new vecInt();
		}
		for (int num = 0; num < m_size; ++num) {
			heuristic = ((this->*m_fillHeutisticPuzzleFunc[m_heuristic])(pos,num));
			heuristicForPosition->push_back(heuristic);
			if (m_heuristic == MANHATTAN_AND_LC){
				linearConflictRow = fillLinearConflictRowPuzzle(pos,num);
				linearConflictColumn = fillLinearConflictColumnPuzzle(pos,num);
				linearConflictForPositionRow->push_back(linearConflictRow);
				linearConflictForPositionColumn->push_back(linearConflictColumn);
			}
		}
		m_heuristicTile.push_back(heuristicForPosition);
		if (m_heuristic == MANHATTAN_AND_LC){
			m_linearConflictRowTile.push_back(linearConflictForPositionRow);
			m_linearConflictColumnTile.push_back(linearConflictForPositionColumn);
		}
	}
}

void 		NPuzzle::definePuzzlePosition(const int & position, const int & number)
{
	vecInt goal = *(m_goal->getGameFieldPtr());
	m_heurCount.heuristic = 0;
	m_heurCount.posX =  position % m_N;
	m_heurCount.posY =  position / m_N;

	m_heurCount.elementGoal = std::find (goal.begin(), goal.end(), number);
	if (m_heurCount.elementGoal == goal.end()){
			throw CustomExeption("Element is not find in goal");
	}

	m_heurCount.posElementGoal = std::distance(goal.begin(),m_heurCount.elementGoal);
	m_heurCount.posElementGoalX = m_heurCount.posElementGoal % m_N;
	m_heurCount.posElementGoalY = m_heurCount.posElementGoal / m_N;
}

int 		NPuzzle::fillLinearConflictRowPuzzle(const int & position, const int & number)
{
	definePuzzlePosition(position, number);
	if (number == 0)
		return 0;
	if (m_heurCount.posElementGoalY != m_heurCount.posY)
		return 0;
	return  m_heurCount.posX - m_heurCount.posElementGoalX;
}

int 		NPuzzle::fillLinearConflictColumnPuzzle(const int & position, const int & number)
{
	definePuzzlePosition(position, number);

	if (m_heurCount.posElementGoalX != m_heurCount.posX)
		return 0;
	return  m_heurCount.posY - m_heurCount.posElementGoalY;
}


int			NPuzzle::fillHummingHeuristicPuzzle(const int & position, const int & number)
{
	if (number == m_goal->getGameFieldPtr()->at(position))
		return 0;
	return 1;
}


int			NPuzzle::fillManhattanHeuristicPuzzle(const int & position, const int & number)
{
	definePuzzlePosition(position, number);
	m_heurCount.heuristic += std::abs(m_heurCount.posElementGoalX - m_heurCount.posX);
	m_heurCount.heuristic += std::abs(m_heurCount.posElementGoalY - m_heurCount.posY);
	return m_heurCount.heuristic;
}

int			NPuzzle::getHeuristic(StatePtr state)
{
	vecInt currentState = *(state->getGameFieldPtr());
	int heuristic = 0;
	for (int i = 0; i < m_size; i++){
		int element = currentState[i];
		if (element == 0)
			continue;
		heuristic += m_heuristicTile[i]->at(element);
	}
	return heuristic;
}

int			NPuzzle::getHeuristic1(StatePtr state)
{
	vecInt currentState = *(state->getGameFieldPtr());
	int heuristic = 0;
	for (int i = 0; i < m_size; i++){
		int element = currentState[i];
		if (element == 0)
			continue;
		heuristic += m_heuristicTile[i]->at(element);
	}
	return heuristic;
}

//terrible code style but fast
int 		NPuzzle::getHeuristicPlusLinearConflict(StatePtr state)
{
	m_LCCount.currentState = *(state->getGameFieldPtr());
	m_LCCount.heuristic = 0;
	m_LCCount.linearConflictTotal = 0;
	m_LCCount.linearConflictFirst = 0;

	//count usual Manhatten heuristic
	for (int i = 0; i < m_size; i++){
		m_LCCount.element = m_LCCount.currentState[i];
		if (m_LCCount.element == 0)
			continue;
		m_LCCount.heuristic += m_heuristicTile[i]->at(m_LCCount.element);
	}

	//count linear conflicts in rows
	for (int i = 0; i < m_size; i++){
		m_LCCount.element = m_LCCount.currentState[i];
		if (m_LCCount.element == 0)
			continue;

		m_LCCount.linearConflictFirst = m_linearConflictRowTile[i]->at(m_LCCount.element);
		if (m_LCCount.linearConflictFirst == 0)
			continue;
		m_LCCount.posX = i % m_N;

		if (m_LCCount.linearConflictFirst < 0){ //if element shift to left
			for (int j = m_LCCount.posX + 1 ; j < m_N - 1; ++j){
				m_LCCount.element = m_LCCount.currentState[i + j - m_LCCount.posX];
				if (m_LCCount.element == 0)
					continue;
				if (m_LCCount.linearConflictFirst == m_linearConflictRowTile[i + j - m_LCCount.posX]->at(m_LCCount.element)){
					break;
				}
				if (m_LCCount.linearConflictFirst == (-1) * m_linearConflictRowTile[i + j - m_LCCount.posX]->at(m_LCCount.element)){
					m_LCCount.linearConflictTotal +=1;
					break;
				}
			}
		} else if (m_LCCount.linearConflictFirst > 0) {//if element shift to right
			for (int j = 0 ; j < m_LCCount.posX; ++j){
				m_LCCount.element = m_LCCount.currentState[i - m_LCCount.posX + j];
				if (m_LCCount.element == 0)
					continue;
				if (m_LCCount.linearConflictFirst == m_linearConflictRowTile[i + j - m_LCCount.posX]->at(m_LCCount.element)){
					break;
				}
				if (m_LCCount.linearConflictFirst == (-1)*m_linearConflictRowTile[i + j- m_LCCount.posX]->at(m_LCCount.element)){
					m_LCCount.linearConflictTotal +=1;
					break;
				}
			}
		}
	}
	//count linear conflicts in columns
	for (int i = 0; i < m_size; i++){
		m_LCCount.element = m_LCCount.currentState[i];
		if (m_LCCount.element == 0)
			continue;
		m_LCCount.linearConflictFirst = m_linearConflictColumnTile[i]->at(m_LCCount.element);
		if (m_LCCount.linearConflictFirst == 0)
			continue;
		m_LCCount.posX = i % m_N;
		m_LCCount.posY = i / m_N;
		if (m_LCCount.linearConflictFirst < 0){//if element shift to up
			for (int j = m_LCCount.posY + 1 ; j < m_N - 1; ++j){
				m_LCCount.checkPos = i + (j - m_LCCount.posY) * m_N;
				m_LCCount.element = m_LCCount.currentState[m_LCCount.checkPos];
				if (m_LCCount.element == 0)
					continue;
				if (m_LCCount.linearConflictFirst == m_linearConflictColumnTile[m_LCCount.checkPos]->at(m_LCCount.element)){
					break;
				}
				if (m_LCCount.linearConflictFirst == (-1) * m_linearConflictColumnTile[m_LCCount.checkPos]->at(m_LCCount.element)){
					m_LCCount.linearConflictTotal +=1;
					break;
				}
			}
		} else if (m_LCCount.linearConflictFirst > 0) {//if element shift to down
			for (int j = 0 ; j < m_LCCount.posY; ++j){
				m_LCCount.checkPos = i + (j - m_LCCount.posY) * m_N;
				m_LCCount.element = m_LCCount.currentState[m_LCCount.checkPos];
				if (m_LCCount.element == 0)
					continue;
				if (m_LCCount.linearConflictFirst == m_linearConflictColumnTile[m_LCCount.checkPos]->at(m_LCCount.element)){
					break;
				}
				if (m_LCCount.linearConflictFirst == (-1)*m_linearConflictColumnTile[m_LCCount.checkPos]->at(m_LCCount.element)){
					m_LCCount.linearConflictTotal +=1;
					break;
				}
			}
		}
	}
	//return summ Manhatten heuristic and linear conflicts in rows and columns
	return m_LCCount.heuristic + m_LCCount.linearConflictTotal;
}


void 			NPuzzle::AStar()
{
	long long int sizeComplexity = 0;
	StatePtr e;
	int emptyPos;

	m_timeComplexity++;
	m_openPriority.push(m_start);

	while (!m_openPriority.empty() && !m_succes){
		e = m_openPriority.top(); //choose state with min f
		//if state equal to goal we find solution
		if (e->getHeuristic() == 0){
			m_succes = 1;
			break;
		}
		m_openPriority.pop();
		//if choosen state already investigated - skip it
 		if (m_closedStates.find(e) != m_closedStates.end()){
			delete e;
			continue;
		}
		//set choosen state to closedStates
		m_closedStates.insert(e);

		//expand state
		for (int i = 0; i < 4; ++i){
			emptyPos = e->getEmptyPos();
			if (!expandState(e, i, emptyPos))
				continue;
			//if expanded state already investigated - skip it
			if (m_closedStates.find(m_stateAfterExpand) != m_closedStates.end()){
				delete m_stateAfterExpand;
				continue;
			}
			//define heuristic for state and push it to open set
			m_stateAfterExpand->setHeuristic((this->*m_getHeutisticFunc[m_heuristic])(m_stateAfterExpand));
			m_openPriority.push(m_stateAfterExpand);
			m_timeComplexity++;
		}
		sizeComplexity = m_openPriority.size() + m_closedStates.size();
		if (m_sizeComplexity < sizeComplexity)
			m_sizeComplexity = sizeComplexity;
		m_stepCounter++;
	}
	findPath(e);
}



int				NPuzzle::expandState(StatePtr state, const int & swap, const int & emptyPos)
{
	if ((this->*m_swapCheckFunc[swap])(emptyPos)){
		m_stateAfterExpand = new State(state);
		switch (swap) {
			case LEFT:
				m_stateAfterExpand->swapEmptyLeft();
				return 1;
			case RIGHT:
				m_stateAfterExpand->swapEmptyRight();
				return 1;
			case UP:
				m_stateAfterExpand->swapEmptyUp();
				return 1;
			case DOWN:
				m_stateAfterExpand->swapEmptyDown();
				return 1;
		}
	}
	return 0;
}

int 			NPuzzle::checkEmptyRightSwap(const int& empty)
{
	int x_pos = empty  % m_N;
	if (x_pos < 0 || empty > m_size - 2 || x_pos == m_N - 1)
		return (0);
	return (1);
}

int 			NPuzzle::checkEmptyLeftSwap(const int& empty)
{
	int x_pos = empty  % m_N;
	if (x_pos <= 0 || empty > m_size - 1)
		return (0);
	return (1);
}

int 			NPuzzle::checkEmptyUpSwap(const int& empty)
{
	int y_pos = empty  / m_N;
	if (y_pos <= 0 || empty > m_size - 1)
		return (0);
	return (1);
}

int 			NPuzzle::checkEmptyDownSwap(const int& empty)
{
	int y_pos = empty  / m_N;
	if (y_pos < 0 || y_pos > m_N - 2 ||  empty > m_size - m_N - 1)
		return (0);
	return (1);
}

//output
void 			NPuzzle::findPath(StatePtr finalState)
{
	StatePtr currentState = finalState;
	while (currentState->getPredecessor()){
		m_path.push_back(currentState);
		currentState = currentState->getPredecessor();
	}
	m_path.push_back(currentState);
}

void 			NPuzzle::printPath()
{
	auto currentState = m_path.rbegin();
	int counter = 0;

	while (currentState != m_path.rend()){
		std::cout<< "--- node on step " << counter++ << "  ---" << std::endl;
		(*currentState)->printGameField();
		std::cout<< "Path legth is : " << (*currentState)->getPathLength() << std::endl;
		std::cout<< "Heuristic is : " << (*currentState)->getHeuristic() << std::endl;
		std::cout<< "Total predict path : " << (*currentState)->getTotalPredict() << std::endl;
		std::cout<< "--- // --- // --- // ---" << std::endl;
		currentState++;
	}
}

void 			NPuzzle::printOutput()
{
	std::cout<< "--- # --- # --- # ---" << std::endl;
	std::cout<< "PATH FROM START TO GOAL STATE : " << std::endl;
	printPath();
	std::cout<< "--- # --- # --- # ---" << std::endl;
	std::cout<< "START STATE : " << std::endl;
	m_start->printGameField();
	std::cout<< "--- # --- # --- # ---" << std::endl;
	std::cout<< "GOAL STATE : " << std::endl;
	m_goal->printGameField();
	std::cout<< "--- # --- # --- # ---" << std::endl;
	std::cout<< "SOLVED BY : ";
	switch (m_algorithm) {
		case A_STAR:
			std::cout<< "standart A-star algorithm" << std::endl;
			break;
		case A_STAR_WEIGHTED:
			std::cout<< "weighted A-star algorithm with heuristic coefficient "
					<< m_heuristicCoeff << std::endl;
			if (m_heuristicCoeff == 1)
			std::cout << "Warning! Heuristic coefficient set to default value 1.\n"
						<< "So weighted A-star algorithm equal to standart A-star"
				 		<< std::endl;
			break;
		case GREEDY:
			std::cout<< "greedy algorithm"  << std::endl;
			break;
		case UNIFORM_COST:
			std::cout<< "uniform cost algorithm"  << std::endl;
			break;
	}
	std::cout<< "WITH HEURISTIC: ";
	switch (m_heuristic) {
		case MANHATTAN:
			std::cout<< "MANHATTAN"  << std::endl;
			break;
		case MANHATTAN_AND_LC:
			std::cout<< "MANHATTAN AND LINEAR CONFLICT"  << std::endl;
			break;
		case HAMMING:
			std::cout<< "HAMMING"  << std::endl;
			break;
	}
	std::cout << "SOLUTION TIME COMPLEXITY : " << m_timeComplexity << std::endl;
	std::cout << "SOLUTION SIZE COMPLEXITY : " << m_sizeComplexity << std::endl;
	std::cout << "TOTAL NUMBER OF STEPS : " << m_stepCounter << std::endl;
	std::cout << "PATH LENGTH : " << m_path.size() - 1 << std::endl;
}
