#include "hello.h"
#include <iostream>


void hello() {
	std::cout << "Hello!" << std::endl;
	std::cout << "Waiting for enter to be pressed... " << std::endl;

	//Wait for keypress
	std::cin.ignore();
}