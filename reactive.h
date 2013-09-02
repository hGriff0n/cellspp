// Copyright 2012, Matthias Andreas Benkard.
//
// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.

// Copied and Edited: 2013, Grayson Andrew Hooper
// Source from original library "benkard/cellspp"
//		found at https://github.com/benkard/cellspp
// Some feature additons and various use improvements

#pragma once

#include "observer.h"

namespace {
	template <typename ch>
	ch char_sum(const ch* text,size_t N) {
		ch tot=0;
		for (auto i=0;i!=N-1;++i) tot += text[i];
		return tot;
	}
}

namespace cells {

	// interface class that holds a value state
	template <typename T>
	class cell : public observer {
		private:
			T current_value;

		protected:
			cell() {}
			virtual void update() {
				using namespace internal;
				T oldval = current_value;
				with<std::forward_list<observer*>, void>(current_dependencies,std::forward_list<observer*>(),[=]{
					current_value = recompute(current_value);
					reset_dependencies(*current_dependencies);
				});
			}
			virtual T recompute(T) = 0;
			virtual T init() = 0;

		public:
			T& get() {
				using namespace internal;
				if (current_dependencies) {
					current_dependencies->push_front(this);
					return current_value;
				} else
					return current_value;
			}
			T& operator *() { return get(); }
			operator T&() { return get(); };

			virtual ~cell() {}
	};

	// computes a value from a provided function reactively
	template <typename T>
	class formula_cell : public cell<T> {
	    //friend class std::shared_ptr<formula_cell<T>>;
		//friend std::shared_ptr<formula_cell<T>> std::make_shared<formula_cell<T>>();
		private:
			std::function<T ()> formula;
			std::function<T (T old)> alt_formula;

		protected:
			virtual T recompute(T old)  {
				return alt_formula(old);
			}
			virtual T init() {
				return formula();
			}

		public:
			formula_cell() {}
			formula_cell(T value)
				: formula([=] { return value; }),alt_formula([=](T) { return value; }) {
				this->mark();
			}
			formula_cell(std::function<T ()> _formula)
				: formula(_formula),alt_formula([=](T) { return formula(); }) {
				this->mark();
			}
			formula_cell(std::function<T ()> _formula,std::function<T (T)> _altFormula)
				: formula(_formula),alt_formula(_altFormula) {
				this->mark();
			}

			void reset(T value) {
				this->alt_formula = ([=](T) { return value; });
				this->formula     = ([=] { return value; });
				this->mark();
			}
			void reset(std::function<T ()> formula) {
				this->formula = formula;
				this->alt_formula = ([=](T old) { return formula(); });
				this->mark();
			}
			void reset(std::function<T ()> formula,std::function<T (T)> alt_formula) {
				this->formula = formula;
				this->alt_formula = alt_formula;
				this->mark();
			}

			formula_cell<T>& operator=(T value) {
				this->reset(value);
				return *this;
			}
			formula_cell<T>& operator=(std::function<T ()>& _func) {
				this->reset(_func);
				return *this;
			}
			formula_cell<T>& operator=(formula_cell<T>& _cell) {
				this->reset(_cell.formula);
				return *this;
			}

			virtual ~formula_cell() {}
	};	



	// helper function that enables use of auto when initializing a formula_cell with a constant value
	// saying auto foo = 5 won't work as the type of foo would be int and not formula_cell<int>
	template <typename T>
	formula_cell<T> lazy(T val) {
		return val;
	};
	/*
	can't infer the template type from a lambda
	template <typename T>
	formula_cell<T> lazy_func(std::function<T ()> val) {
		return val;
	};
	*/

	// reactive compare of a function_cell and another variable based on a passed char* comparison code	"lazy compare"
	// accepted char* comparison codes: "==","!=","<","<=",">",">="
	template <typename T,typename R>
	formula_cell<bool> lazy_comp(formula_cell<T>& lhs,formula_cell<R>& rhs,const char* func,size_t N) {
		/*
		// Can't do switch(func) {	-- not allowed by the compilier
		// Can do switch(sum(func)) { since chars are just numbers
		// The sum of the passed characters will be unique to other accepted codes
		// Note: Function will actually accept any sequence of characters that add up to accepted values for sw_char
		*/

		auto sw_char = ::char_sum(func,N);

		switch(sw_char) {   // formula_cells are implicitly convertible
			case 'z':	// '=' + '=' is 'z'	(in char codes)
				return [&] { return lhs == (T)rhs; };
			case '^':	// '!' + '=' is '^'	(in char codes)
				return [&] { return lhs != (T)rhs; };
			case '>':
				return [&] { return lhs > (T)rhs; };
			case '{':	// '>' + '=' is '{'  (in char codes)
				return [&] { return lhs >= (T)rhs; };
			case '<':
				return [&] { return lhs < (T)rhs; };
			case 'y':	// '<' + '=' is 'y'	(in char codes)
				return [&] { return lhs <= (T)rhs; };
			default:	// defaults to false if the passed code is not recognized
				return false;
		}
	}
	// overload for static char arrays
	template <typename T,typename R,size_t N>
	formula_cell<bool> lazy_comp(formula_cell<T>& lhs,formula_cell<R>& rhs,const char (&func) [N]) {
		return lazy_comp(lhs,rhs,func,N);
	}
	// overload for right value comparison
	template <typename T,typename R>
	formula_cell<bool> lazy_comp(formula_cell<T>& lhs,R rhs,const char* func,size_t N) {
		auto tot = ::char_sum(func,N);

		switch(tot) {
			case 'z':
				return [=,&lhs] { return lhs == (T)rhs; };
			case '^':
				return [=,&lhs] { return lhs != (T)rhs; };
			case '>':
				return [=,&lhs] { return lhs > (T)rhs; };
			case '{':
				return [=,&lhs] { return lhs >= (T)rhs; };
			case '<':
				return [=,&lhs] { return lhs < (T)rhs; };
			case 'y':
				return [=,&lhs] { return lhs <= (T)rhs; };
			default:
				return false;
		}
	}
	template <typename T,typename R,size_t N>
	formula_cell<bool> lazy_comp(formula_cell<T>& lhs,R rhs,const char (&func) [N]) {
		return lazy_comp(lhs,rhs,func,N);
	}
	// overload for left value comparison
	template <typename T,typename R>
	formula_cell<bool> lazy_comp(T lhs,formula_cell<R>& rhs,const char* func,size_t N) {
		auto tot = ::char_sum(func,N);

		switch(tot) {
			case 'z':
				return [=,&rhs] { return lhs == (T)rhs; };
			case '^':
				return [=,&rhs] { return lhs != (T)rhs; };
			case '>':
				return [=,&rhs] { return lhs > (T)rhs; };
			case '{':
				return [=,&rhs] { return lhs >= (T)rhs; };
			case '<':
				return [=,&rhs] { return lhs < (T)rhs; };
			case 'y':
				return [=,&rhs] { return lhs <= (T)rhs; };
			default:
				return false;
		}
	}
	template <typename T,typename R,size_t N>
	formula_cell<bool> lazy_comp(T lhs,formula_cell<R>& rhs,const char (&func) [N]) {
		return lazy_comp(lhs,rhs,func,N);
	}

}

// Formula_cell expression overloads
// works, but relies on the formula_cell staying alive after the operation
// if the formula_cell& gets deleted, value becomes 'permanent'
// fine for normal use, but possibly difficult for chaining of temporary formula_cells
// Can currently add two formula_cells together but cannot chain expressions

// Have all operators return formula_cell<R>&&	?
// Have overloads that accept the formula_cell<R>&& as is	?

// Note: Formula_cell expression type conversions		given: (lhs {op} rhs)	(ex. lhs + rhs)
// Operations between a formula_cell<T> and a value of R will result in a formula_cell<typeof(rhs)>

template <typename T,typename R>
cells::formula_cell<R> operator*(cells::formula_cell<T>& lhs,R rhs) {
	// capture cell by reference (ensure that the new cell gets updated according to the passed cell and not a copy cell)
	// capture parameter by value (keep the parameter's value as part of the function)
	return [=,&lhs] { return (R)lhs.get() * rhs; };
}
template <typename T,typename R>
cells::formula_cell<R> operator*(T lhs,cells::formula_cell<R>& rhs) {
	return [=,&rhs] { return (R)lhs * rhs.get(); };
}
template <typename T,typename R>
cells::formula_cell<R> operator*(cells::formula_cell<T>& rhs,cells::formula_cell<R>& lhs) {
	return [&] { return (R)lhs.get() * rhs.get(); };
}

template <typename T,typename R>
cells::formula_cell<R> operator+(cells::formula_cell<T>& lhs,R rhs) {
	return [=,&lhs] { return (R)lhs.get() + rhs; };
}
template <typename T,typename R>
cells::formula_cell<R> operator+(T lhs,cells::formula_cell<R>& rhs) {
	return [=,&rhs] { return (R)lhs + rhs.get(); };
}
template <typename T,typename R>
cells::formula_cell<R> operator+(cells::formula_cell<T>& rhs,cells::formula_cell<R>& lhs) {
	return [&] { return (R)lhs.get() + rhs.get(); };
}
/*
cell<T>&&,R
cell<T>&&,cell<R>&
cell<T>&&,cell<R>&&
T,cell<R>&&
cell<T>&,cell<R>&&
*/

template <typename T,typename R>
cells::formula_cell<R> operator-(cells::formula_cell<T>& lhs,R rhs) {
	return [=,&lhs] { return (R)lhs.get() - rhs; };
}
template <typename T,typename R>
cells::formula_cell<R> operator-(T lhs,cells::formula_cell<R>& rhs) {
	return [=,&rhs] { return (R)lhs - rhs.get(); };
}
template <typename T,typename R>
cells::formula_cell<R> operator-(cells::formula_cell<T>& rhs,cells::formula_cell<R>& lhs) {
	return [&] { return (R)lhs.get() - rhs.get(); };
}

template <typename T,typename R>
cells::formula_cell<R> operator/(cells::formula_cell<T>& lhs,R rhs) {
	return [=,&lhs] { return (R)lhs.get() / rhs; };
}
template <typename T,typename R>
cells::formula_cell<R> operator/(T lhs,cells::formula_cell<R>& rhs) {
	return [=,&rhs] { return (R)lhs / rhs.get(); };
}
template <typename T,typename R>
cells::formula_cell<R> operator/(cells::formula_cell<T>& rhs,cells::formula_cell<R>& lhs) {
	return [&] { return (R)lhs.get() + rhs.get(); };
}

template <typename T,typename R>
cells::formula_cell<T>& operator+=(cells::formula_cell<T>& lhs,R value) {
	lhs = value + lhs;
	return lhs;
}
template <typename T,typename R>
cells::formula_cell<T>& operator-=(cells::formula_cell<T>& lhs,R value) {
	lhs = lhs - (T)value;
	return lhs;
}
template <typename T,typename R>
cells::formula_cell<T>& operator/=(cells::formula_cell<T>& lhs,R value) {
	lhs = lhs / (T)value ;
	return lhs;
}
template <typename T,typename R>
cells::formula_cell<T>& operator*=(cells::formula_cell<T>& lhs,R value) {
	lhs = value * lhs;
	return lhs;
}
