#include "query-parser.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/variant.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

using namespace boost;
using namespace std;
using namespace boost::spirit;

namespace dmp_library {
	
	using std::shared_ptr;	
}

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast_atom,
	(string, field)
	(string, modifier)
	(string, input)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast_not,
	(dmp_library::ast_query_type, negated)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast_or,
	(dmp_library::ast_query_type, lh)
	(dmp_library::ast_query_type, rh)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast_and,
	(dmp_library::ast_query_type, lh)
	(dmp_library::ast_query_type, rh)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast_nest,
	(dmp_library::ast_query_type, arg)
)
	

namespace dmp_library {
	
	template<typename Iterator>
	struct atom_parser : spirit::qi::grammar<Iterator, ast_query_type(), spirit::qi::ascii::space_type>
	{	
		qi::rule<Iterator, ast_query_type(), ascii::space_type> query;
		qi::rule<Iterator, ast_query_type(), ascii::space_type> _not;
		qi::rule<Iterator, ast_not(), ascii::space_type> _notprime;
		qi::rule<Iterator, ast_query_type(), ascii::space_type> _or;
		qi::rule<Iterator, ast_or(), ascii::space_type> _orprime;
		qi::rule<Iterator, ast_query_type(), ascii::space_type> _and;
		qi::rule<Iterator, ast_and(), ascii::space_type> _andprime;
		qi::rule<Iterator, ast_atom(), ascii::space_type> atom; 
		qi::rule<Iterator, ast_nest(), ascii::space_type> nested;
		qi::rule<Iterator, string(), ascii::space_type> field;
		qi::rule<Iterator, string(), ascii::space_type> modifier;
		qi::rule<Iterator, string(), ascii::space_type> input;
		
		atom_parser() : atom_parser::base_type(query)
		{	
			using qi::lexeme;
			using ascii::char_;
			using phoenix::at_c;
			using phoenix::push_back;
			
			query %= _or | qi::eoi;
			
			_or  %= _orprime | _and;
			
			_orprime %= _and >> qi::lit("or") >> _or;
			
			_and %= _andprime | _not;
			
			_andprime %= _not >> qi::lit("and") >> _and;
			
			_not %= _notprime | nested | atom;
			
			_notprime %= qi::lit("not") >> _not;
			
			nested %= (qi::lit("(") >> query >> qi::lit(")") >> -qi::eoi);
			
			atom %= (field >> modifier >> input);
			
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
			
			input %= 
				    '"'
				>>  lexeme[+(char_ - '"')] 
				>>  '"';
		}
	};
	
	void parse_query(string const& str)
	{
		atom_parser<string::const_iterator> ap;
		ast_query_type ast;
		bool r = qi::phrase_parse(str.cbegin(), str.cend(), ap, ascii::space, ast);
		cout << (r ? "parse_suceeded" : "parse_failed" ) << endl;
		
		print_visitor p(cout);
		apply_visitor(p, ast);
		cout << endl;
		
		to_query_visitor q;
		apply_visitor(q, ast);
		
	}
}
