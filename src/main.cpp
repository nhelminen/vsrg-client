#include "core/app.hpp"

using namespace vsrg;

int main(int argc, char* argv[]) {

	{
		initialize();
		run();
		destroy();
	}

	return 0;
}