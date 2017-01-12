/*
 * Copyright (c) 2016, Niklas Gürtler
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 *    disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *    following disclaimer in the documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SRC_TS_SYS_HH_
#define SRC_TS_SYS_HH_

#include <set>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>

#include "ts_ast.hh"

namespace TS {
	class Label {
		public:
			std::string name;
	};
}

namespace std {
	template <>
	struct hash<::TS::Label> {
		size_t operator() (const ::TS::Label &l) const {
			return std::hash<std::string> () (l.name);
		}
	};
}

namespace TS {
	class State {
		public:
			inline State (const std::string& name_) : name (name_) {}
			std::string name;
			std::set<Label*> atomicPropositions;
			std::set<State*> predecessors, successors;
	};
	struct CompareStatePtr {
		bool operator () (const State* lhs, const State* rhs) {
			return lhs->name < rhs->name;
		}
	};

	class TranSys {
		public:
			std::map<std::string, Label> labels;
			std::map<std::string, State> statesMap;
			std::set<State*> statesSet, init;

			TranSys (const E_Graph& src);
	};
}

#endif /* SRC_TS_SYS_HH_ */
