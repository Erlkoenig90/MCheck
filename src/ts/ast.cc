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

#include <boost/optional/optional_io.hpp>
#include "ast.hh"

namespace TS {
std::ostream& operator<< (std::ostream& os, const E_Transition& l) {
	os << "E_Transition {\"" << l.from << "\", \"" << l.to << "\"}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_AttrGeneric& l) {
	os << "E_AttrGeneric {\"" << l.name << "\", \"" << l.value << "\"}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_AttrShape& l) {
	os << "E_AttrShape {\"" << l.shape << "\"}";
	return os;
}

std::ostream& operator<< (std::ostream& os, const E_AttrLabel& l) {
	os << "E_AttrLabel {";

	if (l.name)
		os << "\"" << *(l.name) << "\", {";
	else
		os << "boost::none, {";

	for (const auto& ap : l.atomicPropositions) {
		os << "\"" << ap << "\",";
	}
	os << "}}";
	return os;
}

std::ostream& operator<< (std::ostream& os, const E_AttrDecl& l) {
	os << "E_AttrDecl {\"" << l.node << "\", {";

	for (const auto& a : l.attributes) {
		os << a << ", ";
	}

	os << "}}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_Graph& a) {
	os << "E_Graph {\"" << a.name << "\", {";

	for (const auto& s : a.statements) {
		os << s << ", ";
	}

	os << "}}";
    return os;
}

}
