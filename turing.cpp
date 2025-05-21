#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

/*
  TuringMachine class for simulating a Turing Machine
 
  The input file must follow this format:
  [states]
  state_1 value
  state_2 value
  (#! value refers to S for start_state, F for final_state, or anything else for intermediate states;
  there can only be one start state, but if there are multiple states marked with S, the last one will be considered the start state;
  there can only be one final state, but if there are multiple states marked with F, the last one will be considered the final state;
  if a state is both a start and final state, it must be set separately, not on the same line)
  ...
 
  [sigma]
  letter_1
  letter_2
  ...
 
  [rules]
  state_1 letter state_2 letter direction (the first rule must have the start state as the first state)
  state_3 letter state_4 letter direction (direction MUST be R or L)
  ...
 */
class TuringMachine
{
private:
    std::unordered_map<std::string, std::string> states_;
    std::unordered_set<char> sigma_;
    std::vector<std::vector<std::string>> rules_;
    std::string start_state_;
    std::string final_state_;

    // Helper function to trim leading and trailing whitespace
    std::string trim(const std::string &s)
    {
        size_t first = s.find_first_not_of(" \t\n\r");
        if (std::string::npos == first)
        {
            return s;
        }
        size_t last = s.find_last_not_of(" \t\n\r");
        return s.substr(first, (last - first + 1));
    }

    // Helper function to split a string into tokens
    std::vector<std::string> split(const std::string &s)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string token;
        while (ss >> token)
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    bool isValidState(const std::string &state_name) const
    {
        return states_.find(state_name) != states_.end();
    }

    bool isValidSymbol(char symbol) const
    {
        return sigma_.find(symbol) != sigma_.end();
    }

    bool isValidDirection(const std::string &dir) const
    {
        return dir == "R" || dir == "L";
    }

public:
    bool loadFromFile(const std::string &filename)
    {
        std::ifstream inputFile(filename);
        if (!inputFile.is_open())
        {
            std::cerr << "Error opening file: " << filename << std::endl;
            return false;
        }

        std::string line;
        std::string current_section;

        while (std::getline(inputFile, line))
        {
            line = trim(line);
            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            if (line.rfind('[', 0) == 0 && line.rfind(']', line.length() - 1) == line.length() - 1)
            {
                current_section = trim(line.substr(1, line.length() - 2));
            }
            else if (current_section == "states")
            {
                std::vector<std::string> parts = split(line);
                if (parts.size() == 2)
                {
                    states_[parts[0]] = parts[1];
                    if (parts[1] == "S")
                    {
                        start_state_ = parts[0];
                    }
                    else if (parts[1] == "F")
                    {
                        final_state_ = parts[0];
                    }
                }
                else
                {
                    std::cerr << "Invalid state definition: " << line << std::endl;
                    return false;
                }
            }
            else if (current_section == "sigma")
            {
                if (line.length() == 1)
                {
                    sigma_.insert(line[0]);
                }
                else
                {
                    std::cerr << "Invalid sigma symbol: " << line << std::endl;
                    return false;
                }
            }
            else if (current_section == "rules")
            {
                std::vector<std::string> parts = split(line);
                if (parts.size() == 5)
                {
                    rules_.push_back(parts);
                }
                else
                {
                    std::cerr << "Invalid rule definition: " << line << std::endl;
                    return false;
                }
            }
        }

        inputFile.close();
        return true;
    }

    bool isValidAutomaton() const
    {
        if (states_.empty() || sigma_.empty() || rules_.empty())
        {
            std::cerr << "The automaton must have states, sigma, and rules defined." << std::endl;
            return false;
        }
        if (start_state_.empty())
        {
            std::cerr << "Start state is not defined or is invalid." << std::endl;
            return false;
        }
        if (final_state_.empty())
        {
            std::cerr << "Final state is not defined or is invalid." << std::endl;
            return false;
        }
        for (const auto &rule : rules_)
        {
            if (!isValidState(rule[0]) || !isValidState(rule[2]) ||
                !isValidSymbol(rule[1][0]) || !isValidSymbol(rule[3][0]) ||
                !isValidDirection(rule[4]))
            {
                std::cerr << "Invalid rule: " << rule[0] << " " << rule[1] << " "
                          << rule[2] << " " << rule[3] << " " << rule[4] << std::endl;
                return false;
            }
        }
        if (rules_.empty() || rules_[0][0] != start_state_)
        {
            std::cerr << "The first rule must start from the start state." << std::endl;
            return false;
        }
        return true;
    }

    bool isValidInput(const std::vector<std::string> &input_string) const
    {
        for (const std::string &symbol_str : input_string)
        {
            if (symbol_str.length() != 1 || sigma_.find(symbol_str[0]) == sigma_.end())
            {
                std::cerr << "Invalid input symbol: " << symbol_str << std::endl;
                return false;
            }
        }
        return true;
    }

    void run()
    {
        if (!isValidAutomaton())
        {
            std::cerr << "Invalid automaton. Please check the input file." << std::endl;
            return;
        }

        std::string input_line;
        std::cout << "Enter input string (symbols separated by spaces): ";
        std::getline(std::cin, input_line);
        std::vector<std::string> input_symbols = split(input_line);

        if (!isValidInput(input_symbols))
        {
            return;
        }

        std::vector<char> bandwidth;
        for (const std::string &symbol : input_symbols)
        {
            bandwidth.push_back(symbol[0]);
        }
        bandwidth.insert(bandwidth.end(), 100, '*'); // Fill with blank symbols

        int head_position = 0;
        std::string current_state = start_state_;

        while (current_state != final_state_)
        {
            if (head_position < 0 || head_position >= bandwidth.size())
            {
                std::cout << "The read/write head moved outside the tape." << std::endl;
                break;
            }

            bool rule_found = false;
            for (const auto &rule : rules_)
            {
                if (rule[0] == current_state && rule[1][0] == bandwidth[head_position])
                {
                    current_state = rule[2];
                    bandwidth[head_position] = rule[3][0];
                    if (rule[4] == "R")
                    {
                        head_position++;
                    }
                    else
                    {
                        head_position--;
                    }
                    rule_found = true;
                    break;
                }
            }

            if (!rule_found)
            {
                std::cout << "No rule found for state " << current_state
                          << " and symbol " << bandwidth[head_position] << std::endl;
                break;
            }
        }

        // Print resulting tape (excluding blank symbols at the end)
        int end = bandwidth.size() - 1;
        while (end >= 0 && bandwidth[end] == '*')
        {
            end--;
        }
        std::cout << "Result: ";
        for (int i = 0; i <= end; ++i)
        {
            std::cout << bandwidth[i];
        }
        std::cout << std::endl;
    }
};

int main()
{
    TuringMachine tm;
    if (tm.loadFromFile("turing.txt"))
    {
        tm.run();
    }
    return 0;
}
