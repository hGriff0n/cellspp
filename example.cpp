#include <iostream>
#include "reactive.h"

using namespace cells;
using namespace std;

int main() {
	auto foo = lazy(2.5);
	auto foo2 = lazy(9);
	auto comp = lazy_comp(foo2,3,"!=");
	auto add = comp + foo;
	// auto add = comp + foo + 2;
	
	/*while (comp) {
		cout << foo << " " << foo2 << " " << comp << " " << add << endl;
		foo2 -= 1;
	}*/
	for(;comp;foo2-=1) cout << foo << " " << foo2 << " " << comp << " " << add << endl;

	cin.get();
	return 0;
};