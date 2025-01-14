// Copyright (C) 2016 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_DOC_ENTITY_HPP_INCLUDED
#define STANDARDESE_DOC_ENTITY_HPP_INCLUDED

#include <memory>

#include <standardese/detail/entity_container.hpp>
#include <standardese/cpp_entity.hpp>
#include <standardese/md_custom.hpp>

namespace standardese
{
    class code_block_writer;
    class comment;
    class cpp_file;
    class doc_entity;
    class index;
    class md_document;
    class md_list;
    class parser;

    enum cpp_access_specifier_t : int;

    template <typename T>
    using doc_ptr = std::unique_ptr<T>;

    using doc_entity_ptr = doc_ptr<doc_entity>;

    namespace detail
    {
        struct generation_access
        {
            static void do_generate_synopsis(const doc_entity& e, const parser& p,
                                             code_block_writer& out, bool top_level);

            static void do_generate_documentation_inline(const doc_entity& e, const parser& p,
                                                         const index&             i,
                                                         md_inline_documentation& doc);
        };
    } // namespace detail

    using doc_entity_container = detail::entity_container<doc_entity, doc_entity, doc_ptr>;

    class doc_entity
    {
    public:
        enum type
        {
            file_t,
            index_t,
            cpp_entity_t,
            member_group_t,
        };

        doc_entity(const doc_entity&) = delete;
        doc_entity(doc_entity&&)      = delete;

        virtual ~doc_entity() STANDARDESE_NOEXCEPT = default;

        doc_entity& operator=(const doc_entity&) = delete;
        doc_entity& operator=(doc_entity&&) = delete;

        type get_entity_type() const STANDARDESE_NOEXCEPT
        {
            return t_;
        }

        cpp_name get_unique_name() const;

        cpp_name get_name() const
        {
            return do_get_name();
        }

        cpp_name get_index_name(bool full_name) const
        {
            return do_get_index_name(full_name);
        }

        cpp_entity::type get_cpp_entity_type() const STANDARDESE_NOEXCEPT
        {
            return do_get_cpp_entity_type();
        }

        bool has_parent() const STANDARDESE_NOEXCEPT
        {
            return parent_ != nullptr;
        }

        const doc_entity& get_parent() const STANDARDESE_NOEXCEPT
        {
            return *parent_;
        }

        bool has_comment() const STANDARDESE_NOEXCEPT
        {
            return comment_ != nullptr;
        }

        const comment& get_comment() const STANDARDESE_NOEXCEPT
        {
            return *comment_;
        }

        bool in_module() const STANDARDESE_NOEXCEPT;

        const std::string& get_module() const STANDARDESE_NOEXCEPT;

        virtual doc_entity_container::iterator begin() STANDARDESE_NOEXCEPT
        {
            return doc_entity_container::iterator{};
        }

        virtual doc_entity_container::iterator end() STANDARDESE_NOEXCEPT
        {
            return doc_entity_container::iterator{};
        }

        virtual doc_entity_container::const_iterator begin() const STANDARDESE_NOEXCEPT
        {
            return doc_entity_container::const_iterator{};
        }

        virtual doc_entity_container::const_iterator end() const STANDARDESE_NOEXCEPT
        {
            return doc_entity_container::const_iterator{};
        }

        void generate_synopsis(const parser& p, code_block_writer& out) const
        {
            do_generate_synopsis(p, out, true);
        }

        void generate_documentation(const parser& p, const index& i, md_document& doc) const
        {
            do_generate_documentation(p, i, doc, 1u);
        }

    protected:
        doc_entity(type t, const doc_entity* parent, const comment* c) STANDARDESE_NOEXCEPT;

        void set_parent(const doc_entity* parent)
        {
            parent_ = parent;
        }

        virtual void do_generate_documentation(const parser& p, const index& i, md_document& doc,
                                               unsigned level) const = 0;

        virtual void do_generate_documentation_inline(const parser&, const index&,
                                                      md_inline_documentation&) const
        {
        }

        virtual void do_generate_synopsis(const parser& p, code_block_writer& out,
                                          bool top_level) const = 0;

    private:
        virtual cpp_name do_get_name() const                     = 0;
        virtual cpp_name do_get_unique_name() const              = 0;
        virtual cpp_name do_get_index_name(bool full_name) const = 0;

        virtual cpp_entity::type do_get_cpp_entity_type() const STANDARDESE_NOEXCEPT = 0;

        doc_entity_ptr    next_;
        const doc_entity* parent_;
        const comment*    comment_;
        type              t_;

        template <class T, class Base, template <typename> class Ptr>
        friend class detail::entity_container;
        friend class doc_container_cpp_entity;
        friend class doc_member_group;
        friend struct detail::generation_access;
    };

    namespace detail
    {
        struct doc_ptr_access
        {
            template <typename T, typename... Args>
            static doc_ptr<T> make(Args&&... args)
            {
                return doc_ptr<T>(new T(std::forward<Args>(args)...));
            }
        };

        template <typename T, typename... Args>
        doc_ptr<T> make_doc_ptr(Args&&... args)
        {
            return doc_ptr_access::make<T>(std::forward<Args>(args)...);
        }
    } // namespace detail

    class doc_cpp_entity : public doc_entity
    {
    public:
        const cpp_entity& get_cpp_entity() const STANDARDESE_NOEXCEPT
        {
            return *entity_;
        }

        void set_cpp_entity(const cpp_entity& e) STANDARDESE_NOEXCEPT
        {
            entity_ = &e;
        }

    protected:
        void do_generate_documentation_base(const parser& p, const index& i, md_document& doc,
                                            unsigned level) const;

        doc_cpp_entity(const doc_entity* parent, const cpp_entity& e, const comment* c)
        : doc_entity(doc_entity::cpp_entity_t, parent, c), entity_(&e)
        {
        }

    private:
        cpp_name do_get_name() const override
        {
            return entity_->get_name();
        }

        cpp_name do_get_unique_name() const override;

        cpp_name do_get_index_name(bool full_name) const override;

        cpp_entity::type do_get_cpp_entity_type() const STANDARDESE_NOEXCEPT override
        {
            return entity_->get_entity_type();
        }

        const cpp_entity* entity_;

        friend detail::doc_ptr_access;
    };

    bool is_inline_cpp_entity(cpp_entity::type t) STANDARDESE_NOEXCEPT;

    class doc_inline_cpp_entity final : public doc_cpp_entity
    {
    protected:
        void do_generate_documentation(const parser& p, const index& i, md_document& doc,
                                       unsigned level) const override;

        void do_generate_synopsis(const parser& p, code_block_writer& out,
                                  bool top_level) const override;

        void do_generate_documentation_inline(const parser& p, const index& i,
                                              md_inline_documentation& doc) const override;

    private:
        doc_inline_cpp_entity(const doc_entity* parent, const cpp_entity& e, const comment* c);

        friend detail::doc_ptr_access;
    };

    class doc_cpp_access_entity final : public doc_cpp_entity
    {
    protected:
        void do_generate_documentation(const parser&, const index&, md_document&,
                                       unsigned) const override
        {
        }

        void do_generate_synopsis(const parser& p, code_block_writer& out,
                                  bool top_level) const override;

    private:
        doc_cpp_access_entity(const doc_entity* parent, const cpp_entity& e, const comment* c);

        friend detail::doc_ptr_access;
    };

    class doc_leave_cpp_entity final : public doc_cpp_entity
    {
    protected:
        void do_generate_documentation(const parser& p, const index& i, md_document& doc,
                                       unsigned level) const override;

        void do_generate_synopsis(const parser& p, code_block_writer& out,
                                  bool top_level) const override;

    private:
        doc_leave_cpp_entity(const doc_entity* parent, const cpp_entity& e, const comment* c);

        friend detail::doc_ptr_access;
    };

    class doc_container_cpp_entity final : public doc_cpp_entity, private doc_entity_container
    {
    public:
        void add_entity(doc_entity_ptr e)
        {
            if (e)
                doc_entity_container::add_entity(std::move(e));
        }

        bool empty() const STANDARDESE_NOEXCEPT
        {
            return doc_entity_container::empty();
        }

        doc_entity_container::iterator begin() STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::begin();
        }

        doc_entity_container::iterator end() STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::end();
        }

        doc_entity_container::const_iterator begin() const STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::begin();
        }

        doc_entity_container::const_iterator end() const STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::end();
        }

    protected:
        void do_generate_documentation(const parser& p, const index& i, md_document& doc,
                                       unsigned level) const override;

        void do_generate_synopsis(const parser& p, code_block_writer& out,
                                  bool top_level) const override;

    private:
        doc_container_cpp_entity(const doc_entity* parent, const cpp_entity& e, const comment* c);

        friend detail::doc_ptr_access;
        friend class doc_file;
    };

    class doc_member_group final : public doc_entity, doc_entity_container
    {
    public:
        static doc_ptr<doc_member_group> make(const doc_entity& parent, const comment& c);

        void add_entity(doc_entity_ptr e)
        {
            if (e)
            {
                e->set_parent(this);
                doc_entity_container::add_entity(std::move(e));
            }
        }

        doc_entity_container::iterator begin() STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::begin();
        }

        doc_entity_container::iterator end() STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::end();
        }

        doc_entity_container::const_iterator begin() const STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::begin();
        }

        doc_entity_container::const_iterator end() const STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::end();
        }

        std::size_t group_id() const STANDARDESE_NOEXCEPT;

    protected:
        void do_generate_documentation(const parser& p, const index& i, md_document& doc,
                                       unsigned level) const override;

        void do_generate_synopsis(const parser& p, code_block_writer& out,
                                  bool top_level) const override;

    private:
        cpp_name do_get_name() const override
        {
            return doc_entity_container::begin()->do_get_name();
        }

        cpp_name do_get_index_name(bool full_name) const override
        {
            return doc_entity_container::begin()->do_get_index_name(full_name);
        }

        cpp_name do_get_unique_name() const override
        {
            return "";
        }

        cpp_entity::type do_get_cpp_entity_type() const STANDARDESE_NOEXCEPT override
        {
            return doc_entity_container::begin()->do_get_cpp_entity_type();
        }

        doc_member_group(const doc_entity* parent, const comment& c)
        : doc_entity(doc_cpp_entity::member_group_t, parent, &c)
        {
        }

        friend detail::doc_ptr_access;
    };

    class doc_file final : public doc_entity
    {
    public:
        static doc_ptr<doc_file> parse(const parser& p, const index& i, std::string output_name,
                                       const cpp_file& f);

        doc_entity_container::iterator begin() STANDARDESE_NOEXCEPT override
        {
            return file_->begin();
        }

        doc_entity_container::iterator end() STANDARDESE_NOEXCEPT override
        {
            return file_->end();
        }

        doc_entity_container::const_iterator begin() const STANDARDESE_NOEXCEPT override
        {
            return static_cast<const doc_entity&>(*file_).begin();
        }

        doc_entity_container::const_iterator end() const STANDARDESE_NOEXCEPT override
        {
            return static_cast<const doc_entity&>(*file_).end();
        }

        cpp_name get_file_name() const
        {
            return output_name_;
        }

    protected:
        void do_generate_documentation(const parser& p, const index& i, md_document& doc,
                                       unsigned level) const override;

        void do_generate_synopsis(const parser& p, code_block_writer& out,
                                  bool top_level) const override;

    private:
        cpp_name do_get_name() const override
        {
            return file_->get_name();
        }

        cpp_name do_get_unique_name() const override
        {
            return file_->get_cpp_entity().get_unique_name(true);
        }

        cpp_name do_get_index_name(bool full_name) const override
        {
            return file_->get_index_name(full_name);
        }

        cpp_entity::type do_get_cpp_entity_type() const STANDARDESE_NOEXCEPT override
        {
            return cpp_entity::file_t;
        }

        doc_file(std::string output_name, doc_ptr<doc_container_cpp_entity> f)
        : doc_entity(doc_entity::file_t, nullptr, f->has_comment() ? &f->get_comment() : nullptr),
          file_(std::move(f)),
          output_name_(output_name)
        {
        }

        doc_ptr<doc_container_cpp_entity> file_;
        std::string                       output_name_;

        friend detail::doc_ptr_access;
    };
} // namespace standardese

#endif // STANDARDESE_DOC_ENTITY_HPP_INCLUDED
