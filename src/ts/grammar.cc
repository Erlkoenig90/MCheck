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

#if BOOST_VERSION < 106200
#	error "Boost too old"
#endif

#include "grammar.hh"

namespace TS {

using boost::spirit::qi::symbols;
using boost::spirit::qi::rule;

template <typename Iterator, typename Skip>
struct Grammar : boost::spirit::qi::grammar<Iterator, E_Graph(), Skip> {

	Grammar () :	Grammar::base_type(graph)
	{
		using boost::spirit::qi::lit;
		using namespace boost::spirit::qi::labels;
		using boost::spirit::ascii::char_;
		using boost::spirit::ascii::alnum;
		using boost::spirit::ascii::string;
		using boost::spirit::ascii::space;
		using boost::spirit::qi::lexeme;
		using boost::spirit::qi::bool_;
		using boost::spirit::qi::eoi;
		using boost::phoenix::at_c;
		using boost::spirit::qi::as_string;
		using boost::spirit::qi::omit;
		using boost::spirit::qi::hold;


		escape.add	("\\\"", '\"')("\\\\", '\\')("\\n", '\n')("\\r", '\r')("\\t", '\t')("\'", '\'');
		lstring %= lexeme[+(alnum | char_('_'))];
		qstring %= (lexeme[lit('"') >> *((char_ - '"' - '\\') | escape) >> lit('"')]) | lstring;

		graph %= lit("digraph") >> qstring >> lit("{") >> *statement >> lit("}") >> eoi;

		statement %= (transition|attrDecl) >> -lit(';');
		transition %= qstring >> lit("->") >> qstring;

		attrDecl %= qstring >> lit('[') >> (attribute % lit(',')) >> lit(']');
		attribute %= attrLabel | attrShape | attrGeneric;
		attrGeneric %= qstring >> lit('=') >> qstring;
		attrShape %= (lit("\"shape\"") | lit("shape")) >> lit('=') >> qstring;
		attrLabel %= (lit("\"label\"") | lit("label")) >> lit('=') >> lit('\"') >> optNodeLabel >> (lstring % ',') >> lit('\"');

		optNodeLabel %= -hold[lexeme[+(char_ - lit(':'))] >> lit(':')];
	}

	boost::spirit::qi::rule <Iterator, E_Graph(), Skip> graph;
	rule <Iterator, std::string()> qstring;
	rule <Iterator, std::string()> lstring;
	rule <Iterator, E_Statement, Skip> statement;
	rule <Iterator, E_Transition, Skip> transition;
	rule <Iterator, E_AttrDecl, Skip> attrDecl;
	rule <Iterator, E_Attribute, Skip> attribute;
	rule <Iterator, E_AttrGeneric, Skip> attrGeneric;
	rule <Iterator, E_AttrShape, Skip> attrShape;
	rule <Iterator, E_AttrLabel, Skip> attrLabel;
	rule <Iterator, boost::optional<std::string>> optNodeLabel;
	symbols<char, char> escape;

};

bool parse (const std::string& str, E_Graph& res) {
	Grammar<std::string::const_iterator, boost::spirit::ascii::space_type> g;

	std::string::const_iterator begin = str.cbegin ();
	std::string::const_iterator end = str.cend ();

	return boost::spirit::qi::phrase_parse (begin, end, g, boost::spirit::ascii::space, res) && begin == end;
}

}
