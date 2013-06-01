
#include "query-parser.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted.hpp>

using namespace boost;
using namespace std;
using namespace boost::spirit;

/*
BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::atom,
	(int, field)
	(int, modifier)
	(std::string, query_string)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::_and,
	(std::shared_ptr<dmp_library::query>, lh)
	(std::shared_ptr<dmp_library::query>, rh)
)

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::_or,
	(std::shared_ptr<dmp_library::query>, lh)
	(std::shared_ptr<dmp_library::query>, rh)
)
	
BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::_not,
	(std::shared_ptr<dmp_library::query>, arg)
)
*/


namespace dmp_library {
	struct ast_atom;
}

BOOST_FUSION_ADAPT_STRUCT(
	dmp_library::ast_atom,
	(bool, negate)
	(string, field)
	(string, modifier)
	(string, input)
)

namespace dmp_library {
	
	struct ast_atom {
		bool negate;
		string field;
		string modifier;
		string input;
	};
	

		
	
	template<typename Iterator>
	struct atom_parser : spirit::qi::grammar<Iterator, ast_atom, spirit::qi::ascii::space_type>
	{	
		//qi::rule<Iterator, std::shared_ptr<query>(), spirit::ascii::space_type> query;
		//qi::rule<Iterator, std::shared_ptr<query>(), spirit::ascii::space_type> negate;
		qi::rule<Iterator, ast_atom(), spirit::ascii::space_type> atom; 
		qi::rule<Iterator, query::field(), spirit::ascii::space_type> field;
		qi::rule<Iterator, query::modifier(), spirit::ascii::space_type> modifier;
		qi::rule<Iterator, std::string(), spirit::ascii::space_type> input;
		
		atom_parser() : atom_parser::base_type(atom)
		{
			using qi::lexeme;
			using ascii::char_;
			
			//query %= *(negate) >> atom;
			
			//negate %= "not";
			
			atom %= field >> modifier >> input;
			
			field %= qi::lit("artist") | qi::lit("album") | qi::lit("title");
			
			modifier %= qi::lit("is") | qi::lit("contains");
			
			input %= '"' >> lexeme[+(char_ - '"')] >> '"';
		}
	};
	
	std::shared_ptr<query> parse_query(string const& str)
	{
		
	}
	
}

