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
	formula_cell<T> reactive(T val) {
		return val;
	};

	/*
	template <typename T>
	formula_cell<T> reactive(const std::function<T ()>& val) {
		return val;
	};
	*/
}

// Formula_cell expression overloads
// works, but relies on the formula_cell staying alive after the operation
// if the formula_cell& gets deleted, value becomes 'permanent'
// fine for normal use, but possibly difficult for chaining of temporary formula_cells

// Note: Formula_cell expression type conversions		given: (lhs {op} rhs)	(ex. lhs + rhs)
// Operations between a formula_cell<T> and a value of R will result in a formula_cell<typeof(rhs)>

template <typename T,typename R>
cells::formula_cell<R> operator*(cells::formula_cell<T>& rhs,R lhs) {
	// capture cell by reference (ensure that the new cell gets updated according to the passed cell and not a copy cell)
	// capture parameter by value (keep the parameter's value as part of the function)
	return [=,&rhs] { return (R)rhs.get() * lhs; };
}
template <typename T,typename R>
cells::formula_cell<R> operator*(T rhs,cells::formula_cell<R>& lhs) {
	return [=,&lhs] { return lhs.get() * (R)rhs; };
}

template <typename T,typename R>
cells::formula_cell<R> operator+(cells::formula_cell<T>& rhs,R lhs) {
	return [=,&rhs] { return (R)rhs.get() + lhs; };
}
template <typename T,typename R>
cells::formula_cell<R> operator+(T rhs,cells::formula_cell<R>& lhs) {
	return [=,&lhs] { return lhs.get() + (R)rhs; };
}

template <typename T,typename R>
cells::formula_cell<R> operator-(cells::formula_cell<T>& rhs,R lhs) {
	return [=,&rhs] { return (R)rhs.get() - lhs; };
}
template <typename T,typename R>
cells::formula_cell<R> operator-(T rhs,cells::formula_cell<R>& lhs) {
	return [=,&lhs] { return lhs.get() - (R)rhs; };
}

template <typename T,typename R>
cells::formula_cell<R> operator/(cells::formula_cell<T>& rhs,R lhs) {
	return [=,&rhs] { return (R)rhs.get() / lhs; };
}
template <typename T,typename R>
cells::formula_cell<R> operator/(T rhs,cells::formula_cell<R>& lhs) {
	return [=,&lhs] { return lhs.get() / (R)rhs; };
}