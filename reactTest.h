// Feature Work: assign in constructor
	
	// implemented
	// copy & move constructors not made
	// formula_cell<int> cVar(3);
	// formula_cell<int> fVar([=] { cVar.get() / 3; });


	// Feature Work: assign from function
	
	// implemented
	// rint fVar = ([=] { rVar.get() / 3; });

	/*
	template <typename T>
	formula_cell<T>& operator=(formula_cell<T>& _cell,std::function<T ()>&& _func) {
		_func.swap(_cell.formula);
		_cell.alt_formula = ([=](T old) { return formula(); });
		_cell.mark();
		return _cell;
	}
	*/


	// Feature work : assign from value

	// implemented
	// auto temp = reactive(5);
	// temp = 7;


	// Feature work : assign from expression

	// implemented (partially) - unsure about chaining expressions	eg. auto tVar = oVar * dVar - aVar;
	// not possible to express the results of two formula_cells

	// formula_cell<int> oVar(5);
	// formula_cell<int> aVar = oVar + 6;			--> formula_cell<int> aVar([=] { return oVar.get() + 6; });
	// formula_cell<float> dVar = oVar / 2.5;		--> formula_cell<float> dVar([=] { return oVar.get() / 2.5; });

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

	// Feature work : assign from value with auto

	// implemented
	// formula_cell<int> foo = reactive(5);		--> formula_cell<int> foo(5);


	// Feature work : boolean operators

	// completed (theoretically) - formula_cells are implicitly convertible to their value types
	// operators to compare between two formula_cells
	// function provided to created a reactive compare of two variables
	// 

	/*
	template <typename T,typename R>
	bool operator==(formula_cell<T>& lhs,formula_cell<R>& rhs) {
		return (lhs.get() == (T)rhs.get());
	};
	template <typename T,typename R>
	bool operator!=(formula_cell<T>& lhs,formula_cell<R>& rhs) {
		return (lhs.get() != (T)rhs.get());
	};
	template <typename T,typename R>
	bool operator<(formula_cell<T>& lhs,formula_cell<R>& rhs) {
		return (lhs.get() < (T)rhs.get());
	};
	template <typename T,typename R>
	bool operator<=(formula_cell<T>& lhs,formula_cell<R>& rhs) {
		return (lhs.get() <= (T)rhs.get());
	};
	template <typename T,typename R>
	bool operator>(formula_cell<T>& lhs,formula_cell<R>& rhs) {
		return (lhs.get() > (T)rhs.get());
	};
	template <typename T,typename R>
	bool operator>=(formula_cell<T>& lhs,formula_cell<R>& rhs) {
		return (lhs.get() >= (T)rhs.get());
	};
	template <typename T,typename R>
	std::function<bool ()> compare(formula_cell<T>& lhs,formula_cell<R>& rhs,std::function<bool (T&,T&)>& _func) {
		return ([=] { return _func(lhs.get(),rhs.get()); });
	};
	
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
	formula_cell<int> foo = reactive(5);		// can't use auto foo = 5  <-- foo would be an int
	formula_cell<int> bar = foo + 2;

	// Feature +6: (Current Implementation)
	auto foo = reactive(5);
	auto bar = foo + 2;

	// Feature +7: (Current Design)
	auto foo = reactive(5);
	auto bar = foo+2;
	auto comp = compare(foo,bar,[](decltype(foo.get())& x,decltype(foo.get())& y) { return x > y; });	// can't have polymorphic lambdas yet
	//auto comp = compare(foo,bar,[](auto& x,auto& y) { return x > y; };

	*/