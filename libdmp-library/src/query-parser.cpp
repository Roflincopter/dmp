#include "query-parser.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/variant.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <sstream>
#include <iostream>

using namespace boost;
using namespace std;
using namespace boost::spirit;

namespace dmp_library {

	using std::shared_ptr;
}

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast::Atom,
	(string, field)
	(string, modifier)
	(string, input)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast::Not,
	(dmp_library::ast::AstQueryType, negated)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast::Or,
	(dmp_library::ast::AstQueryType, lh)
	(dmp_library::ast::AstQueryType, rh)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast::And,
	(dmp_library::ast::AstQueryType, lh)
	(dmp_library::ast::AstQueryType, rh)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast::Nest,
	(dmp_library::ast::AstQueryType, arg)
)

namespace dmp_library {

	std::string ParseError::what() const
	{
		std::stringstream ss;
		ss << "expected: " << expected << " at pos: " << pivot;
		return ss.str();
	}

	struct ReportError {
		ParseError& error;

		ReportError(ParseError& error)
		: error(error)
		{}

		template<typename, typename, typename, typename>
		struct result { typedef void type; };

		template<typename Iter>
		void operator()(Iter first_iter, Iter __attribute__((unused)) last_iter, Iter error_iter, const qi::info& what) const {
			std::stringstream ss;
			ss << what;
			error.expected = ss.str();
			error.pivot = error_iter - first_iter;
		}
	};

	template<typename Iterator>
	struct atom_parser : spirit::qi::grammar<Iterator, ast::AstQueryType(), spirit::qi::ascii::space_type>
	{
		ParseError error;
		const phoenix::function<ReportError> error_reporter;

		qi::rule<Iterator, ast::AstQueryType(), ascii::space_type> start;
		qi::rule<Iterator, ast::AstQueryType(), ascii::space_type> query;

		qi::rule<Iterator, ast::AstQueryType(), ascii::space_type> _or;
		qi::rule<Iterator, ast::Or(), ascii::space_type> _orprime;
		qi::rule<Iterator, void(), ascii::space_type> _orliteral;

		qi::rule<Iterator, ast::AstQueryType(), ascii::space_type> _and;
		qi::rule<Iterator, ast::And(), ascii::space_type> _andprime;
		qi::rule<Iterator, void(), ascii::space_type> _andliteral;

		qi::rule<Iterator, ast::AstQueryType(), ascii::space_type> _not;
		qi::rule<Iterator, ast::Not(), ascii::space_type> _notprime;
		qi::rule<Iterator, void(), ascii::space_type> _notliteral;

		qi::rule<Iterator, ast::Nest(), ascii::space_type> nested;
		qi::rule<Iterator, void(), ascii::space_type> parenthesis_open;
		qi::rule<Iterator, void(), ascii::space_type> parenthesis_close;

		qi::rule<Iterator, ast::Atom(), ascii::space_type> atom;

		qi::rule<Iterator, string(), ascii::space_type> field;
		qi::rule<Iterator, string(), ascii::space_type> modifier;
		qi::rule<Iterator, string(), ascii::space_type> input;
		qi::rule<Iterator, void(), ascii::space_type> quote;
		qi::rule<Iterator, void(), ascii::space_type> end_of_query;

		atom_parser()
		: atom_parser::base_type(start)
		, error()
		, error_reporter(error)
		{
			using ascii::char_;

			using phoenix::at_c;
			using phoenix::push_back;
			using phoenix::val;
			using phoenix::construct;

			using qi::on_error;
			using qi::fail;
			using qi::lexeme;
			using qi::eps;

			start.name("start");
			query.name("query");
			_or.name("or");
			_orprime.name("or");
			_orliteral.name("or literal");

			_and.name("and");
			_andprime.name("and");
			_andliteral.name("and literal");

			_not.name("not");
			_notprime.name("not");
			_notliteral.name("not literal");

			nested.name("nested query");
			end_of_query.name("end of query");
			parenthesis_open.name("open parenthesis");
			parenthesis_close.name("close parenthesis");
			atom.name("query atom");
			field.name("field");
			modifier.name("modifier");
			input.name("qouted string");
			quote.name("qoute");


			start %= eps > query > end_of_query;

			query %= _or;

			_or  %= _orprime | _and;

			_orprime %= _and >> _orliteral > _or;

			_orliteral %= qi::lit("or");

			_and %= _andprime | _not;

			_andprime %= _not >> _andliteral > _and;

			_andliteral %= qi::lit("and");

			_not %= _notprime | nested | atom;

			_notprime %= _notliteral > _not;

			_notliteral %= qi::lit("not");

			nested %= (parenthesis_open > query > parenthesis_close > end_of_query);

			end_of_query %= qi::eoi;

			parenthesis_open %= '(';

			parenthesis_close %= ')';

			atom %= (field > modifier > input);

			field %=
				(
						qi::string("artist")
					|   qi::string("album")
					|   qi::string("title")
				);

			modifier %=
				(
						qi::string("is")
					|   qi::string("contains")
				);

			input %= qi::eps > quote >> lexeme[+(char_ - '"')] >> quote > qi::eps;

			quote %= '"';

			on_error<fail>
			(
				start,
				error_reporter(_1, _2, _3, _4)
			);
		}
	};

	shared_ptr<Query> parse_query(string const& str)
	{
		atom_parser<string::const_iterator> ap;
		ast::AstQueryType ast;
		bool r = qi::phrase_parse(str.cbegin(), str.cend(), ap, ascii::space, ast);

		if(!r) {
			throw ap.error;
		}

		ast::precedence_visitor precedence;
		ast = boost::apply_visitor(precedence, ast);
/*
		PrintVisitor p(cout);
		apply_visitor(p, ast);
		cout << endl;
*/
		ast::to_query_visitor q;
		return apply_visitor(q, ast);
	}
}
