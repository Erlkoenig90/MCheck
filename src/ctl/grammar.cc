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

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/repository/include/qi_iter_pos.hpp>

#if BOOST_VERSION < 106200
#	error "Boost too old"
#endif

#include "grammar.hh"

namespace CTL {
	BOOST_SPIRIT_TERMINAL(identifier);
	BOOST_SPIRIT_TERMINAL(uspace);
}

namespace boost {
	namespace spirit {
		template <> struct use_terminal<qi::domain, CTL::tag::identifier> : mpl::true_ {};
		template <> struct use_terminal<qi::domain, CTL::tag::uspace> : mpl::true_ {};
	}
}

namespace CTL {
	struct identifier_parser : boost::spirit::qi::primitive_parser<identifier_parser> {
			template <typename Context, typename Iterator>
			struct attribute {
				using type = std::basic_string<typename Iterator::value_type>;
			};

			template <typename Iterator, typename Context, typename Skipper, typename Attribute>
			bool parse (Iterator& first, Iterator const& last, Context&, Skipper const& skipper, Attribute& attr) const {
				boost::spirit::qi::skip_over (first, last, skipper);

				Iterator scan = first;

				while (scan != last && ((*scan >= 0x41 && *scan <= 0x5A) || (*scan >= 0x61 && *scan <= 0x7A) || (*scan >= 0x30 && *scan <= 39) || *scan == 0x5F))
					++scan;

				if (scan == first) return false;
				boost::spirit::traits::assign_to (std::basic_string<typename Iterator::value_type> (first, scan), attr);

				first = scan;
				return true;
			}

			template <typename Context>
			boost::spirit::info what (Context&) const {
				return boost::spirit::info ("identifier");
			}
	};
	struct uspace_parser : boost::spirit::qi::primitive_parser<uspace_parser> {
			template <typename Context, typename Iterator>
			struct attribute {
				using type = boost::spirit::unused_type;
			};

			template <typename Iterator, typename Context, typename Skipper, typename Attribute>
			bool parse (Iterator& first, Iterator const& last, Context&, Skipper const&, Attribute&) const {
				Iterator scan = first;

				while (scan != last && ((*scan == 0x9) || (*scan == 0xA) || (*scan == 0xD) || (*scan == 0x20)))
					++scan;

				if (scan == first) return false;

				first = scan;
				return true;
			}

			template <typename Context>
			boost::spirit::info what (Context&) const {
				return boost::spirit::info ("Space Skip");
			}
	};
}

namespace boost {
	namespace spirit {
		namespace qi {
			template <typename Modifiers>
			struct make_primitive<CTL::tag::identifier, Modifiers> {
					using result_type = CTL::identifier_parser;

					result_type operator() (unused_type, unused_type) const {
						return result_type ();
					}
			};
			template <typename Modifiers>
			struct make_primitive<CTL::tag::uspace, Modifiers> {
					using result_type = CTL::uspace_parser;

					result_type operator() (unused_type, unused_type) const {
						return result_type ();
					}
			};
		}
	}
}

namespace CTL {

using boost::spirit::qi::rule;

template <typename Iterator>
struct Grammar : boost::spirit::qi::grammar<Iterator, Expression(), uspace_type> {
	using Skip = uspace_type;

	Grammar () :	Grammar::base_type(formula)
	{
		using boost::spirit::qi::lit;
		using namespace boost::spirit::qi::labels;
		using boost::spirit::qi::lexeme;
		using boost::spirit::qi::bool_;
		using boost::spirit::qi::eoi;
		using boost::phoenix::at_c;
		using boost::spirit::qi::as_string;
		using boost::spirit::repository::qi::iter_pos;

		formula %= expression >> eoi;

		expression %=
				andR | orR | implication | existNext | existAlways | allNext | allAlways | primaryExpression;

		primaryExpression %= literal | negation | existUntil | allUntil | label | (lit('(') >> expression >> lit(')'));

		literal %= lexeme[iter_pos >> bool_ >> !identifier >> iter_pos];
		label %= lexeme [iter_pos >> identifier >> iter_pos];

		negation %= iter_pos >> lit("¬") >> primaryExpression >> iter_pos;
		andR %= iter_pos >> primaryExpression >> lit("∧") >> primaryExpression >> iter_pos;
		orR %= iter_pos >> primaryExpression >> lit("∨") >> primaryExpression >> iter_pos;
		implication %= iter_pos >> primaryExpression >> lit("→") >> primaryExpression >> iter_pos;

		existNext %= iter_pos >> lit("∃") >> lit('X') >> primaryExpression >> iter_pos;
		existUntil %= iter_pos >> lit("∃") >> lit('(') >> primaryExpression >> lit('U') >> primaryExpression >> lit(')') >> iter_pos;
		existAlways %= iter_pos >> lit("∃") >> lit("⬜") >> primaryExpression >> iter_pos;

		allNext %= iter_pos >> lit("∀") >> lit('X') >> primaryExpression >> iter_pos;
		allUntil %= iter_pos >> lit("∀") >> lit('(') >> primaryExpression >> lit('U') >> primaryExpression >> lit(')') >> iter_pos;
		allAlways %= iter_pos >> lit("∀") >> lit("⬜") >> primaryExpression >> iter_pos;
	}

	rule <Iterator, Expression(), Skip> formula;
	rule <Iterator, Expression(), Skip> expression;
	rule <Iterator, Expression(), Skip> primaryExpression;
	rule <Iterator, E_Literal(), Skip> literal;
	rule <Iterator, E_Label(), Skip> label;


	rule <Iterator, E_Negation(), Skip> negation;
	rule <Iterator, E_And(), Skip> andR;
	rule <Iterator, E_Or(), Skip> orR;
	rule <Iterator, E_Implication(), Skip> implication;

	rule <Iterator, E_ExistNext(), Skip> existNext;
	rule <Iterator, E_ExistUntil(), Skip> existUntil;
	rule <Iterator, E_ExistAlways(), Skip> existAlways;

	rule <Iterator, E_AllNext(), Skip> allNext;
	rule <Iterator, E_AllUntil(), Skip> allUntil;
	rule <Iterator, E_AllAlways(), Skip> allAlways;
};

bool parse (const std::string& str, Expression& res) {
	Grammar<std::string::const_iterator> g;

	std::string::const_iterator begin = str.cbegin ();
	std::string::const_iterator end = str.cend ();

	return boost::spirit::qi::phrase_parse (begin, end, g, uspace, res) && begin == end;
}

}
