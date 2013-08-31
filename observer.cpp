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

#include "observer.h"

#include <algorithm>
#include <exception>
#include <stdexcept>
//#include <iostream>
#include <deque>

namespace cells {

	observer::observer() : self(std::make_shared<observer*>(this)) {}

	observer::observer(observer const& other) : self(std::make_shared<observer*>(this)) {
		reset_dependents(other.dependents);
		reset_dependencies(other.dependencies);
	}

	observer& observer::operator=(observer const& other) {
	    reset_dependents(other.dependents);
		reset_dependencies(other.dependencies);
		return *this;
	}

	void observer::reset_dependents(std::list<std::weak_ptr<observer*>> const& new_dependents) {
		dependents = new_dependents;
	}
	
	void observer::clear_dependencies() {
		for (auto const& dep : dependencies) {
			std::shared_ptr<observer*> sdep = dep.lock();
			if (sdep)
				(*sdep)->remove_dependent(this);
		}
		dependencies.empty();
	}

	void observer::add_dependent(observer* dependent) {
		dependents.push_front(dependent->self);
	}

	void observer::remove_dependent(observer* dependent) {
		dependents.remove_if([&](std::weak_ptr<observer*> const& other) -> bool {
			std::shared_ptr<observer*> other2 = other.lock();
			return (!other2 || *other2 == dependent);
		});
	}
  
	void observer::reset_dependencies(std::forward_list<std::weak_ptr<observer*>> const& newdeps)  {
		clear_dependencies();
		dependencies = newdeps;
		for (auto const& dep : newdeps) {
			std::shared_ptr<observer*> p = dep.lock();
			if (p) 
				(*p)->add_dependent(this);
		}
	}

	void observer::reset_dependencies(std::forward_list<observer*> const& newdeps) {
	    clear_dependencies();
		for (auto const& dep : newdeps) {
			dependencies.push_front(dep->self);
			dep->add_dependent(this);
		}
	}

	observer::~observer() {
		clear_dependencies();
	}

	void observer::mark() {
		using namespace internal;
		if (!internal::current_transaction) {
			with_transaction([&]() { this->mark(); });
			return;
		}
		if (current_transaction->dag.find(this) == current_transaction->dag.end()) {
			dag_node* node = new dag_node(this);
			current_transaction->dag[this] = node;
			for (auto e = dependents.cbegin(); e != dependents.end(); e++) {
				std::weak_ptr<observer*> const& x = *e;
				std::shared_ptr<observer*> px = x.lock();
				if (px) {
					(*px)->mark();
					dag_node* xn = current_transaction->dag[*px];
					xn->incoming_edges.insert(node);
					node->outgoing_edges.insert(xn);
				} else {
					// Purge dead nodes
					dependents.erase(e);
				}
			}
		}
	}
	
	void with_transaction(std::function<void ()> thunk) {
		using namespace internal;
		//using std::cout; using std::cerr; using std::endl;
		with<transaction, void>(current_transaction, transaction(), [&]() -> void {
			//cerr << "; begin transaction." << endl;
			thunk();

			//cerr << "; number of affected nodes: " << current_transaction->dag.size() << endl;
			std::deque<observer*> nodes;

			// topological sort
			std::forward_list<dag_node*> independent_nodes;
			auto left = current_transaction->dag.size();

			for (auto const& o_and_n : current_transaction->dag) {
				auto node = o_and_n.second;
				if (node->incoming_edges.size() == 0)
					independent_nodes.push_front(node);
			}
			while (!independent_nodes.empty()) {
				left--;
				auto node = independent_nodes.front();
				independent_nodes.pop_front();
				nodes.push_back(node->item);
				// Or we can do away with the nodes list and just do:
				//    node->item->update()
				// which makes transactions non-transactional but improves
				// performance.
				for (dag_node* other : node->outgoing_edges) {
					other->incoming_edges.erase(node);
					if (other->incoming_edges.size() == 0)
						independent_nodes.push_front(other);
				}
				delete node;
			}
			if (left != 0) {
				// FIXME: delete remaining dag_nodes.
				throw std::logic_error("Cell cycle detected");
			}

			for (auto const& node : nodes) 
				node->update();
		});
  }
}