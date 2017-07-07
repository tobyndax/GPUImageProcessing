#include "hello.h"
#include <iostream>


void hello() {
	std::cout << "Hello!" << std::endl;
	std::cout << "Waiting for key input... " << std::endl;

	//Wait for keypress
	std::cin.ignore();
}