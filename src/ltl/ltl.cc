/*
 * Copyright (c) 2017, Niklas Gürtler
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

#include <climits>
#include <iostream>
#include <map>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <map>

#include "ltl.hh"
#include <boost/functional/hash.hpp>
#include <boost/fusion/include/hash.hpp>


std::ostream& LTL::operator << (std::ostream& os, const LTL::ExpSet& c) {
	if (os.iword (Formula::Formula_Xalloc) == 2) os << "\\left\\{";
	else os << "{";
	bool first = true;
	for (auto& exp : c) {
		if (!first)
			os << ", ";
		else
			first = false;
		os << *exp;
	}
	if (os.iword (Formula::Formula_Xalloc) == 2) os << "\\right\\}";
	else os << '}';
	return os;
}

LTL::Algorithm::Algorithm (const Formula::Expression& exp, const TS::TranSys& ts) :formula (exp) {
	ClosureVisitor visitor { closure, auxExp, &exp};
	boost::apply_visitor (visitor, exp);

	using Counter = std::uint64_t;

	if (closure.size () >= sizeof (Counter) * CHAR_BIT)
		throw std::runtime_error ("Too many closure elements!");

	const Counter max = (1 << closure.size ()) - 1;

	atomExpressions.push_back ({});

	for (Counter iComb = 0; iComb < max; ++iComb) {
		ExpSet& atom = atomExpressions.back ();

		Counter mask = 1;
		for (auto& cexp : closure) {
			if (iComb & mask) {
				atom.insert (cexp);
			}

			mask <<= 1;
		}
		bool needed = false;
		for (auto& state : ts.statesSet) {
			Consistency_ClosureVisitor v (atom, ts, *state);
			bool consistent = std::all_of (closure.begin (), closure.end (), [&](const Formula::ExpRef& elem) { v.m_parentExp = elem; return ::boost::apply_visitor (v, *elem); });

			if (consistent) {
				needed = true;
				atomMap.insert (std::make_pair<const TS::State*, std::size_t> (state, atoms.size ()));
				atoms.emplace_back (state, atomExpressions.size () - 1);
			}
		}
		if (needed) {
			atomExpressions.push_back ({});
		} else {
			atom.clear ();
		}
	}
	atomExpressions.pop_back ();

	for (std::size_t iStartAtom = 0; iStartAtom < atoms.size (); ++iStartAtom) {
		Atom& startAtom = atoms [iStartAtom];
		for (const TS::State* succ : startAtom.state->successors) {
			auto endRange = atomMap.equal_range (succ);
			for (auto iter = endRange.first; iter != endRange.second; ++iter) {
				std::size_t iEndAtom = iter->second;
				const Atom& endAtom = atoms [iEndAtom];

				const ExpSet& startExp = atomExpressions [startAtom.expressions];
				const ExpSet& endExp = atomExpressions [endAtom.expressions];

				if (std::all_of (closure.begin (), closure.end (), [&] (const Formula::ExpRef& elem) {
					if (const Formula::E_Next* next = boost::get<Formula::E_Next> (&*elem)) {
						return	(startExp.find (elem) == startExp.end ())
							==	(endExp.find(&next->exp) == endExp.end ());
					} else {
						return true;
					}
				})) {
					edges.emplace_back (iStartAtom, iEndAtom);
				}
			}
		}
	}
}

bool LTL::Consistency_ClosureVisitor::operator () (const Formula::E_Label& exp) {
	auto iter = m_tranSys.labels.find (exp.name);
	if (iter == m_tranSys.labels.end ())
		throw std::runtime_error ("Unknown label: " + exp.name);

	const bool inState = m_state.atomicPropositions.find (const_cast<TS::Label*>(&iter->second)) == m_state.atomicPropositions.end ();
	const bool inAtom = m_atom.find (m_parentExp) == m_atom.end ();
	return inState == inAtom;
}

bool LTL::Consistency_ClosureVisitor::operator () (const Formula::E_Literal& exp) {
	return (m_atom.find (m_parentExp) != m_atom.end ()) == exp.value;
}

bool LTL::Consistency_ClosureVisitor::operator () (const Formula::E_Negation&) {
	throw std::runtime_error ("Negation in closure!");
}

bool LTL::Consistency_ClosureVisitor::operator () (const Formula::E_And& exp) {
	return	(containsConsistent (exp.lhs)	&&	containsConsistent (exp.rhs))
		==	(m_atom.find (m_parentExp) != m_atom.end ());
}

bool LTL::Consistency_ClosureVisitor::operator () (const Formula::E_Or& exp) {
	return	(containsConsistent (exp.lhs)	||	containsConsistent (exp.rhs))
		==	(m_atom.find (m_parentExp) != m_atom.end ());
}

bool LTL::Consistency_ClosureVisitor::operator () (const Formula::E_Implication& exp) {
	return	(!containsConsistent (exp.lhs)	||	containsConsistent (exp.rhs))
		==	(m_atom.find (m_parentExp) != m_atom.end ());
}

bool LTL::Consistency_ClosureVisitor::operator () (const Formula::E_Until& exp) {
	Formula::Expression next { Formula::E_Next { {}, exp, {}}};
	return	(m_atom.find (m_parentExp) != m_atom.end ())
		==	(
				containsConsistent (exp.rhs)
			||	(containsConsistent (exp.lhs) && m_atom.find (&next) != m_atom.end ())
			);
}

bool LTL::Consistency_ClosureVisitor::operator () (const Formula::E_Next&) {
	return true;
}

int LTL::ClosureVisitor::operator () (const Formula::E_Negation& exp) {
	m_parentExp = &exp.exp;
	boost::apply_visitor (*this, exp.exp);
	return 0;
}

int LTL::ClosureVisitor::operator () (const Formula::E_Until& exp) {
	m_closure.insert (m_parentExp);

	m_auxExp.emplace_back (Formula::E_Next ({}, exp, {}));
	m_closure.insert (&m_auxExp.back ());

	m_parentExp = &exp.lhs;
	boost::apply_visitor (*this, exp.lhs);
	m_parentExp = &exp.rhs;
	boost::apply_visitor (*this, exp.rhs);
	return 0;
}

int LTL::ClosureVisitor::operator () (const Formula::E_Literal&) {
	m_closure.insert (m_parentExp);
	return 0;
}

int LTL::ClosureVisitor::operator () (const Formula::E_Label&) {
	m_closure.insert (m_parentExp);
	return 0;
}
