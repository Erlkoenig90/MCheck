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

#include "ast.hh"

namespace CTL {
std::ostream& operator<< (std::ostream& os, const E_Literal& l) {
	os << "E_Literal {" << std::boolalpha << l.value << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_Label& l) {
	os << "E_Label {\"" << l.name << "\"}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_Negation& l) {
	os << "E_Negation {" << l.exp << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_And& a) {
	os << "E_And {" << a.lhs << ", " << a.rhs << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_Or& a) {
	os << "E_Or {" << a.lhs << ", " << a.rhs << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_Implication& a) {
	os << "E_Implication {" << a.lhs << ", " << a.rhs << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_ExistNext& a) {
	os << "E_ExistNext {" << a.exp << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_ExistUntil& a) {
	os << "E_ExistUntil {" << a.lhs << ", " << a.rhs << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_ExistAlways& a) {
	os << "E_ExistAlways {" << a.exp << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_AllNext& a) {
	os << "E_AllNext {" << a.exp << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_AllUntil& a) {
	os << "E_AllUntil {" << a.lhs << ", " << a.rhs << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const E_AllAlways& a) {
	os << "E_AllAlways {" << a.exp << "}";
    return os;
}

}
