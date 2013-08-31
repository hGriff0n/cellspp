

#pragma once

#include "dynvars.h"

#include <memory>
#include <forward_list>
#include <unordered_map>
#include <unordered_set>


// undefine to use thread local storage for thread_local data when tls is feasible
// #define threaded_storage
#ifndef thread_local
	#ifdef threaded_storage
		#ifdef _MSC_VER
			#define thread_local _declspec(thread)
		#else
			#define thread_local __thread
		#endif
	#else
		#define thread_local
	#endif
#endif

namespace cells {

	//
	static void with_transaction(std::function<void ()>);

	// interface class that holds a list of dependents and dependencies
	class observer {
		private:
			std::shared_ptr<observer*> const self;
			std::list<std::weak_ptr<observer*>> dependents;
			std::forward_list<std::weak_ptr<observer*>> dependencies;

			void clear_dependencies();
			void mark_dependents();
	
			void reset_dependencies(std::forward_list<std::weak_ptr<observer*>> const& new_dependents);
			void reset_dependents(std::list<std::weak_ptr<observer*>> const&);

		protected:
			void mark();

		public:
			observer();
			observer(observer const& other);

			observer& operator =(observer const& other);

			void add_dependent(observer* dependent);
			void remove_dependent(observer* dependent);
			void reset_dependencies(std::forward_list<observer*> const&);

			virtual void update() = 0;

			virtual ~observer();
	};

	namespace internal {
		struct dag_node {
			dag_node(observer* item_) : item(item_) { }
			observer* item;
			std::unordered_set<dag_node*> incoming_edges;
			std::unordered_set<dag_node*> outgoing_edges;
		};

		struct transaction {
			std::unordered_map<observer*, dag_node*> dag;
		};

		// forward declare current_dependencies
		static thread_local dynvar<std::forward_list<observer*>> current_dependencies;
		static thread_local dynvar<transaction> current_transaction;

	}

}