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

#ifndef SRC_CTL_CTL_HH_
#define SRC_CTL_CTL_HH_

#include <cassert>
#include <utility>
#include <set>
#include <memory>
#include <algorithm>
#include <iostream>

#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/equal_to.hpp>

#include "../ts/sys.hh"
#include "ast.hh"

namespace CTL {
	using SatSet = std::set<TS::State*>;

	std::ostream& operator << (std::ostream& os, const SatSet& sat);

	class S_Formula {
		public:
			S_Formula (const Expression& e, SatSet&& s, std::string::const_iterator sb, std::string::const_iterator se) : exp (e), sat (std::move (s)), strBegin (sb), strEnd (se) {}

			virtual void print (std::ostream& os, size_t depth);

			const Expression& exp;
			SatSet sat;
			std::string::const_iterator strBegin, strEnd;
	};
	class S_Nullary : public S_Formula {
		public:
			S_Nullary (const Expression& e, SatSet&& s, std::string::const_iterator sb, std::string::const_iterator se) : S_Formula (e, std::move (s), sb, se) {}
			virtual void print (std::ostream& os, size_t depth);
	};
	class S_Unary : public S_Formula {
		public:
			S_Unary (const Expression& e, SatSet&& s, std::unique_ptr<S_Formula>&& c, std::string::const_iterator sb, std::string::const_iterator se)
				: S_Formula (e, std::move (s), sb, se), child (std::move (c)) {}
			virtual void print (std::ostream& os, size_t depth);
			std::unique_ptr<S_Formula> child;
	};
	class S_Binary : public S_Formula {
		public:
			S_Binary (const Expression& e, SatSet&& s, std::unique_ptr<S_Formula>&& l, std::unique_ptr<S_Formula>&& r, std::string::const_iterator sb, std::string::const_iterator se)
				: S_Formula (e, std::move (s), sb, se), left (std::move (l)), right (std::move (r)) {}
			virtual void print (std::ostream& os, size_t depth);
			std::unique_ptr<S_Formula> left, right;
	};

	struct SatVisitor : public boost::static_visitor<std::unique_ptr<S_Formula>> {
		SatVisitor (TS::TranSys& ts) : tranSys (ts) {}
		TS::TranSys& tranSys;

		template <typename E, typename F>
		std::unique_ptr<S_Formula> nullary (const E& exp, F f) const {
			return std::make_unique<S_Nullary> (exp, f (), exp.strBegin, exp.strEnd);
		}
		template <typename E, typename F>
		std::unique_ptr<S_Formula> unary (const E& exp, F f) const {
			auto child = boost::apply_visitor (*this, exp.exp);
			SatSet sat = f (*child);

			return std::make_unique<S_Unary> (exp, std::move (sat), std::move (child), exp.strBegin, exp.strEnd);
		}
		template <typename E, typename F>
		std::unique_ptr<S_Formula> binary (const E& exp, F f) const {
			auto left = boost::apply_visitor (*this, exp.lhs);
			auto right = boost::apply_visitor (*this, exp.rhs);
			SatSet sat = f (*left, *right);

			return std::make_unique<S_Binary> (exp, std::move (sat), std::move (left), std::move (right), exp.strBegin, exp.strEnd);
		}

		std::unique_ptr<S_Formula> operator() (const E_Literal& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_Negation& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_And& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_Or& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_Implication& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_ExistNext& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_ExistUntil& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_ExistAlways& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_AllNext& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_AllUntil& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_AllAlways& exp) const;
		std::unique_ptr<S_Formula> operator() (const E_Label& exp) const;
	};

	template <typename T>
	bool operator <= (const std::set<T>& sub, const std::set<T>& super) {
		return std::includes (super.begin (), super.end (), sub.begin (), sub.end ());
	}
	template <typename T>
	std::set<T> operator - (const std::set<T>& super, const std::set<T>& sub) {
		assert (sub <= super);

		std::set<T> res;
		for (const auto& el : super) {
			if (sub.find (el) == sub.end ())
				res.insert (el);
		}
		return res;
	}
	template <typename T>
	std::set<T> operator & (const std::set<T>& a, const std::set<T>& b) {
		std::set<T> res;

		for (auto& x : a) {
			if (b.find (x) != b.end ())
				res.insert (x);
		}

		return res;
	}
	template <typename T>
	std::set<T> operator | (const std::set<T>& a, const std::set<T>& b) {
		std::set<T> res (a);
		res.insert (b.begin (), b.end ());
		return res;
	}
	template <typename T>
	std::set<T>& operator |= (std::set<T>& a, std::set<T>& b) {
		a.insert (b.begin (), b.end ());
		return a;
	}
	template <typename T>
	bool operator && (const std::set<T>& a, const std::set<T>& b) {
		for (auto& x : a) {
			if (b.find (x) != b.end ())
				return true;
		}
		return false;
	}

	std::pair<bool, std::unique_ptr<S_Formula>> computeSat (const Expression& formula, TS::TranSys& ts);
}


#endif /* SRC_CTL_CTL_HH_ */
