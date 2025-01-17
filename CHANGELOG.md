## 0.3-1

### Documentation

* Now generates inline docs for member groups as well
* Now generates synopsis for undocumented entities again
* Hide template parameters in class names
* Add support for `\n` and `\t` in synopsis override
* Add support for documentation comments in macros
* Add `doc_` prefix to all documentation output files

### Library

* Allow multiple registration of entities (warning, not a critical error)
* Ignore `static_assert` silently
* Add support for friend function definitiones
* Add heuristic for include guard detection
* Improve libclang diagnostic output
* Various other bugfixes

### Tool

* Add `output.show_complex_noexcept` option that controls whether complex noexcept expressions will be shown in the synopsis
* Add `output.prefix` option that sets a global prefix for all output
* Improve standardese-config.cmake

## 0.3

### Documentation

* Add `synopsis` command to override the synopsis
* Add `group` command to group documentation together
* Add `module` command as a way to categorize entities
* Add template language
* Allow `unique_name` on files to override the output name
* Allow name lookup on entity links (when link starts with `*` or `?`)
* *Breaking:* Change hard line break character to a forward slash
* *Breaking:* Require `entity` and `file` command to be first in a comment
* *Breaking:* Remove template parameters for function template unique name
* Improve documentation headings
* Fix termination of section by all special commands
* Fix matching of end-of-line comments

### Library

* Parsing bugfixes
* Use clang as preprocessor instead of Boost.Wave
* Rewrite generation and synopsis to allow more advanced output

### Tool

* Add `output.inline_doc` option to enable inline documentation
* Add `output.show_modules` option to enable/disable showing the module of an entity in the output
* Add `output.show_macro_replacement` option to enable/disable showing the macro replacement in the synopsis
* Add `output.show_group_member_id` option to enable/disable showing an integral id for members of a member group
* Add `compilation.clang_binary` option to control clang binary used as preprocessor
* Add `compilation.ms_compatibility` option to give more control over MSVC compatibility as well as tweaked `compilation.ms_extensions`
* Add `template.*` options and other template support

## 0.2-2

### Documentation

* Simplify comment format: Now a special command can be at the beginning of each new line
* Remove section merging as it has become unnecessary
* Section is now active until paragraph end, another special command or hard line break

### Library

* Clarify AST vs semantic parent of `cpp_entity`
* Change preprocessing: Now the entire file is preprocessed before passing it to libclang
* Generate full synopsis for non-documented entities
* Bugfixes, bugfixes and bugfixes

### Tool

* Remove `comment.implicit_paragraph` option, it is obsolete
* Add `compilation.ms_extensions` option

## 0.2-1

### Buildsystem

* Add pre-built binaries for Travis CI and Windows
* Improve Travis dependency management
* Improve Appveyor

## 0.2

### Buildsystem

* New method of handling external projects

### Documentation

* Add ability to use almost arbitrary Markdown in the documentation comments

* Link to other entities via `[link-text](<> "unique-name")`, where `unique-name` is the name of the entity with all scopes and (template) parameters, `link-text` is the arbitrary text of the link and the literal `<>` denotes an empty URL.
 This can be abbreviated to `[unique-name]()` if you don't need a special text.
* New commmand: `exclude` to exclude entites from the output via comment

* New command: `unique_name` to override the unique name of an entity

* Support for more comment variants (`///`, `//!`, `/** ... */` and `/*! ... */`)

* Support for end-of line comments:
```cpp
int bar; //< Bar.
/// Bar continued.
```

* New commands: `file`, `param`, `tparam` and `base` to document the current file, (template) parameters and base classes.

* New command: `entity` to document an entity from a different location given its name.

### Library

* New `md_entity` hierachy

* Redid comment parsing (twice) and sectioning (twice)

* Redid outputting, new `output_format_base` hierachy, new `output` class merely acting as wrapper; `code_block_writer` now standalone class

* New functions in `cpp_entity`

* Internal changes

### Tool

* Multithreaded documentation generation and new option `jobs/j` to specify number of threads.

* Index generation

* More output formats set by the `output.format` option: CommonMark, HTML, Latex (experimental), Man (experimental) and XML

* New options:  `input.blacklist_dotfiles`, `comment.implicit_paragraph`, `comment.external_doc`, `output.link_extension`, `output.width` and `output.tab_width`

## 0.1-1

Bugfixes, better compiler support.

## 0.1

### Buildsystem

* changed target names to reflect namespaces

* installation options

* support for libclang 3.8

* CMake integration for building with `standardese_generate()`

### Library:

* complete restructuring (seperation of comments and entity, multiple configurations)

* `standardese::compile_config` class for compilation options and `compile_commands.json` support

* new parsing with the help of Boost.Wave
 
* skip attributes when parsing

* more robust parsing, error handling options

* `standardese::entity_blacklist` to blacklist entities for synopsis and generation

* a couple of utility functions

* better detection of overridden `virtual` functions

* support for libclang 3.8 and template aliases

* many internal changes and bugfixes

### Tool

* new options for compilation and entity filtering

* verbose output and coloring options

## 0.0

First basic prototype.
