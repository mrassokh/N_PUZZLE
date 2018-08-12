/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrassokh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/07 11:16:43 by mrassokh          #+#    #+#             */
/*   Updated: 2018/07/07 11:16:47 by mrassokh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NPuzzle.hpp"
# include "Parser.hpp"


int main(int ac, char **av){
	try{
		Parser* parser = Parser::getInstance();
		parser->processing(ac, av);
		NPuzzle *npuzzle = new NPuzzle(parser->getParserInform());
		npuzzle->proccesing();
	} catch (std::exception & ex) {
		std::cout << ex.what()  << std::endl;
		exit(0);
	}
}
