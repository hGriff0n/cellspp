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

#pragma once

#include <functional>
#include <list>

namespace cells {

	// class that holds a history of values for a type
	template <typename T>
	class dynvar {
		private:
			std::list<T> value_stack;
		public:
			dynvar() : value_stack() {}					// default constructor
			dynvar(T val) : value_stack(1,val) {}		// value constructor
			dynvar<T>& operator=(T val) {				// changes the top list value
				if (!value_stack.empty())
					this->pop();
				this->push(val);
				return *this;
			}
			void push(T val) {							// wrapper over list::push_front
				value_stack.push_front(val);
			}
			void pop() {								// wrapper over list::pop_front
				value_stack.pop_front();
			}
			T& operator*() {							// reference the top list value
				return value_stack.front();
			}
			T* operator->() {					// pointer to the top list value
				return &value_stack.front();
			}
			operator bool() const {				// is the list empty
				return !value_stack.empty();
			}
	};

	// interface class for dynvar to control size through RAII
	template <typename R>
	class dyn {
			dynvar<R>& myvar;
		public:
			// confusion
			dyn(dynvar<R>& var, R val) : myvar(var) {   // what is this supposed to do?
				myvar.push(val);
			}
			~dyn() {
				myvar.pop();
			}
	};

	// where dyn and dynvar come together (how is it used in update)
	// creates a dyn and then calls f
	template <typename R, typename T>
	T with(dynvar<R>& var, R val, std::function<T ()> f) {
		dyn<R> d_(var, val);
		return f();
	}


}