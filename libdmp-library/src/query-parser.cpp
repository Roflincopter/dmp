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

        qi::rule<Iterator, ast_query_type(), ascii::space_type> _or;
        qi::rule<Iterator, ast_or(), ascii::space_type> _orprime;
        qi::rule<Iterator, void(), ascii::space_type> _orliteral;

        qi::rule<Iterator, ast_query_type(), ascii::space_type> _and;
        qi::rule<Iterator, ast_and(), ascii::space_type> _andprime;
        qi::rule<Iterator, void(), ascii::space_type> _andliteral;

        qi::rule<Iterator, ast_query_type(), ascii::space_type> _not;
        qi::rule<Iterator, ast_not(), ascii::space_type> _notprime;
        qi::rule<Iterator, void(), ascii::space_type> _notliteral;

        qi::rule<Iterator, ast_nest(), ascii::space_type> nested;
        qi::rule<Iterator, void(), ascii::space_type> parenthesis_open;
        qi::rule<Iterator, void(), ascii::space_type> parenthesis_close;

        qi::rule<Iterator, ast_atom(), ascii::space_type> atom;

        qi::rule<Iterator, string(), ascii::space_type> field;
        qi::rule<Iterator, string(), ascii::space_type> modifier;
        qi::rule<Iterator, string(), ascii::space_type> input;
        qi::rule<Iterator, void(), ascii::space_type> quote;
        qi::rule<Iterator, void(), ascii::space_type> end_of_query;

        atom_parser() : atom_parser::base_type(query)
        {
            using qi::lexeme;
            using ascii::char_;
            using phoenix::at_c;
            using phoenix::push_back;
            using phoenix::val;
            using phoenix::construct;

            using qi::on_error;
            using qi::fail;

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


            query %= _or | end_of_query;

            _or  %= _orprime | _and;

            _orprime %= _and >> _orliteral > _or;

            _orliteral %= "or";

            _and %= _andprime | _not;

            _andprime %= _not >> _andliteral > _and;

            _andliteral %= "and";

            _not %= _notprime | nested | atom;

            _notprime %= _notliteral > _not;

            _notliteral %= "not";

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

            input %= quote > lexeme[+(char_ - '"')] > quote;

            quote %= '"';

            on_error<fail>
            (
                query
                , std::cout
                    << val("Error! Expecting ")
                    << qi::_4                               // what failed?
                    << std::endl
                    << construct<std::string>(qi::_1, qi::_3)
                    << val("<here>")
                    << construct<std::string>(qi::_3, qi::_2)   // iterators to error-pos, end
                    << std::endl
            );
        }
    };

    shared_ptr<query> parse_query(string const& str)
    {
        atom_parser<string::const_iterator> ap;
        ast_query_type ast;
        bool r = qi::phrase_parse(str.cbegin(), str.cend(), ap, ascii::space, ast);
        cout << (r ? "parse_suceeded" : "parse_failed" ) << endl;

        precedence_visitor precedence;
        ast = boost::apply_visitor(precedence, ast);

        print_visitor p(cout);
        apply_visitor(p, ast);
        cout << endl;

        to_query_visitor q;
        return apply_visitor(q, ast);
    }
}
