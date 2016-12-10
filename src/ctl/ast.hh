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

#ifndef CTL_AST_HH_
#define CTL_AST_HH_

#include <string>
#include <iostream>

#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/equal_to.hpp>

namespace CTL {
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
}

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_Literal,
	(std::string::const_iterator, strBegin)
	(bool, value)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_Label,
	(std::string::const_iterator, strBegin)
	(std::string, name)
	(std::string::const_iterator, strEnd)
)
namespace CTL {
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
										boost::recursive_wrapper<E_AllAlways>
									>;
}

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_Negation,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, exp)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_And,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, lhs)
	(CTL::Expression, rhs)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_Or,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, lhs)
	(CTL::Expression, rhs)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_Implication,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, lhs)
	(CTL::Expression, rhs)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_ExistNext,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, exp)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_ExistUntil,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, lhs)
	(CTL::Expression, rhs)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_ExistAlways,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, exp)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_AllNext,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, exp)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_AllUntil,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, lhs)
	(CTL::Expression, rhs)
	(std::string::const_iterator, strEnd)
)

BOOST_FUSION_DEFINE_STRUCT(
	(CTL), E_AllAlways,
	(std::string::const_iterator, strBegin)
	(CTL::Expression, exp)
	(std::string::const_iterator, strEnd)
)

namespace CTL {
	inline bool operator == (const E_Label& a, const E_Label& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_Literal& a, const E_Literal& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_Negation& a, const E_Negation& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_And& a, const E_And& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_Or& a, const E_Or& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_Implication& a, const E_Implication& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_ExistNext& a, const E_ExistNext& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_ExistUntil& a, const E_ExistUntil& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_ExistAlways& a, const E_ExistAlways& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_AllNext& a, const E_AllNext& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_AllUntil& a, const E_AllUntil& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_AllAlways& a, const E_AllAlways& b) { return boost::fusion::equal_to (a, b); }
	std::ostream& operator<< (std::ostream& os, const E_Literal& l);
	std::ostream& operator<< (std::ostream& os, const E_Label& l);
	std::ostream& operator<< (std::ostream& os, const E_Negation& l);
	std::ostream& operator<< (std::ostream& os, const E_And& a);
	std::ostream& operator<< (std::ostream& os, const E_Or& a);
	std::ostream& operator<< (std::ostream& os, const E_Implication& a);
	std::ostream& operator<< (std::ostream& os, const E_ExistNext& a);
	std::ostream& operator<< (std::ostream& os, const E_ExistUntil& a);
	std::ostream& operator<< (std::ostream& os, const E_ExistAlways& a);
	std::ostream& operator<< (std::ostream& os, const E_AllNext& a);
	std::ostream& operator<< (std::ostream& os, const E_AllUntil& a);
	std::ostream& operator<< (std::ostream& os, const E_AllAlways& a);
}

#endif /* CTL_AST_HH_ */
