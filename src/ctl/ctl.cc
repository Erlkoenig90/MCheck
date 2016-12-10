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

#include <boost/variant/apply_visitor.hpp>

#include "ctl.hh"

namespace CTL {

std::unique_ptr<S_Formula> SatVisitor::operator () (const E_Literal& exp) const {
	return nullary (exp, [&] () { return exp.value ? SatSet { tranSys.statesSet } : SatSet {}; });
}

std::unique_ptr<S_Formula> SatVisitor::operator () (const E_Negation& exp) const {
	return unary (exp, [&] (S_Formula& child) {
		return tranSys.statesSet - child.sat;
	});
}

std::unique_ptr<S_Formula> SatVisitor::operator () (const E_And& exp) const {
	return binary (exp, [&] (S_Formula& left, S_Formula& right) {
		return left.sat & right.sat;
	});
}

std::unique_ptr<S_Formula> SatVisitor::operator () (const E_Or& exp) const {
	return binary (exp, [&] (S_Formula& left, S_Formula& right) {
		return left.sat | right.sat;
	});
}

std::unique_ptr<S_Formula> SatVisitor::operator() (const E_Implication& exp) const {
	return binary (exp, [&] (S_Formula& left, S_Formula& right) {
		return (tranSys.statesSet - left.sat) | right.sat;
	});
}


std::unique_ptr<S_Formula> SatVisitor::operator () (const E_ExistNext& exp) const {
	return unary (exp, [&] (S_Formula& child) {
		std::set<TS::State*> predecessors;
		for (auto& s : child.sat) {
			predecessors |= s->predecessors;
		}

		return predecessors;
	});
}

std::unique_ptr<S_Formula> SatVisitor::operator () (const E_ExistUntil& exp) const {
	return binary (exp, [&] (S_Formula& left, S_Formula& right) {
		std::set<TS::State*> T = right.sat;
		bool modified;
		do {
			modified = false;
			for (TS::State* s : T) {
				for (TS::State* p : s->predecessors) {
					if (left.sat.find (p) != left.sat.end () && T.find (p) != T.end ()) {
						T.insert (p);
						modified = true;
					}
				}
			}
		} while (modified);

		return T;
	});
}

std::unique_ptr<S_Formula> SatVisitor::operator () (const E_ExistAlways& exp) const {
	return unary (exp, [&] (S_Formula& child) {
		std::set<TS::State*> T = child.sat;
		bool modified;
		do {
			modified = false;
			for (TS::State* s : T) {
				if (!(s->successors && T)) {
					T.erase (s);
					modified = true;
				}
			}
		} while (modified);

		return T;
	});
}

std::unique_ptr<S_Formula> SatVisitor::operator() (const E_AllNext& exp) const {
	return unary (exp, [&] (S_Formula& child) {
		std::set<TS::State*> exclusivePredecessors;
		for (auto& s : child.sat) {
			for (auto& p : s->predecessors) {
				if (p->successors <= child.sat)
					exclusivePredecessors.insert (p);
			}
		}

		return exclusivePredecessors;
	});
}

std::unique_ptr<S_Formula> SatVisitor::operator() (const E_AllUntil& exp) const {
	return binary (exp, [&] (S_Formula& left, S_Formula& right) {
		std::set<TS::State*> T = right.sat;
		bool modified;
		do {
			modified = false;
			for (TS::State* s : T) {
				for (TS::State* p : s->predecessors) {

					if (left.sat.find (p) != left.sat.end ()
					&&	s->successors <= T
					&&	T.find (p) != T.end ()) {
						T.insert (p);
						modified = true;
					}
				}
			}
		} while (modified);

		return T;
	});
}

std::unique_ptr<S_Formula> SatVisitor::operator() (const E_AllAlways& exp) const {
	return unary (exp, [&] (S_Formula& child) {
		std::set<TS::State*> T = child.sat;
		bool modified;
		do {
			modified = false;
			for (TS::State* s : T) {
				if (!(s->successors <= T)) {
					T.erase (s);
					modified = true;
				}
			}
		} while (modified);

		return T;
	});
}


std::unique_ptr<S_Formula> SatVisitor::operator () (const E_Label& exp) const {
	return nullary (exp, [&] () {
		auto res = tranSys.labels.find (exp.name);
		if (res == tranSys.labels.end ())
			throw std::runtime_error ("Unknown label \"" + exp.name + "\" in formula");

		std::set<TS::State*> sat;
		for (const auto s : tranSys.statesSet) {
			if (s->atomicPropositions.find (&res->second) != s->atomicPropositions.end ())
				sat.insert (s);
		}

		return sat;
	});
}

std::pair<bool, std::unique_ptr<S_Formula>> computeSat (const Expression& formula, TS::TranSys& ts) {
	std::unique_ptr<S_Formula> res = boost::apply_visitor (SatVisitor { ts }, formula);
	bool s = ts.init <= res->sat;
	return std::make_pair (s, std::move (res));
}

void S_Formula::print (std::ostream& os, size_t depth) {
	for (size_t i = 0; i < depth; ++i) os << '\t';
	os << "Sat (";
	std::copy (strBegin, strEnd, std::ostreambuf_iterator<char> (os));
	os << ") = ";
}

void S_Nullary::print (std::ostream& os, size_t depth) {
	S_Formula::print (os, depth);
	os << sat << std::endl;
}

void S_Unary::print (std::ostream& os, size_t depth) {
	S_Formula::print (os, depth);
	os << sat << std::endl;
	child->print (os, depth+1);
}

void S_Binary::print (std::ostream& os, size_t depth) {
	S_Formula::print (os, depth);
	os << sat << std::endl;
	left->print (os, depth+1);
	right->print (os, depth+1);
}

std::ostream& operator << (std::ostream& os, const SatSet& sat) {
	os << '{';
	bool next = false;
	for (SatSet::const_iterator i = sat.cbegin (); i != sat.cend (); ++i) {
		if (next) {
			os << ", ";
		} else {
			next = true;
		}
		os << (*i)->name;
	}
	os << '}';
	return os;
}

}
