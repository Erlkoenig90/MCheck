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

#ifndef TS_AST_HH_
#define TS_AST_HH_

#include <string>
#include <vector>
#include <iostream>

#include <boost/variant.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/equal_to.hpp>
#include <boost/optional.hpp>

BOOST_FUSION_DEFINE_STRUCT(
	(TS), E_AttrShape,
	(std::string, shape)
)

BOOST_FUSION_DEFINE_STRUCT(
	(TS), E_AttrLabel,
	(boost::optional<std::string>, name)
	(std::vector<std::string>, atomicPropositions)
)

BOOST_FUSION_DEFINE_STRUCT(
	(TS), E_AttrGeneric,
	(std::string, name)
	(std::string, value)
)

namespace TS {
	using E_Attribute = boost::variant	<E_AttrShape, E_AttrLabel, E_AttrGeneric>;
}

BOOST_FUSION_DEFINE_STRUCT(
	(TS), E_AttrDecl,
	(std::string, node)
	(std::vector<::TS::E_Attribute>, attributes)
)

BOOST_FUSION_DEFINE_STRUCT(
	(TS), E_Transition,
	(std::string, from)
	(std::string, to)
)

namespace TS {
	using E_Statement = boost::variant	<E_Transition, E_AttrDecl>;
}

BOOST_FUSION_DEFINE_STRUCT(
	(TS), E_Graph,
	(std::string, name)
	(std::vector<::TS::E_Statement>, statements)
)
namespace TS {
	inline bool operator == (const E_Transition& a, const E_Transition& b) { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_AttrDecl& a, const E_AttrDecl& b)  { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_AttrGeneric& a, const E_AttrGeneric& b)  { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_AttrShape& a, const E_AttrShape& b)  { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_AttrLabel& a, const E_AttrLabel& b)  { return boost::fusion::equal_to (a, b); }
	inline bool operator == (const E_Graph& a, const E_Graph& b)  { return boost::fusion::equal_to (a, b); }

	std::ostream& operator<< (std::ostream& os, const E_Transition& l);
	std::ostream& operator<< (std::ostream& os, const E_AttrGeneric& l);
	std::ostream& operator<< (std::ostream& os, const E_AttrShape& l);
	std::ostream& operator<< (std::ostream& os, const E_AttrLabel& l);
	std::ostream& operator<< (std::ostream& os, const E_AttrDecl& l);
	std::ostream& operator<< (std::ostream& os, const E_Graph& a);
}


#endif /* TS_AST_HH_ */
