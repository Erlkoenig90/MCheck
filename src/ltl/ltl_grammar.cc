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

#include "ltl_grammar.hh"

namespace LTL {
	using boost::spirit::qi::rule;
	using namespace Formula;

	template <typename Iterator>
	struct Grammar : public BaseGrammar::Grammar<Iterator> {
		using B = BaseGrammar::Grammar<Iterator>;
		using typename B::Skip;
		using B::formula;
		using B::expression;
		using B::primaryExpression;
		using B::literal;
		using B::label;
		using B::negation;
		using B::andR;
		using B::orR;
		using B::implication;

		Grammar () {
			using boost::spirit::qi::lit;
			using boost::spirit::repository::qi::iter_pos;

			expression %=
					andR | orR | implication | next | until | primaryExpression;

			primaryExpression %= literal | negation | label | (lit('(') >> expression >> lit(')'));


			next %= iter_pos >> lit('X') >> primaryExpression >> iter_pos;
			until %= iter_pos >> primaryExpression >> lit('U') >> primaryExpression >> iter_pos;
		}
		rule <Iterator, E_Next(), Skip> next;
		rule <Iterator, E_Until(), Skip> until;
	};

	bool parse (const std::string& str, Formula::Expression& res) {
		Grammar<std::string::const_iterator> g;

		std::string::const_iterator begin = str.cbegin ();
		std::string::const_iterator end = str.cend ();

		return boost::spirit::qi::phrase_parse (begin, end, g, BaseGrammar::uspace, res) && begin == end;
	}

}
