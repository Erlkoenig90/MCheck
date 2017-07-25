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

#ifndef FORMULA_AST_HH_
#define FORMULA_AST_HH_

#include <string>
#include <iostream>
#include <cstddef>
#include <utility>
#include <ios>

#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/functional/hash.hpp>
#include <boost/preprocessor/stringize.hpp>


namespace Formula {
	struct E_Literal;
	struct E_Negation;
	struct E_And;
	struct E_Or;
	struct E_Implication;
	struct E_ExistNext;
	struct E_ExistUntil;
	struct E_ExistAlways;
	struct E_AllNext;
	struct E_AllUntil;
	struct E_AllAlways;
	struct E_Next;
	struct E_Until;
}

BOOST_FUSION_DEFINE_STRUCT(
	(Formula), E_Literal,
	(std::string::const_iterator, strBegin)
	(bool, value)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(Formula), E_Label,
	(std::string::const_iterator, strBegin)
	(std::string, name)
	(std::string::const_iterator, strEnd)
)
namespace Formula {
	using Expression = boost::variant<	E_Literal, E_Label,
										boost::recursive_wrapper<E_Negation>,
										boost::recursive_wrapper<E_And>,
										boost::recursive_wrapper<E_Or>,
										boost::recursive_wrapper<E_Implication>,
										boost::recursive_wrapper<E_ExistNext>,
										boost::recursive_wrapper<E_ExistUntil>,
										boost::recursive_wrapper<E_ExistAlways>,
										boost::recursive_wrapper<E_AllNext>,
										boost::recursive_wrapper<E_AllUntil>,
										boost::recursive_wrapper<E_AllAlways>,
										boost::recursive_wrapper<E_Next>,
										boost::recursive_wrapper<E_Until>
									>;
	template <typename T>
	struct ExpTraits;

	extern int Formula_Xalloc;
}

#define MCHECK_DEFINE_UNARY(NAME,C,L)	BOOST_FUSION_DEFINE_STRUCT((Formula), NAME, (std::string::const_iterator, strBegin)(Formula::Expression, exp)(std::string::const_iterator, strEnd)) \
										namespace Formula { \
											std::size_t hash_value (const NAME& a); \
											bool operator == (const NAME& a, const NAME& b); \
											std::ostream& operator<< (std::ostream& os, const NAME& l); \
											template <> struct ExpTraits<NAME> { static constexpr bool unary = true; static constexpr bool ltl = L; static constexpr bool ctl = C; static const char* name; }; \
										}

#define MCHECK_IMPL_UNARY(NAME,PM,PL)	std::size_t Formula::hash_value (const Formula::NAME& a) { return boost::hash<Formula::Expression> {} (a.exp); } \
										bool Formula::operator == (const Formula::NAME& a, const Formula::NAME& b) { return a.exp == b.exp; } \
										const char* Formula::ExpTraits<Formula::NAME>::name = BOOST_PP_STRINGIZE(NAME); \
										std::ostream& Formula::operator<< (std::ostream& os, const Formula::NAME& a) { long f = os.iword (Formula_Xalloc); if (f == 0) os << ExpTraits<NAME>::name << " {" << a.exp << '}'; else if (f == 1) os << PM ; else os << PL ; return os; }



#define MCHECK_DEFINE_BINARY(NAME,C,L)	BOOST_FUSION_DEFINE_STRUCT((Formula), NAME, (std::string::const_iterator, strBegin)(Formula::Expression, lhs)(Formula::Expression, rhs)(std::string::const_iterator, strEnd)) \
										namespace Formula { \
											std::size_t hash_value (const NAME& a); \
											bool operator == (const NAME& a, const NAME& b); \
											std::ostream& operator<< (std::ostream& os, const NAME& l); \
											template <> struct ExpTraits<NAME> { static constexpr bool binary = true; static constexpr bool ltl = L; static constexpr bool ctl = C; static const char* name; }; \
										}

#define MCHECK_IMPL_BINARY(NAME,PM,PL)	std::size_t Formula::hash_value (const Formula::NAME& a) { std::size_t s = 0; boost::hash_combine (s, a.lhs); boost::hash_combine (s, a.rhs); return s; } \
										bool Formula::operator == (const Formula::NAME& a, const Formula::NAME& b) { return a.lhs == b.lhs && a.rhs == b.rhs; } \
										const char* Formula::ExpTraits<Formula::NAME>::name = BOOST_PP_STRINGIZE(NAME); \
										std::ostream& Formula::operator<< (std::ostream& os, const Formula::NAME& a) { long f = os.iword (Formula_Xalloc); if (f == 0) os << ExpTraits<NAME>::name << " {" << a.lhs << ", " << a.rhs << '}'; else if (f == 1) os << PM ; else os << PL ; return os; }


MCHECK_DEFINE_UNARY(E_Negation, true, true)
MCHECK_DEFINE_UNARY(E_ExistNext, true, false)
MCHECK_DEFINE_UNARY(E_ExistAlways, true, false)
MCHECK_DEFINE_UNARY(E_AllNext, true, false)
MCHECK_DEFINE_UNARY(E_AllAlways, true, false)
MCHECK_DEFINE_UNARY(E_Next, false, true)


MCHECK_DEFINE_BINARY(E_And, true, true)
MCHECK_DEFINE_BINARY(E_Or, true, true)
MCHECK_DEFINE_BINARY(E_Implication, true, true)
MCHECK_DEFINE_BINARY(E_ExistUntil, true, false)
MCHECK_DEFINE_BINARY(E_AllUntil, true, false)
MCHECK_DEFINE_BINARY(E_Until, false, true)

namespace Formula {
	bool operator == (const E_Label& a, const E_Label& b);
	bool operator == (const E_Literal& a, const E_Literal& b);
	std::ostream& operator<< (std::ostream& os, const E_Literal& l);
	std::ostream& operator<< (std::ostream& os, const E_Label& l);
    std::size_t hash_value (const E_Label& a);
    std::size_t hash_value (const E_Literal& a);

	std::ios_base& mathprint (std::ios_base& os);
	std::ios_base& cxxprint (std::ios_base& os);
	std::ios_base& latexprint (std::ios_base& os);

    class ExpRef {
    	public:
    		using Target = const Expression;

    		inline ExpRef () : exp {} {}
    		inline ExpRef (Target* e) : exp (e) {}
    		inline ExpRef (const ExpRef&) = default;
    		inline ExpRef (ExpRef&&) = default;
    		inline ExpRef& operator = (const ExpRef&) = default;
    		inline ExpRef& operator = (ExpRef&&) = default;

			inline const Target* operator -> () const { return exp; }
			inline const Target& operator * () const { return *exp; }

    	private:
			const Target* exp;
    };
    bool operator == (const ExpRef& a, const ExpRef& b);
    std::size_t hash_value (const ExpRef& e);
}

#endif /* FORMULA_AST_HH_ */
