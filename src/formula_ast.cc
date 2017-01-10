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

#include "formula_ast.hh"

namespace Formula {
	int Formula_Xalloc = std::ios_base::xalloc ();

	std::ostream& operator<< (std::ostream& os, const E_Literal& l) {
		long f = os.iword (Formula_Xalloc);
		if (f == 0) {
			os << "E_Literal {" << std::boolalpha << l.value << "}";
		} else if (f == 1) {
			if (l.value)
				os << "⊤";
			else
				os << "⊥";
		} else {
			if (l.value)
				os << "\\top";
			else
				os << "\\bot";
		}
		return os;
	}

	std::ostream& operator<< (std::ostream& os, const E_Label& l) {
		long f = os.iword (Formula_Xalloc);
		if (f == 0) {
			os << "E_Label {\"" << l.name << "\"}";
		} else if (f == 1) {
			os << l.name;
		} else {
			os << "{ " << l.name << " }";
		}
		return os;
	}

	bool operator == (const E_Label& a, const E_Label& b)				{ return a.name == b.name; }
	bool operator == (const E_Literal& a, const E_Literal& b)			{ return a.value == b.value; }

	std::size_t hash_value (const E_Label& a)				{ return boost::hash<decltype(a.name)> {} (a.name); }
	std::size_t hash_value (const E_Literal& a)				{ return boost::hash<decltype(a.value)> {} (a.value); }


	bool operator == (const ExpRef& a, const ExpRef& b) { return *a == *b; }
	std::size_t hash_value (const ExpRef& e) { return boost::hash<Expression> () (*e); }

	std::ios_base& cxxprint (std::ios_base& os) {
		os.iword (Formula_Xalloc) = 0;
		return os;
	}

	std::ios_base& mathprint (std::ios_base& os) {
		os.iword (Formula_Xalloc) = 1;
		return os;
	}

	std::ios_base& latexprint (std::ios_base& os) {
		os.iword (Formula_Xalloc) = 2;
		return os;
	}
}

MCHECK_IMPL_UNARY(E_Negation, "¬" << a.exp, "\\neg " << a.exp)
MCHECK_IMPL_UNARY(E_ExistNext, "(∃X" << a.exp << ')', "\\left(\\exists \\mathrm{X} " << a.exp << "\\right) ")
MCHECK_IMPL_UNARY(E_ExistAlways, "(∃⬜" << a.exp << ')', "\\left(\\exists \\Box " << a.exp << "\\right) ")
MCHECK_IMPL_UNARY(E_AllNext, "(∀X" << a.exp << ')', "\\left(\\forall \\mathrm{X} " << a.exp << "\\right) ")
MCHECK_IMPL_UNARY(E_AllAlways, "(∀⬜" << a.exp << ')', "\\left(\\forall \\Box " << a.exp << "\\right) ")
MCHECK_IMPL_UNARY(E_Next, "(X" << a.exp << ')', "\\left(\\mathrm{X} " << a.exp << "\\right) ")


MCHECK_IMPL_BINARY(E_And, '(' << a.lhs << "∧" << a.rhs << ')', "\\left(" << a.lhs << "\\land " << a.rhs << "\\right)")
MCHECK_IMPL_BINARY(E_Or, '(' << a.lhs << "∨" << a.rhs << ')', "\\left(" << a.lhs << "\\lor " << a.rhs << "\\right)")
MCHECK_IMPL_BINARY(E_Implication, '(' << a.lhs << "→" << a.rhs << ')', "\\left(" << a.lhs << "\\to " << a.rhs << "\\right)")
MCHECK_IMPL_BINARY(E_ExistUntil, "∃(" << a.lhs << " U " << a.rhs << ')', "\\exists \\left(" << a.lhs << "\\mathrm{U} " << a.rhs << "\\right)")
MCHECK_IMPL_BINARY(E_AllUntil, "∀(" << a.lhs << " U " << a.rhs << ')', "\\forall \\left(" << a.lhs << "\\mathrm{U} " << a.rhs << "\\right)")
MCHECK_IMPL_BINARY(E_Until, '(' << a.lhs << " U " << a.rhs << ')', "\\left(" << a.lhs << "\\mathrm{U} " << a.rhs << "\\right)")
