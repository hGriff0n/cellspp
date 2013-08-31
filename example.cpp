#include "Perceptron.h"
#include "FSM.h"
#include <iostream>
#include "reactive.h"

using namespace cells;
using namespace std;

int main() {
	/*
	formula_cell<int> foo;
	formula_cell<int> bar;
	foo.reset(5);
	bar.reset([&] { return foo.get() * 2; });
	*/
	auto foo = reactive(2.5);
	auto foo2 = reactive(2.5);
	auto bar = foo * 2;				// Illustrates the formula_cell expression type conversion rule
	auto bar2 = 2 * foo;
	//auto bar2 = foo * foo2;

	cout << *foo << " " << *bar << " " << *bar2 << endl;

	foo = 7.5f;

	cout << *foo << " " << *bar << " " << *bar2 << endl;

	cin.get();
	return 0;
};