/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrassokh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/14 11:31:55 by mrassokh          #+#    #+#             */
/*   Updated: 2018/07/14 11:31:57 by mrassokh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser*			Parser::m_instance = 0;

Parser::Parser() : m_digit_regex("((\\s|\\t|\\r)+)?[\\d]+((\\s|\\t|\\r)+)?"),
					m_heuristic_flag_regex("((\\s|\\t|\\r)+)?(-h)((\\s|\\t|\\r)+)?"),
					m_algorithm_flag_regex("((\\s|\\t|\\r)+)?(-a)((\\s|\\t|\\r)+)?"),
					m_heuristicWeight_flag_regex("((\\s|\\t|\\r)+)?(-w)((\\s|\\t|\\r)+)?"),
					m_manhattan_heuristic_regex("((\\s|\\t|\\r)+)?(M)((\\s|\\t|\\r)+)?"),
					m_manhattan_lc_heuristic_regex("((\\s|\\t|\\r)+)?(M_LC)((\\s|\\t|\\r)+)?"),
					m_hamming_heuristic_regex("((\\s|\\t|\\r)+)?(H)((\\s|\\t|\\r)+)?"),
					m_astar_algorithm_regex("((\\s|\\t|\\r)+)?(A_STAR)((\\s|\\t|\\r)+)?"),
					m_astar_weighted_algorithm_regex("((\\s|\\t|\\r)+)?(A_STAR_W)((\\s|\\t|\\r)+)?"),
					m_greedy_algorithm_regex("((\\s|\\t|\\r)+)?(G)((\\s|\\t|\\r)+)?"),
					m_uniform_cost_algorithm_regex("((\\s|\\t|\\r)+)?(U_C)((\\s|\\t|\\r)+)?")
{
	m_readedLines = vesStr(new std::vector<std::string>());
	m_parsedInform.heuristic = MANHATTAN;
	m_parsedInform.algorithm = A_STAR;
	m_parsedInform.heuristicWeight = 1;
	initRegexArrays();
	initFunctionArrays();
	initUsage();
}

Parser::~Parser()
{

}

Parser*			Parser::getInstance()
{
	if (m_instance == nullptr)
		m_instance = new Parser();
	return m_instance;
}

void 			Parser::initFunctionArrays()
{
	m_fill_func.push_back(&Parser::fillHeuristic);
	m_fill_func.push_back(&Parser::fillAlgorithm);
	m_fill_func.push_back(&Parser::fillHeuristicWeight);
}

void 			Parser::initRegexArrays()
{
	m_flag_regex.push_back(m_heuristic_flag_regex);
	m_flag_regex.push_back(m_algorithm_flag_regex);
	m_flag_regex.push_back(m_heuristicWeight_flag_regex);

	m_heuristic_regex.push_back(m_manhattan_heuristic_regex);
	m_heuristic_regex.push_back(m_manhattan_lc_heuristic_regex);
	m_heuristic_regex.push_back(m_hamming_heuristic_regex);

	m_algorithm_regex.push_back(m_astar_algorithm_regex);
	m_algorithm_regex.push_back(m_astar_weighted_algorithm_regex);
	m_algorithm_regex.push_back(m_greedy_algorithm_regex);
	m_algorithm_regex.push_back(m_uniform_cost_algorithm_regex);
}

void 			Parser::initUsage()
{
	std::ostringstream usage;
	usage << "Usage:\nFor reading from file: ./npuzzle -f filename\n"
			<< "For generate random n_puzzle: ./npuzzle -n size_of_square_side\n"
			<< "Set heuristic: [-h] [heuristic]\n"
			<<"	Possible heuristic:\n"
			<<"		M 		Manhattan\n"
			<<"		M_LC 		Manhattan with linear conflicts\n"
			<<"		H		Hamming\n"
			<< "Set algorithm: [-a] [algorithm]\n"
			<<"	Possible algorithm:\n"
			<<"		A_STAR		Standart A-star: f = g + h\n"
			<<"		A_STAR_W	A-star with weighted heuristic: f = g + w * h\n"
			<<"		G		greedy: f = h\n"
			<<"		U_C		uniform cost: f = g\n"
			<< "Set heuristic weight: [-w] [heuristic_weight] (make sense only for A_STAR_W algorithm)\n";
	m_usage = usage.str();
}

void 			Parser::processing(int ac, char **av)
{
	std::string size;
	std::string flag;

	if (ac < 3 || ac > 9){
		throw CustomExeption(m_usage);
	} else if (std::string(av[1]) == "-f"){
		m_parsedInform.fromFile = true;
		parseFile(av[2]);
	} else if (std::string(av[1]) == "-n") {
		m_parsedInform.fromFile = false;
		size = std::string(av[2]);
		readSize(size);
	}

	for (int i = 3; i < ac; ++i) {
		flag = std::string(av[i]);
		for (size_t j = 0; j < m_flag_regex.size(); ++j) {
			if (std::regex_match(flag, m_flag_regex[j])) {
				(this->*m_fill_func[j])(ac, av);
			}
		}
	}
}

void 			Parser::parseFile(char *fileName)
{
	try {
		readFile(fileName);
	} catch (std::exception & ex) {
		std::cout << ex.what() << std::endl;
		std::cout << m_usage << std::endl;
		exit(0);
	}
}


t_parsedInform 		*Parser::getParserInform()
{
   return 			&m_parsedInform;
}


void 			Parser::fillHeuristic(int ac, char **av)
{
	std::string heuristic;
	int findHeuristic = 0;

	for (int i = 3; i < ac; ++i) {
		heuristic = std::string(av[i]);
		for (size_t j = 0; j < m_heuristic_regex.size(); ++j) {
			if (std::regex_match(heuristic, m_heuristic_regex[j])) {
				if (!findHeuristic)
					findHeuristic = 1;
				else
					throw CustomExeption("Multiple heuristic found!!!\n" + m_usage);
				m_parsedInform.heuristic = static_cast<eHeuristic>(j);
			}
		}
	}
	if (!findHeuristic)
		throw CustomExeption("Heuristic is not found!!!\n" + m_usage);
}

void 			Parser::fillHeuristicWeight(int ac, char **av)
{
	std::string heuristicWeight;
	int findHeuristicWeight = 0;

	for (int i = 3; i < ac; ++i) {
		heuristicWeight = std::string(av[i]);
		if (std::regex_match(heuristicWeight, m_digit_regex)) {
			if (!findHeuristicWeight)
				findHeuristicWeight = 1;
			else
				throw CustomExeption("Multiple heuristic weight found!!!\n" + m_usage);
			m_parsedInform.heuristicWeight = std::stoi(heuristicWeight);
		}
	}
	if (!findHeuristicWeight)
		throw CustomExeption("Heuristic weight is not found!!!\n" + m_usage);
}

void 			Parser::fillAlgorithm(int ac, char **av)
{
	std::string algorithm;
	int findAlgorithm = 0;

	for (int i = 3; i < ac; ++i) {
		algorithm = std::string(av[i]);
		for (size_t j = 0; j < m_algorithm_regex.size(); ++j) {
			if (std::regex_match(algorithm, m_algorithm_regex[j])) {
				if (!findAlgorithm)
					findAlgorithm = 1;
				else
					throw CustomExeption("Multiple algorithm found!!!\n" + m_usage);
				m_parsedInform.algorithm = static_cast<eAlgorithm>(j);
			}
		}
	}
	if (!findAlgorithm)
		throw CustomExeption("Algorithm is not found!!!\n" + m_usage);
}



void 			Parser::readFile(char *fileName)
{
	std::ifstream   myFile(fileName);

	if (myFile.is_open()) {
		std::string     inputBuffer;
		int				rowCounter = 0;
		int				rowNumber = 0;
		while (std::getline(myFile, inputBuffer)) {
			if (myFile.fail()) {
				throw std::ios_base::failure("Failed to read from input source.");
			}
			try {
				eraseComment(inputBuffer);
				rowNumber++;
				if (inputBuffer.empty())
					continue;
				if (rowCounter++ == 0){
					readSize(inputBuffer);
				} else {
					proceedInputString(inputBuffer);
				}
			} catch (CustomExeption & ex) {
				throw CustomExeption(std::string("[line ") + std::to_string(rowNumber) + "] " + ex.what());
			}
		}
		myFile.close();
		if (rowCounter - 1 != m_parsedInform.n)
			throw CustomExeption("Number of rows is not equal to size");
	} else {
		throw CustomExeption("Unable to open " + std::string(fileName) + "\n");
	}
}

void 			Parser::readSize(std::string &str)
{
	if (std::regex_match(str, m_digit_regex)) {
		int squareSize = std::stoi(str);

		if (squareSize > 1) {
			m_parsedInform.n = squareSize;
		} else {
			throw CustomExeption("Square size must be at least 2(two)");
		}
	} else {
		throw CustomExeption("Incorrect data for square size. It must be integer");
	}
}

void            Parser::eraseComment(std::string &str) {
    auto    startCommentPosition = str.find('#');

    if (startCommentPosition != std::string::npos) {
        str.erase(str.begin() + startCommentPosition, str.end());
    }
	trim(str);
}

void 			 Parser::proceedInputString(std::string &str)
{
	size_t	f = std::string::npos;
	std::string	token = "";
	int columnCounter = 1;

	f = str.find_first_of(" \t");
	while (f != std::string::npos){
		token = std::string(str.begin(), str.begin() + f);
		pushPuzzle(token);
		str = str.substr(f + 1, str.size() - f + 1 );
		trim(str);
		f = str.find_first_of(" \t");
		columnCounter++;
	}
	if (columnCounter != m_parsedInform.n)
			throw CustomExeption("Wrong number of elements in string!");
	token = std::string(str.begin(), str.begin() +  str.size());
	pushPuzzle(token);
}

void 			Parser::pushPuzzle(std::string &str)
{
	if (std::regex_match(str, m_digit_regex)) {
		int puzzle = std::stoi(str);
		if (puzzle >= 0) {
			m_parsedInform.gameField.push_back(puzzle);
		} else {
			throw CustomExeption("Puzzle must be at least 0");
		}
	} else {
		throw CustomExeption("Incorrect data for puzzle. It must be integer");
	}
}

void 			Parser::printGameField() const
{
	int counter = 0;
	for (auto i = m_parsedInform.gameField.begin(); i != m_parsedInform.gameField.end(); i++) {
		std::ostringstream strs;
		*i < 10 ? (strs << " " << *i) : strs << *i;
		if ((counter + 1) % m_parsedInform.n){
			std::cout << strs.str() << ",";
		}
		else
			std::cout << strs.str() << ";" << std::endl;
		counter++;
	}
}

void            Parser::trim(std::string &str) {
	//trim string
	size_t p = str.find_first_not_of(" \t");
	str.erase(0, p);
	p = str.find_last_not_of(" \t");
	if (std::string::npos != p)
		str.erase(p + 1);
}
