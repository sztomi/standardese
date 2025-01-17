// Copyright (C) 2016 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <standardese/comment.hpp>

#include <catch.hpp>

#include <standardese/detail/raw_comment.hpp>
#include <standardese/cpp_function.hpp>
#include <standardese/cpp_template.hpp>
#include <standardese/doc_entity.hpp>
#include <standardese/index.hpp>
#include <standardese/md_blocks.hpp>
#include <standardese/md_inlines.hpp>
#include <standardese/parser.hpp>

#include "test_parser.hpp"

using namespace standardese;

std::string get_text(const md_paragraph& paragraph)
{
    std::string result;
    for (auto& child : paragraph)
        if (child.get_entity_type() == md_entity::text_t)
            result += dynamic_cast<const md_text&>(child).get_string();
        else if (child.get_entity_type() == md_entity::soft_break_t)
            result += '\n';
    return result;
}

const comment& parse_comment(parser& p, std::string source)
{
    auto  tu     = parse(p, "md_comment", (source + "\nint a;").c_str());
    auto& entity = *tu.get_file().begin();

    auto res = p.get_comment_registry().lookup_comment(entity, nullptr);
    REQUIRE(res);
    return *res;
}

TEST_CASE("md_comment", "[doc]")
{
    parser p(test_logger);

    SECTION("comment styles")
    {
        auto source = R"(///   C++ style.

            //! C++ exclamation.

            /** C style. */

            // ignored
            /* ignored as well */

            /*! C exclamation.
            */

            /** C style
              *
              *   multiline.
              */

            /** C style
            /// C++ multiline. */

            /// Multiple
            //! C++
            /** and C style.
            */

            foo, //< End line style.
            bar, //< End line style.
            /// Continued.

            /**/
)";

        auto comments = detail::read_comments(source);
        REQUIRE(comments.size() == 10);

        REQUIRE(comments[0].content == "  C++ style.");
        REQUIRE(comments[0].count_lines == 1u);
        REQUIRE(comments[0].end_line == 1u);

        REQUIRE(comments[1].content == "C++ exclamation.");
        REQUIRE(comments[1].count_lines == 1u);
        REQUIRE(comments[1].end_line == 3u);

        REQUIRE(comments[2].content == "C style.");
        REQUIRE(comments[2].count_lines == 1u);
        REQUIRE(comments[2].end_line == 5u);

        REQUIRE(comments[3].content == "C exclamation.");
        REQUIRE(comments[3].count_lines == 2u);
        REQUIRE(comments[3].end_line == 11u);

        REQUIRE(comments[4].content == "C style\n\n  multiline.");
        REQUIRE(comments[4].count_lines == 4u);
        REQUIRE(comments[4].end_line == 16u);

        REQUIRE(comments[5].content == "C style\n/// C++ multiline.");
        REQUIRE(comments[5].count_lines == 2u);
        REQUIRE(comments[5].end_line == 19u);

        REQUIRE(comments[6].content == "Multiple\nC++");
        REQUIRE(comments[6].count_lines == 2u);
        REQUIRE(comments[6].end_line == 22u);

        REQUIRE(comments[7].content == "and C style.");
        REQUIRE(comments[7].count_lines == 2u);
        REQUIRE(comments[7].end_line == 24u);

        REQUIRE(comments[8].content == "End line style.");
        REQUIRE(comments[8].count_lines == 1u);
        REQUIRE(comments[8].end_line == 26u);

        REQUIRE(comments[9].content == "End line style.\nContinued.");
        REQUIRE(comments[9].count_lines == 2u);
        REQUIRE(comments[9].end_line == 28u);
    }
    SECTION("simple parsing")
    {
        auto& comment = parse_comment(p, R"(/// Hello World.)");
        auto  count   = 0u;
        for (auto& child : comment.get_content())
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);

            auto& paragraph = dynamic_cast<const md_paragraph&>(child);
            REQUIRE(get_text(paragraph) == std::string("Hello World."));
            REQUIRE(paragraph.get_section_type() == section_type::brief);
            ++count;
        }
        REQUIRE(count == 1u);
    }
    SECTION("multiple sections explicit")
    {
        auto& comment = parse_comment(p, R"(/**
\brief A

\details B
C
*/)");

        auto count = 0u;
        for (auto& child : comment.get_content())
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);
            INFO(get_text(paragraph));

            if (get_text(paragraph) == "A")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::brief);
            }
            else if (get_text(paragraph) == "B\nC")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::details);
            }
            else
                REQUIRE(false);
        }
        REQUIRE(count == 2u);
    }
    SECTION("multiple sections implicit")
    {
        auto& comment = parse_comment(p, R"(/**
* A
*
* B
* C /// C
*/)");

        auto count = 0u;
        for (auto& child : comment.get_content())
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);

            if (get_text(paragraph) == "A")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::brief);
            }
            else if (get_text(paragraph) == "B\nC /// C")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::details);
            }
            else
                REQUIRE(false);
        }
        REQUIRE(count == 2u);
    }
    SECTION("cherry pick other commands")
    {
        auto& comment = parse_comment(p, R"(
/// \effects A A
/// A A
///
/// \returns B B
///
/// \error_conditions C C)");

        auto count = 0u;
        for (auto& child : comment.get_content())
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);
            INFO(get_text(paragraph));

            if (get_text(paragraph) == "A A\nA A")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::effects);
            }
            else if (get_text(paragraph) == "B B")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::returns);
            }
            else if (get_text(paragraph) == "C C")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::error_conditions);
            }
            else
                REQUIRE(paragraph.get_section_type() == section_type::brief);
        }
        REQUIRE(count == 3u);
    }
    SECTION("one paragraph")
    {
        const char* source;
        SECTION("C style")
        {
            source = R"(/**
\effects A
A
\brief E
\requires B
B2
\exclude
C
\details D
\brief E
\notes F/
\notes G
*/)";
        }
        SECTION("C style continuation")
        {
            source = R"(/**
* \effects A
* A
* \brief E
* \requires B
* B2
* \exclude
* C
* \details D
* \brief E
* \notes F/
* \notes G
*/)";
        }
        SECTION("C++ style")
        {
            source = R"(/// \effects A
/// A
/// \brief E
/// \requires B
/// B2
/// \exclude
/// C
/// \details D
/// \brief E
/// \notes F/
/// \notes G)";
        }

        auto& comment = parse_comment(p, source);

        auto count = 0u;
        for (auto& child : comment.get_content())
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);
            INFO('"' + get_text(paragraph) + '"');

            if (get_text(paragraph) == "A\nA")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::effects);
            }
            else if (get_text(paragraph) == "B\nB2")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::requires);
            }
            else if (get_text(paragraph) == "D" || get_text(paragraph) == "C")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::details);
            }
            else if (get_text(paragraph) == "F")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::notes);
            }
            else if (get_text(paragraph) == "G")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::notes);
            }
            else
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::brief);
                REQUIRE(get_text(paragraph) == "E\nE");
            }
        }
        REQUIRE(count == 7u);
    }
    SECTION("commands")
    {
        auto& comment = parse_comment(p, R"(
                                      /// Normal markup.
                                      ///
                                      /// \exclude
                                      /// \unique_name foo)");
        REQUIRE(comment.is_excluded());
        REQUIRE(comment.get_unique_name_override() == "foo");

        auto count = 0u;
        for (auto& child : comment.get_content())
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);
            INFO('"' << get_text(paragraph) << '"');
            REQUIRE(paragraph.get_section_type() == section_type::brief);
            REQUIRE(get_text(paragraph) == "Normal markup.");
            ++count;
        }
        REQUIRE(count == 1u);
    }
}

std::string get_text(const md_comment& comment)
{
    std::string result;
    for (auto& child : comment)
    {
        if (child.get_entity_type() != md_entity::paragraph_t)
            continue;
        auto& par = static_cast<const md_paragraph&>(child);
        result += get_text(par);
    }
    return result;
}

std::string get_text(const doc_entity& e)
{
    REQUIRE(e.has_comment());
    auto& content = e.get_comment().get_content();
    return get_text(content);
}

TEST_CASE("comment-matching", "[doc]")
{
    parser p(test_logger);

    auto source = R"(
        /// a
        #define a

        /// \file
        ///
        /// file

        void b(int g);

        /// \entity b
        ///
        /// b
        ///
        /// \param g g

        /// \entity f
        ///
        /// e

        /// c
        /// \unique_name c2
        ///
        /// \param d d
        /// \param e
        /// \unique_name f
        void c(int d, int e, int k);

        /// \entity c2.k
        /// k

        /// h
        ///
        /// \param g g-param
        ///
        /// \base g g-base
        template <typename g>
        struct h : g {};

        struct i {}; //< i
        struct j {}; //< j
      )";

    auto tu = parse(p, "comment-matching", source);

    standardese::index i;
    auto               file = doc_file::parse(p, i, "", tu.get_file());

    REQUIRE(get_text(*file) == "file");
    for (auto& entity : *file)
    {
        INFO(entity.get_name().c_str());
        REQUIRE(get_text(entity) == entity.get_name().c_str());

        if (is_function_like(entity.get_cpp_entity_type()))
        {
            for (auto& param : static_cast<doc_container_cpp_entity&>(entity))
            {
                INFO(param.get_name().c_str());
                REQUIRE(get_text(param) == param.get_name().c_str());
            }
        }
        else if (entity.get_cpp_entity_type() == cpp_entity::class_template_t)
        {
            for (auto& child : static_cast<doc_container_cpp_entity&>(entity))
            {
                INFO(child.get_name().c_str());
                if (child.get_cpp_entity_type() == cpp_entity::base_class_t)
                    REQUIRE(get_text(child) == "g-base");
                else
                    REQUIRE(get_text(child) == "g-param");
            }
        }
    }
}
