#include "Approximator.h"

int main() {

	Approximator app;

	app.selectReference()
		.selectDestination()
		.selectConfiguration()
		.start();

	return 0;
}
