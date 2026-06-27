# Custom grep
A high-performance, custom command-line regular expression engine built from scratch in modern C++17. This project mimics core functionalities of the standard UNIX `grep` utility, utilizing a custom tokenization and recursive backtracking match engine to parse complex patterns including character classes, wildcards, quantifiers, and alternations.

## Features

- **Custom Regex Engine**: Hand-rolled tokenization and matching logic completely independent of `<regex>`.
- **Advanced Pattern Support**:
  - Anchors (`^`, `$`)
  - Character classes (`\d`, `\w`)
  - Custom bracket expressions (`[abc]`, `[^abc]`)
  - Quantifiers (`+`, `?`, `*`, `{n,m}`)
  - Alternations (`(cat|dog)`)
  - Wildcard matching (`.`)
- **Robust CLI Flags**:
  - `-E <pattern>` : Specify the regular expression pattern to match.
  - `-o` : Output only the precise matching substrings rather than the full line.
  - `-r` : Recursively scan directories for matches.

## Repository Layout

```text
grep-cpp/
├── CMakeLists.txt      # Build configuration setup
├── .gitignore          # Version control ignore file
├── README.md           # Documentation
└── src/
    └── main.cpp        # Self-contained implementation file
