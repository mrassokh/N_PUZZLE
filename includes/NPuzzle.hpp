/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   npuzzle.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrassokh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/07 11:17:56 by mrassokh          #+#    #+#             */
/*   Updated: 2018/07/07 11:17:57 by mrassokh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NPUZZLE_HPP
# define NPUZZLE_HPP
# include "State.hpp"
# include "Parser.hpp"
# include "CommonDeclare.hpp"
# include <cmath>
# include <algorithm>
# include <queue>
# include <unordered_set>
# include <set>
# include <deque>

typedef std::priority_queue<StatePtr, std::deque<State *>, StateCmp>	statePriorityQueue;
typedef std::unordered_set<StatePtr, StateHash, StateIsEqual>			stateUnorderedSet;

typedef struct					s_heuristicCount
{
	int 						heuristic;
	int 						posX;
	int 						posY;
	int 						posElementGoal;
	int 						posElementGoalX;
	int 						posElementGoalY;
	std::vector<int>::iterator 	elementGoal;
}								t_heuristicCount;

typedef struct					s_linearConflict
{
	int 						heuristic;
	int 						posX;
	int 						posY;
	int 						linearConflictFirst;
	int 						linearConflictTotal;
	int 						element;
	int 						checkPos;
	int 						posElementGoalX;
	int 						posElementGoalY;
	vecInt 						currentState;
}								t_linearConflict;



class NPuzzle {
public:
	NPuzzle(t_parsedInform *initInfo);
	virtual ~NPuzzle();
	void 						proccesing();

private:
/*[--------------------------------FUNCTIONS---------------------------------]*/
	NPuzzle();
	NPuzzle(NPuzzle const & rhs) = delete;
	NPuzzle & operator = (NPuzzle const & rhs) = delete;

	//initialisation common functions and start/goal states
	void 						initFunc();
	StatePtr 					makeGoalState(int N, int size, int heuristicCoeff, int pathCoeff);
	StatePtr 					makeRandomStartState(int N, int size, int heuristicCoeff, int pathCoeff);
	void 						CheckAndSettingStartState();

	//define solvability functions
	int							resolvePuzzleInversion(unsigned long pos, vecInt gameField);
	int							resolveBoardInversion(vecInt gameField);
	int							isSolvable(vecInt *startField, vecInt *goalField, int size, int emptyPos);

	//configure type of the solution
	void 						configureAlgorithm(eAlgorithm	algorithm);
	void 						configAStar();//f = g + h
	void 						configAStarWeighted();//f = g + w * h
	void 						configGreedy();//f = h
	void 						configUniformCost();//f = g

	//precalculate heuristic and linear conflict for separate puzzles
	void 						initHeuristicPuzzle();
	void 						initLinearConflictRowPuzzle();
	void 						initLinearConflictColumnPuzzle();

	void 						definePuzzlePosition(const int & position, const int & number);
	int 						fillManhattanHeuristicPuzzle(const int & position, const int & number);
	int							fillHummingHeuristicPuzzle(const int & position, const int & number);
	int 						fillLinearConflictRowPuzzle(const int & position, const int & number);
	int 						fillLinearConflictColumnPuzzle(const int & position, const int & number);

	//get heuristic functions
	int 						getHeuristic(StatePtr state);
	int							getHeuristic1(StatePtr state);
	int 						getHeuristicPlusLinearConflict(StatePtr state);

	//general algorithm
	void 						AStar();

	//expanding state function with checkers for swap possibility in four direction
	int 						expandState(StatePtr state, const int &  swap, const int &  emptyPos);
	int 						checkEmptyRightSwap(const int& empty);
	int 						checkEmptyLeftSwap(const int& empty);
	int 						checkEmptyUpSwap(const int& empty);
	int 						checkEmptyDownSwap(const int& empty);

	//output
	void 						findPath(StatePtr finalState);
	void 						printPath();
	void 						printOutput();

/*[--------------------------------PROPERTIES---------------------------------]*/

	int							m_N;//size of the square side
	int							m_size;//m_N * m_N
	eHeuristic					m_heuristic;
	eAlgorithm					m_algorithm;
	int							m_heuristicCoeff;
	int							m_heuristicWeight;
	int							m_pathCoeff;
	int							m_succes;
	int							m_stepCounter;

	StatePtr					m_goal;//goal state
	StatePtr					m_start;//start state
	StatePtr					m_stateAfterExpand;//expandend state for new iteration

	statePriorityQueue			m_openPriority;
	stateUnorderedSet			m_closedStates;
	deqState					m_expandStates;

	/*precalculated heuristic and linear conflicts
	for every puzzle in every positions*/
	std::vector<vecInt *>		m_heuristicTile;
	std::vector<vecInt *>		m_linearConflictRowTile;
	std::vector<vecInt *>		m_linearConflictColumnTile;

	//output information(complexities and final path)
	long long int				m_timeComplexity;
	long long int				m_sizeComplexity;
	vecState					m_path;

	//strange structures for counting info
	t_heuristicCount			m_heurCount;
	t_linearConflict			m_LCCount;

	//arrays of pointers to functions
	std::vector<int (NPuzzle::*)(const int& empty)>      	m_swapCheckFunc;
	std::vector<void (NPuzzle::*)(void)>      				m_configAlgorithmFunc;
	std::vector<int (NPuzzle::*)(StatePtr state)>       	m_getHeutisticFunc;
	std::vector<int (NPuzzle::*)(const int & position,
									const int & number)>	m_fillHeutisticPuzzleFunc;
};

#endif
