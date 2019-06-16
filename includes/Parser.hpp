/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrassokh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/14 11:31:40 by mrassokh          #+#    #+#             */
/*   Updated: 2018/07/14 11:31:41 by mrassokh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP
# include <State.hpp>
# include <regex>
# include <fstream>
# include <sstream>
# include "CommonDeclare.hpp"

typedef std::shared_ptr<std::vector<std::string>> vesStr;

class Parser {
public:
	static Parser*			getInstance();
	void 					processing(int ac, char **av);
	void 					parseFile(char *fileName);
	t_parsedInform			*getParserInform();

private:
	Parser();
	Parser(Parser const & rhs) = delete;
	Parser & operator = (Parser const & rhs) = delete;
	virtual ~Parser();

	void 					initFunctionArrays();
	void 					initRegexArrays();
	void 					initUsage();

	void 					fillHeuristic(int ac, char **av, int *argCounter);
	void 					fillAlgorithm(int ac, char **av, int *argCounter);
	void 					fillHeuristicWeight(int ac, char **av, int *argCounter);

	void 					readFile(char *fileName);
	void 					readSize(std::string &str);
	void            		eraseComment(std::string &str);
	void 					proceedInputString(std::string &str);
	void 					pushPuzzle(std::string &str);
	void 					printGameField() const;
	void            		trim(std::string &str);



	static Parser*			m_instance;
	vesStr 					m_readedLines;
	t_parsedInform			m_parsedInform;

	std::string				m_usage;
	const std::regex 		m_digit_regex;

	const std::regex 		m_heuristic_flag_regex;
	const std::regex 		m_algorithm_flag_regex;
	const std::regex 		m_heuristicWeight_flag_regex;

	const std::regex 		m_manhattan_heuristic_regex;
	const std::regex 		m_manhattan_lc_heuristic_regex;
	const std::regex 		m_hamming_heuristic_regex;

	const std::regex 		m_astar_algorithm_regex;
	const std::regex 		m_astar_weighted_algorithm_regex;
	const std::regex 		m_greedy_algorithm_regex;
	const std::regex 		m_uniform_cost_algorithm_regex;

	std::vector<std::regex>	m_flag_regex;
	std::vector<std::regex>	m_heuristic_regex;
	std::vector<std::regex>	m_algorithm_regex;

	std::vector<void (Parser::*)(int ac, char **av, int *argCounter)> m_fill_func;
};
#endif
