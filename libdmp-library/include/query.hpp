#pragma once

#include "library-entry.hpp"

#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/regex.hpp>

#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <type_traits>

namespace dmp_library {

struct ast_and;
struct ast_or;
struct ast_not;
struct ast_atom;
struct ast_nest;

struct query;
struct atom;
struct _not;
struct _and;
struct _or;

typedef boost::variant<ast_atom, boost::recursive_wrapper<ast_or>, boost::recursive_wrapper<ast_and>, boost::recursive_wrapper<ast_not>, boost::recursive_wrapper<ast_nest>> ast_query_type;

struct print_visitor : public boost::static_visitor<std::ostream&>
{
    std::ostream& os;

    print_visitor(std::ostream& os)
    : os(os)
    {}

    template <typename T>
    std::ostream& operator()(T x)
    {
        return os << x;
    }
};

struct ast_atom {
    std::string field;
    std::string modifier;
    std::string input;

    typedef atom query_type;

    friend std::ostream& operator<<(std::ostream& os, ast_atom const& atom)
    {
        std::cout << atom.field << " " << atom.modifier << " " << atom.input;
    }
};

struct ast_not {
    ast_query_type negated;

    typedef _not query_type;

    friend std::ostream& operator<<(std::ostream& os, ast_not const& _not)
    {
        os << " not: ";
        print_visitor p(os);
        boost::apply_visitor(p, _not.negated);
        return os;
    }
};

struct ast_or {
    ast_query_type lh;
    ast_query_type rh;

    typedef _or query_type;

    friend std::ostream& operator<<(std::ostream& os, ast_or const& _or)
    {
        print_visitor p(os);
        os << " :or: ";

        os << std::endl << "\t";
        boost::apply_visitor(p, _or.lh);

        os << std::endl << "\t";
        boost::apply_visitor(p, _or.rh);
        return os;
    }
};

struct ast_and {
    ast_query_type lh;
    ast_query_type rh;

    typedef _and query_type;

    friend std::ostream& operator<<(std::ostream& os, ast_and const& _and)
    {
        print_visitor p(os);
        boost::apply_visitor(p, _and.lh);
        os << " :and: ";
        boost::apply_visitor(p, _and.rh);
        return os;
    }
};

struct ast_nest {
    ast_query_type arg;

    friend std::ostream& operator<<(std::ostream& os, ast_nest const& nest)
    {
        os << " (";
        print_visitor p(os);
        boost::apply_visitor(p, nest.arg);
        os << ") ";
        return os;
    }
};

template <typename T>
struct rotate_visitor : public boost::static_visitor<T>
{
    T const& parent;

    rotate_visitor(T const& parent)
    :parent(parent)
    {}

    template <typename V>
    T rotate(std::false_type, V const& x)
    {
        return parent;
    }

    T rotate(std::true_type, T const& child)
    {
        T new_parent;
        T new_child;

        new_child.lh = parent.lh;
        new_child.rh = child.lh;
        new_parent.lh = new_child;
        new_parent.rh = child.rh;
        return new_parent;
    }

    template <typename U>
    T operator()(U const& x)
    {
        return rotate(std::is_same<T, U>(), x);
    }
};

struct precedence_visitor : public boost::static_visitor<ast_query_type>
{
    ast_query_type operator()(ast_atom const& x)
    {
        return x;
    }

    ast_query_type operator()(ast_not const& x)
    {
        precedence_visitor precedence;
        ast_not ret;
        ret.negated = boost::apply_visitor(precedence, x.negated);
        return ret;
    }

    ast_query_type operator()(ast_nest const& x)
    {
        precedence_visitor precedence;
        ast_nest ret;
        ret.arg = boost::apply_visitor(precedence, x.arg);
        return ret;
    }

    template <typename T>
    ast_query_type operator()(T const& x)
    {
        rotate_visitor<T> rotate(x);
        T ret = boost::apply_visitor(rotate, x.rh);

        precedence_visitor precedence;
        ret.rh = boost::apply_visitor(precedence, ret.rh);
        return ret;
    }
};


struct to_query_visitor : public boost::static_visitor<std::shared_ptr<query>>
{
    template <typename T>
    std::shared_ptr<query> operator()(T x)
    {
        return std::make_shared<typename T::query_type>(x);
    }

    std::shared_ptr<query> operator()(ast_nest nest)
    {
        to_query_visitor v;
        boost::apply_visitor(v, nest.arg);
    }
};

struct query
{
    to_query_visitor to_query;

    virtual ~query(){}

    enum class field
    {
        artist,
        title,
        album
    };

    enum class modifier
    {
        contains,
        is
    };

    query::field to_field(std::string const& x);
    query::modifier to_modifier(std::string const& x);

    virtual std::vector<size_t> handle_search(std::vector<library_entry> const& library) = 0;
};

struct atom : public query
{
    query::field field;
    query::modifier modifier;
    std::string query_string;

    atom(ast_atom const& ast);
    atom(query::field field, query::modifier modifier, std::string query_string);

    std::string const& get_field_string(query::field const& f, library_entry const& entry);
    boost::regex const get_regex(query::modifier const& m);

    std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _and : public query
{
    std::shared_ptr<query> lh;
    std::shared_ptr<query> rh;

    _and(ast_and const& ast);
    _and(std::shared_ptr<query> lh, std::shared_ptr<query> rh);
    std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _or : public query
{
    std::shared_ptr<query> lh;
    std::shared_ptr<query> rh;

    _or(ast_or const& ast);
    _or(std::shared_ptr<query> lh, std::shared_ptr<query> rh);
    std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _not : public query
{
    struct sequence_generator {
        int current;

        sequence_generator()
        : current(0)
        {}

        int operator()() {return current++;}
    };

    std::shared_ptr<query> negated;

    _not(ast_not const& ast);
    _not(std::shared_ptr<query> arg);
    std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

}
