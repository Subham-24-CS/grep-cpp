#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <utility>

bool match_char(char c, const std::string &token)
{
    if (token == ".")
    {
        return c != '\n';
    }
    else if (token == "\\d")
    {
        return c >= '0' && c <= '9';
    }
    else if (token == "\\w")
    {
        return (c >= 'A' && c <= 'Z') ||
               (c >= 'a' && c <= 'z') ||
               (c == '_') ||
               (c >= '0' && c <= '9');
    }
    else if (token.front() == '[' && token.back() == ']')
    {
        if (token.length() >= 4 && token[1] == '^')
        {
            for (size_t i = 2; i < token.length() - 1; i++)
            {
                if (c == token[i])
                    return false;
            }
            return true;
        }
        else
        {
            for (size_t i = 1; i < token.length() - 1; i++)
            {
                if (c == token[i])
                    return true;
            }
            return false;
        }
    }
    else
    {
        return c == token[0];
    }
}

std::vector<std::string> tokenize(const std::string &pattern)
{
    std::vector<std::string> tokens;
    size_t i = 0;
    while (i < pattern.length())
    {
        if (pattern[i] == '\\' && i + 1 < pattern.length())
        {
            std::string token = pattern.substr(i, 2);
            i += 2;
            if (i < pattern.length())
            {
                if (pattern[i] == '+' || pattern[i] == '?' || pattern[i] == '*')
                {
                    token += pattern[i];
                    i++;
                }
                else if (pattern[i] == '{')
                {
                    size_t brace_end = pattern.find('}', i);
                    if (brace_end != std::string::npos)
                    {
                        token += pattern.substr(i, brace_end - i + 1);
                        i = brace_end + 1;
                    }
                }
            }
            tokens.push_back(token);
        }
        else if (pattern[i] == '[')
        {
            size_t end = pattern.find(']', i);
            if (end != std::string::npos)
            {
                std::string token = pattern.substr(i, end - i + 1);
                i = end + 1;
                if (i < pattern.length())
                {
                    if (pattern[i] == '+' || pattern[i] == '?' || pattern[i] == '*')
                    {
                        token += pattern[i];
                        i++;
                    }
                    else if (pattern[i] == '{')
                    {
                        size_t brace_end = pattern.find('}', i);
                        if (brace_end != std::string::npos)
                        {
                            token += pattern.substr(i, brace_end - i + 1);
                            i = brace_end + 1;
                        }
                    }
                }
                tokens.push_back(token);
            }
            else
            {
                tokens.push_back(pattern.substr(i, 1));
                i++;
            }
        }
        else if (pattern[i] == '(')
        {
            size_t end = pattern.find(')', i);
            if (end != std::string::npos)
            {
                std::string token = pattern.substr(i, end - i + 1);
                i = end + 1;
                if (i < pattern.length())
                {
                    if (pattern[i] == '+' || pattern[i] == '?' || pattern[i] == '*')
                    {
                        token += pattern[i];
                        i++;
                    }
                    else if (pattern[i] == '{')
                    {
                        size_t brace_end = pattern.find('}', i);
                        if (brace_end != std::string::npos)
                        {
                            token += pattern.substr(i, brace_end - i + 1);
                            i = brace_end + 1;
                        }
                    }
                }
                tokens.push_back(token);
            }
            else
            {
                tokens.push_back(pattern.substr(i, 1));
                i++;
            }
        }
        else
        {
            std::string token = pattern.substr(i, 1);
            i++;
            if (i < pattern.length())
            {
                if (pattern[i] == '+' || pattern[i] == '?' || pattern[i] == '*')
                {
                    token += pattern[i];
                    i++;
                }
                else if (pattern[i] == '{')
                {
                    size_t brace_end = pattern.find('}', i);
                    if (brace_end != std::string::npos)
                    {
                        token += pattern.substr(i, brace_end - i + 1);
                        i = brace_end + 1;
                    }
                }
            }
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool has_plus(const std::string &token)
{
    return !token.empty() && token.back() == '+';
}

bool has_question(const std::string &token)
{
    return !token.empty() && token.back() == '?';
}

bool has_star(const std::string &token)
{
    return !token.empty() && token.back() == '*';
}

bool has_brace_quantifier(const std::string &token)
{
    return !token.empty() && token.back() == '}';
}

std::pair<int, int> parse_brace_quantifier(const std::string &token)
{
    size_t brace_start = token.find('{');
    if (brace_start == std::string::npos)
        return {1, 1};

    std::string inner = token.substr(brace_start + 1, token.length() - brace_start - 2);
    size_t comma_pos = inner.find(',');

    if (comma_pos == std::string::npos)
    {
        int n = std::stoi(inner);
        return {n, n};
    }
    else if (comma_pos == inner.length() - 1)
    {
        int n = std::stoi(inner.substr(0, comma_pos));
        return {n, -1};
    }
    else
    {
        int n = std::stoi(inner.substr(0, comma_pos));
        int m = std::stoi(inner.substr(comma_pos + 1));
        return {n, m};
    }
}

std::string get_base_token(const std::string &token)
{
    if (has_plus(token) || has_question(token) || has_star(token))
    {
        return token.substr(0, token.length() - 1);
    }
    if (has_brace_quantifier(token))
    {
        size_t brace_start = token.find('{');
        if (brace_start != std::string::npos)
        {
            return token.substr(0, brace_start);
        }
    }
    return token;
}

bool is_alternation(const std::string &token)
{
    std::string base = get_base_token(token);
    return base.length() >= 2 && base.front() == '(' && base.back() == ')';
}

std::vector<std::string> get_alternatives(const std::string &token)
{
    std::string base = get_base_token(token);

    std::vector<std::string> alts;
    std::string inner = base.substr(1, base.length() - 2);

    size_t start = 0;
    for (size_t i = 0; i <= inner.length(); i++)
    {
        if (i == inner.length() || inner[i] == '|')
        {
            alts.push_back(inner.substr(start, i - start));
            start = i + 1;
        }
    }
    return alts;
}

bool match_here(const std::string &input, size_t pos, const std::vector<std::string> &tokens, size_t token_idx, bool anchor_end);

std::vector<size_t> try_match_alternation_once(const std::string &input, size_t pos, const std::string &token, const std::vector<std::string> &tokens, size_t token_idx, bool anchor_end);

std::vector<size_t> try_match_alternation_once(const std::string &input, size_t pos, const std::string &token, const std::vector<std::string> &tokens, size_t token_idx, bool anchor_end)
{
    std::vector<size_t> end_positions;
    std::vector<std::string> alts = get_alternatives(token);

    for (const std::string &alt : alts)
    {
        std::vector<std::string> alt_tokens = tokenize(alt);
        for (size_t try_len = 0; try_len <= input.length() - pos; try_len++)
        {
            std::vector<std::string> temp_tokens = alt_tokens;
            std::string sub = input.substr(pos, try_len);
            if (match_here(sub, 0, alt_tokens, 0, true))
            {
                end_positions.push_back(pos + try_len);
            }
        }
    }
    return end_positions;
}

bool match_here(const std::string &input, size_t pos, const std::vector<std::string> &tokens, size_t token_idx, bool anchor_end)
{
    if (token_idx >= tokens.size())
    {
        if (anchor_end)
        {
            return pos == input.length();
        }
        return true;
    }

    const std::string &token = tokens[token_idx];
    std::string base = get_base_token(token);

    if (is_alternation(token))
    {
        if (has_plus(token))
        {
            std::vector<size_t> first_matches = try_match_alternation_once(input, pos, token, tokens, token_idx, anchor_end);
            if (first_matches.empty())
                return false;

            std::vector<size_t> current_positions = first_matches;
            std::vector<size_t> all_positions = first_matches;

            while (!current_positions.empty())
            {
                std::vector<size_t> next_positions;
                for (size_t p : current_positions)
                {
                    std::vector<size_t> more = try_match_alternation_once(input, p, token, tokens, token_idx, anchor_end);
                    for (size_t np : more)
                    {
                        if (np > p)
                        {
                            next_positions.push_back(np);
                            all_positions.push_back(np);
                        }
                    }
                }
                current_positions = next_positions;
            }
            std::sort(all_positions.rbegin(), all_positions.rend());
            for (size_t end_pos : all_positions)
            {
                if (match_here(input, end_pos, tokens, token_idx + 1, anchor_end))
                {
                    return true;
                }
            }
            return false;
        }
        else if (has_question(token))
        {
            std::vector<size_t> matches = try_match_alternation_once(input, pos, token, tokens, token_idx, anchor_end);
            for (size_t end_pos : matches)
            {
                if (match_here(input, end_pos, tokens, token_idx + 1, anchor_end))
                {
                    return true;
                }
            }
            return match_here(input, pos, tokens, token_idx + 1, anchor_end);
        }
        else
        {
            std::vector<std::string> alts = get_alternatives(token);
            for (const std::string &alt : alts)
            {
                std::vector<std::string> alt_tokens = tokenize(alt);
                std::vector<std::string> combined;
                for (const auto &t : alt_tokens)
                {
                    combined.push_back(t);
                }
                for (size_t j = token_idx + 1; j < tokens.size(); j++)
                {
                    combined.push_back(tokens[j]);
                }

                if (match_here(input, pos, combined, 0, anchor_end))
                {
                    return true;
                }
            }
            return false;
        }
    }
    else if (has_plus(token))
    {
        if (pos >= input.length() || !match_char(input[pos], base))
        {
            return false;
        }

        size_t count = 1;
        while (pos + count < input.length() && match_char(input[pos + count], base))
        {
            count++;
        }

        for (size_t len = count; len >= 1; len--)
        {
            if (match_here(input, pos + len, tokens, token_idx + 1, anchor_end))
            {
                return true;
            }
        }
        return false;
    }
    else if (has_star(token))
    {
        size_t count = 0;
        while (pos + count < input.length() && match_char(input[pos + count], base))
        {
            count++;
        }
        for (size_t len = count; ; len--)
        {
            if (match_here(input, pos + len, tokens, token_idx + 1, anchor_end))
            {
                return true;
            }
            if (len == 0) break;
        }
        return false;
    }
    else if (has_brace_quantifier(token))
    {
        auto [min_count, max_count] = parse_brace_quantifier(token);
        size_t count = 0;
        while (pos + count < input.length() && match_char(input[pos + count], base))
        {
            count++;
            if (max_count >= 0 && (int)count >= max_count)
                break;
        }
        if ((int)count < min_count)
            return false;
        size_t actual_max = (max_count < 0) ? count : std::min(count, (size_t)max_count);
        for (size_t len = actual_max; len >= (size_t)min_count; len--)
        {
            if (match_here(input, pos + len, tokens, token_idx + 1, anchor_end))
            {
                return true;
            }
        }
        return false;
    }
    else if (has_question(token))
    {
        if (pos < input.length() && match_char(input[pos], base))
        {
            if (match_here(input, pos + 1, tokens, token_idx + 1, anchor_end))
            {
                return true;
            }
        }
        return match_here(input, pos, tokens, token_idx + 1, anchor_end);
    }
    else
    {
        if (pos >= input.length())
        {
            return false;
        }
        if (!match_char(input[pos], base))
        {
            return false;
        }
        return match_here(input, pos + 1, tokens, token_idx + 1, anchor_end);
    }
}

bool match_at(const std::string &input, size_t pos, const std::vector<std::string> &tokens, bool anchor_end)
{
    return match_here(input, pos, tokens, 0, anchor_end);
}

int match_length_here(const std::string &input, size_t pos, const std::vector<std::string> &tokens, size_t token_idx, bool anchor_end)
{
    if (token_idx >= tokens.size())
    {
        if (anchor_end)
        {
            return pos == input.length() ? 0 : -1;
        }
        return 0;
    }

    const std::string &token = tokens[token_idx];
    std::string base = get_base_token(token);

    if (is_alternation(token))
    {
        if (has_plus(token))
        {
            std::vector<size_t> first_matches = try_match_alternation_once(input, pos, token, tokens, token_idx, anchor_end);
            if (first_matches.empty())
                return -1;

            std::vector<size_t> current_positions = first_matches;
            std::vector<size_t> all_positions = first_matches;

            while (!current_positions.empty())
            {
                std::vector<size_t> next_positions;
                for (size_t p : current_positions)
                {
                    std::vector<size_t> more = try_match_alternation_once(input, p, token, tokens, token_idx, anchor_end);
                    for (size_t np : more)
                    {
                        if (np > p)
                        {
                            next_positions.push_back(np);
                            all_positions.push_back(np);
                        }
                    }
                }
                current_positions = next_positions;
            }
            std::sort(all_positions.rbegin(), all_positions.rend());
            for (size_t end_pos : all_positions)
            {
                int rest = match_length_here(input, end_pos, tokens, token_idx + 1, anchor_end);
                if (rest >= 0)
                {
                    return (int)(end_pos - pos) + rest;
                }
            }
            return -1;
        }
        else if (has_question(token))
        {
            std::vector<size_t> matches = try_match_alternation_once(input, pos, token, tokens, token_idx, anchor_end);
            for (size_t end_pos : matches)
            {
                int rest = match_length_here(input, end_pos, tokens, token_idx + 1, anchor_end);
                if (rest >= 0)
                {
                    return (int)(end_pos - pos) + rest;
                }
            }
            int rest = match_length_here(input, pos, tokens, token_idx + 1, anchor_end);
            if (rest >= 0)
                return rest;
            return -1;
        }
        else
        {
            std::vector<std::string> alts = get_alternatives(token);
            for (const std::string &alt : alts)
            {
                std::vector<std::string> alt_tokens = tokenize(alt);
                std::vector<std::string> combined;
                for (const auto &t : alt_tokens)
                {
                    combined.push_back(t);
                }
                for (size_t j = token_idx + 1; j < tokens.size(); j++)
                {
                    combined.push_back(tokens[j]);
                }

                int len = match_length_here(input, pos, combined, 0, anchor_end);
                if (len >= 0)
                {
                    return len;
                }
            }
            return -1;
        }
    }
    else if (has_plus(token))
    {
        if (pos >= input.length() || !match_char(input[pos], base))
        {
            return -1;
        }

        size_t count = 1;
        while (pos + count < input.length() && match_char(input[pos + count], base))
        {
            count++;
        }

        for (size_t len = count; len >= 1; len--)
        {
            int rest = match_length_here(input, pos + len, tokens, token_idx + 1, anchor_end);
            if (rest >= 0)
            {
                return (int)len + rest;
            }
        }
        return -1;
    }
    else if (has_star(token))
    {
        size_t count = 0;
        while (pos + count < input.length() && match_char(input[pos + count], base))
        {
            count++;
        }
        for (size_t len = count; ; len--)
        {
            int rest = match_length_here(input, pos + len, tokens, token_idx + 1, anchor_end);
            if (rest >= 0)
            {
                return (int)len + rest;
            }
            if (len == 0) break;
        }
        return -1;
    }
    else if (has_brace_quantifier(token))
    {
        auto [min_count, max_count] = parse_brace_quantifier(token);

        size_t count = 0;
        while (pos + count < input.length() && match_char(input[pos + count], base))
        {
            count++;
            if (max_count >= 0 && (int)count >= max_count)
                break;
        }
        if ((int)count < min_count)
            return -1;
        size_t actual_max = (max_count < 0) ? count : std::min(count, (size_t)max_count);
        for (size_t len = actual_max; len >= (size_t)min_count; len--)
        {
            int rest = match_length_here(input, pos + len, tokens, token_idx + 1, anchor_end);
            if (rest >= 0)
            {
                return (int)len + rest;
            }
        }
        return -1;
    }
    else if (has_question(token))
    {
        if (pos < input.length() && match_char(input[pos], base))
        {
            int rest = match_length_here(input, pos + 1, tokens, token_idx + 1, anchor_end);
            if (rest >= 0)
            {
                return 1 + rest;
            }
        }
        int rest = match_length_here(input, pos, tokens, token_idx + 1, anchor_end);
        if (rest >= 0)
            return rest;
        return -1;
    }
    else
    {
        if (pos >= input.length())
        {
            return -1;
        }
        if (!match_char(input[pos], base))
        {
            return -1;
        }
        int rest = match_length_here(input, pos + 1, tokens, token_idx + 1, anchor_end);
        if (rest >= 0)
            return 1 + rest;
        return -1;
    }
}

std::vector<std::string> find_all_matches(const std::string &input_line, const std::string &pattern)
{
    std::vector<std::string> matches;
    std::string pat = pattern;
    bool anchor_start = false;
    bool anchor_end = false;

    if (!pat.empty() && pat[0] == '^')
    {
        anchor_start = true;
        pat = pat.substr(1);
    }

    if (!pat.empty() && pat.back() == '$')
    {
        anchor_end = true;
        pat = pat.substr(0, pat.length() - 1);
    }

    std::vector<std::string> tokens = tokenize(pat);

    if (tokens.empty())
    {
        return matches;
    }

    size_t i = 0;
    if (anchor_start)
    {
        int len = match_length_here(input_line, 0, tokens, 0, anchor_end);
        if (len >= 0)
        {
            matches.push_back(input_line.substr(0, len));
        }
        return matches;
    }

    while (i <= input_line.length())
    {
        int len = match_length_here(input_line, i, tokens, 0, anchor_end);
        if (len >= 0)
        {
            matches.push_back(input_line.substr(i, len));
            if (len == 0)
            {
                i++;
            }
            else
            {
                i += len;
            }
        }
        else
        {
            i++;
        }
    }
    return matches;
}

bool match_pattern(const std::string &input_line, const std::string &pattern)
{
    std::string pat = pattern;
    bool anchor_start = false;
    bool anchor_end = false;

    if (!pat.empty() && pat[0] == '^')
    {
        anchor_start = true;
        pat = pat.substr(1);
    }

    if (!pat.empty() && pat.back() == '$')
    {
        anchor_end = true;
        pat = pat.substr(0, pat.length() - 1);
    }

    std::vector<std::string> tokens = tokenize(pat);

    if (tokens.empty())
    {
        if (anchor_start && anchor_end)
        {
            return input_line.empty();
        }
        return true;
    }

    if (anchor_start)
    {
        return match_at(input_line, 0, tokens, anchor_end);
    }

    for (size_t i = 0; i <= input_line.length(); i++)
    {
        if (match_at(input_line, i, tokens, anchor_end))
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    std::cerr << "Logs from your program will appear here" << std::endl;

    bool only_matching = false;
    bool recursive = false;
    std::string pattern;
    std::vector<std::string> files;

    int arg_idx = 1;
    while (arg_idx < argc)
    {
        std::string arg = argv[arg_idx];
        if (arg == "-o")
        {
            only_matching = true;
            arg_idx++;
        }
        else if (arg == "-r")
        {
            recursive = true;
            arg_idx++;
        }
        else if (arg == "-E")
        {
            arg_idx++;
            if (arg_idx >= argc)
            {
                std::cerr << "Expected pattern after -E" << std::endl;
                return 1;
            }
            pattern = argv[arg_idx];
            arg_idx++;
        }
        else
        {
            files.push_back(arg);
            arg_idx++;
        }
    }

    if (pattern.empty())
    {
        std::cerr << "Expected -E flag with pattern" << std::endl;
        return 1;
    }

    std::string input_line;
    bool any_match = false;

    try
    {
        if (files.empty())
        {
            // Read from stdin
            while (std::getline(std::cin, input_line))
            {
                if (only_matching)
                {
                    std::vector<std::string> matches = find_all_matches(input_line, pattern);
                    for (const std::string &m : matches)
                    {
                        std::cout << m << std::endl;
                        any_match = true;
                    }
                }
                else
                {
                    if (match_pattern(input_line, pattern))
                    {
                        std::cout << input_line << std::endl;
                        any_match = true;
                    }
                }
            }
        }
        else
        {
            std::vector<std::string> all_files;
            for (const std::string &path : files)
            {
                if (recursive && std::filesystem::is_directory(path))
                {
                    for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
                    {
                        if (entry.is_regular_file())
                        {
                            all_files.push_back(entry.path().string());
                        }
                    }
                }
                else
                {
                    all_files.push_back(path);
                }
            }

            bool multiple_files = all_files.size() > 1 || recursive;

            for (const std::string &filepath : all_files)
            {
                std::ifstream file(filepath);
                if (!file.is_open())
                {
                    std::cerr << "Cannot open file: " << filepath << std::endl;
                    continue;
                }

                while (std::getline(file, input_line))
                {
                    if (only_matching)
                    {
                        std::vector<std::string> matches = find_all_matches(input_line, pattern);
                        for (const std::string &m : matches)
                        {
                            if (multiple_files)
                            {
                                std::cout << filepath << ":" << m << std::endl;
                            }
                            else
                            {
                                std::cout << m << std::endl;
                            }
                            any_match = true;
                        }
                    }
                    else
                    {
                        if (match_pattern(input_line, pattern))
                        {
                            if (multiple_files)
                            {
                                std::cout << filepath << ":" << input_line << std::endl;
                            }
                            else
                            {
                                std::cout << input_line << std::endl;
                            }
                            any_match = true;
                        }
                    }
                }
            }
        }
        return any_match ? 0 : 1;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}