	// Feature Work: assign in constructor
		// implemented
	// Feature Work: assign from function
		// implemented
	// Feature work : assign from value
		// implemented
	// Feature work : assign from value with auto
		// implemented
	// Feature work : boolean operators
		// implemented
	// Feature work : self expressions	
		// implemented


	// Feature work : assign from expression
		// implemented (partially) - chaining expressions not implemented
		// unsure about chaining expressions	eg. auto tVar = oVar * dVar - aVar;

		/*
		template <typename T, typename R>
		cells::formula_cell<R> operator+(formula_cell<T>& rhs,std::function<R ()>& lhs) {
			return [=,&rhs] { return (R)rhs.get() + lhs(); };
		}
		template <typename T, typename R>
		cells::formula_cell<R> operator+(std::function<T ()>& rhs,cells::formula_cell<R>& lhs) {
			return [=,&rhs] { return (R)rhs() + lhs.get; };
		}
		template <typename T, typename R>
		cells::formula_cell<R> operator+(cells::formula_cell<T>& rhs,cells::formula_cell<R>& lhs) {
			return [=,&rhs] { return (R)rhs.get() + lhs.get(); };
		}
		template <typename T, typename R>
		cells::formula_cell<R> operator*(cells::formula_cell<T>& rhs,cells::formula_cell<R>& lhs) {
			return [=,&rhs] { return (R)rhs.get() * lhs.get(); };
		}
		template <typename T, typename R>
		cells::formula_cell<R> operator-(cells::formula_cell<T>& rhs,cells::formula_cell<R>& lhs) {
			return [=,&rhs] { return (R)rhs.get() - lhs.get(); };
		}
		template <typename T, typename R>
		cells::formula_cell<R> operator/(cells::formula_cell<T>& rhs,cells::formula_cell<R>& lhs) {
			return [=,&rhs] { return (R)rhs.get() / lhs.get(); };
		}

		*/


	// Feature work : move assignment
		// immplemented (partially)	-- 'copy' not move assignment
		// no need to move the current_value (will get recomputed when get() is called)
		// auto foo = bar + 5;		--> formula_cell<int> foo([=] { return (int)bar.get() + 5; });

		/*
		template <typename R>
		formula_cell<T>& formula_cell<T>::swap(formula_cell<R>&& val) {
			this->reset_dependents(val.dependents);
			this->reset_dependencies(val.dependencies);
			this->reset([=] { return (T)val.formula(); });
		}
		template <typename T, typename R=T>
		formula_cell<T>& operator=(formula_cell<T>& lhs, formula_cell<R>&& rhs) {
			return lhs.swap(rhs);
		};
		*/

	
	/*
	// Feature +0:
	formula_cell<int> foo;
	formula_cell<int> bar;
	foo.reset(5);
	bar.reset([=] { return foo.get() + 2; });

	// Feature +1:
	formula_cell<int> foo(5);	<-- internally this is formula_cell<int> foo([=] { return 5; });
	formula_cell<int> bar([&] { foo.get() + 2; });

	// Feature +2:
	formula_cell<int> foo(5);
	formula_cell<int> bar = ([&] { foo.get() + 2; });

	// Feature +3:
	formula_cell<int> foo = 5;
	formula_cell<int> bar = ([&] { foo.get() + 2; });
	
	// Feature +4:
	formula_cell<int> foo = 5;
	formula_cell<int> bar = foo + 2;

	// Feature +5:
	formula_cell<int> foo = lazy(5);		// can't use auto foo = 5  <-- foo would be an int
	formula_cell<int> bar = foo + 2;

	// Feature +6:
	auto foo = lazy(5);
	auto bar = foo + 2;

	// Feature +7:
	auto foo = lazy(5);
	auto bar = foo+2;
	auto comp = lazy_comp(foo,bar,">");

	// Feature +8: (Current Implementation)
	auto foo = lazy(5);
	foo += 2;					// value of foo is now 7

	// Feature +9:
	auto foo = lazy(5);
	auto bar = 2 * foo;
	auto foobar = foo / (bar + 1.f);		// value of foobar is ~5/11 (due to truncation)
	*/