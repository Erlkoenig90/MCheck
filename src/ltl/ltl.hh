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

#ifndef LTL_HH_
#define LTL_HH_

#include <cstddef>
#include <stdexcept>
#include <unordered_set>
#include <map>
#include <vector>
#include "../formula_ast.hh"
#include "../ts/sys.hh"

namespace LTL {
	using ExpSet = std::unordered_set<Formula::ExpRef, boost::hash<Formula::ExpRef>>;

	struct Atom {
		inline Atom (const TS::State* s, std::size_t exp) : state (s), expressions (exp), name (s->name + '_' + std::to_string (exp)) {}
		const TS::State* state;
		std::size_t expressions; // Index into Algorithm::atomExpressions
		std::string name;
	};
	struct TableauEdge {
		inline TableauEdge (std::size_t s, std::size_t e) : start (s), end (e) {}
		std::size_t start, end;
	};
	struct Algorithm {
		Algorithm (const Formula::Expression& exp, const TS::TranSys& sys);

		ExpSet closure;
		const Formula::Expression& formula;
		std::list<Formula::Expression> auxExp;
		std::vector<ExpSet> atomExpressions;
		std::vector<Atom> atoms;
		std::multimap<const TS::State*, std::size_t, TS::CompareStatePtr> atomMap;
		std::vector<TableauEdge> edges;
	};

	std::ostream& operator << (std::ostream& os, const LTL::ExpSet& c);

	class ClosureVisitor : public boost::static_visitor<int> {
		private:
			ExpSet& m_closure;
			std::list<Formula::Expression>& m_auxExp;
			const Formula::Expression* m_parentExp;
		public:
			inline ClosureVisitor (ExpSet& c, std::list<Formula::Expression>& ae, const Formula::Expression* parentExp) : m_closure (c), m_auxExp (ae), m_parentExp (parentExp) {}

			template <typename T>
			typename std::enable_if<Formula::ExpTraits<T>::unary && Formula::ExpTraits<T>::ltl, int>::type operator () (const T& exp) {
				m_closure.insert (m_parentExp);

				m_parentExp = &exp.exp;
				boost::apply_visitor (*this, exp.exp);
				return 0;
			}
			template <typename T>
			typename std::enable_if<Formula::ExpTraits<T>::binary && Formula::ExpTraits<T>::ltl, int>::type operator () (const T& exp) {
				m_closure.insert (m_parentExp);

				m_parentExp = &exp.lhs;
				boost::apply_visitor (*this, exp.lhs);
				m_parentExp = &exp.rhs;
				boost::apply_visitor (*this, exp.rhs);
				return 0;
			}

			int operator () (const Formula::E_Literal& exp);
			int operator () (const Formula::E_Label& exp);
			int operator () (const Formula::E_Negation& exp);
			int operator () (const Formula::E_Until& exp);

			template <typename T>
			typename std::enable_if<!Formula::ExpTraits<T>::ltl, int>::type operator () (const T&) {
				throw std::runtime_error (std::string ("Illegal node in AST: ") + Formula::ExpTraits<T>::name);
			}
	};

	class Consistency_ClosureVisitor : public boost::static_visitor<bool> {
		private:
			const ExpSet& m_atom;
			const TS::TranSys& m_tranSys;
			const TS::State& m_state;
		public:
			Formula::ExpRef m_parentExp;
			inline Consistency_ClosureVisitor (const ExpSet& atom, const TS::TranSys& ts, const TS::State& state) : m_atom (atom), m_tranSys (ts), m_state (state) {}

			bool operator () (const Formula::E_Label& exp);
			bool operator () (const Formula::E_Literal& exp);
			bool operator () (const Formula::E_Negation& exp);
			bool operator () (const Formula::E_And& exp);
			bool operator () (const Formula::E_Or& exp);
			bool operator () (const Formula::E_Implication& exp);
			bool operator () (const Formula::E_Until& exp);
			bool operator () (const Formula::E_Next& exp);

			template <typename T>
			typename std::enable_if<!Formula::ExpTraits<T>::ltl, bool>::type operator () (const T&) {
				throw std::runtime_error (std::string ("Illegal node in AST: ") + Formula::ExpTraits<T>::name);
			}

			bool containsConsistent (const Formula::Expression& exp) {
				if (const Formula::E_Negation* neg = boost::get<Formula::E_Negation> (&exp)) {
					return m_atom.find (&neg->exp) == m_atom.end ();
				} else {
					return m_atom.find (&exp) != m_atom.end ();
				}
			}
	};
}


#endif /* SRC_LTL_LTL_HH_ */
