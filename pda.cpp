#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>

/**
  PushdownAutomaton class for parsing and executing PDA operations
  
  Input file must follow this model:
  [states]
  state_1 value
  state_2 value 
  ( value refers to S for start_state, F for final_state or anything else for intermediary states usually 0) 
  there can only be one start state, but if there are more than one state signaled with S, the last one will be considered the start state; 
  there can only be one final state, but if there are more than one state signaled with F, the last one will be considered the final state;
  
  [sigma]
  letter_1
  letter_2

  [rules]
  state_1 input_value pop_value push_value state_2 (first rule must have the start state on the first state position)
  state_3 input_value pop_value push_value state_4

 */
class PushdownAutomaton {
private:
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> automaton;
    
    // Helper function to trim whitespace from beginning and end of string
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    // Helper function to split a string into words
    std::vector<std::string> splitString(const std::string& str) {
        std::vector<std::string> tokens;
        std::istringstream iss(str);
        std::string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }

public:
    // Function for loading automaton, checks for commentaries or empty lines
    bool loadAutomaton(const std::string& filename) {
        std::ifstream inputFile(filename);
        
        if (!inputFile.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        std::string currentSection;
        
        while (std::getline(inputFile, line)) {
            line = trim(line);
            
            // Skip empty lines or comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check if this is a section header
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                currentSection = line.substr(1, line.length() - 2);
                automaton[currentSection] = std::vector<std::vector<std::string>>();
            } 
            else if (currentSection == "sigma") {
                std::vector<std::string> symbol;
                symbol.push_back(trim(line));
                
                // Check if symbol already exists
                bool exists = false;
                for (const auto& existingSymbol : automaton[currentSection]) {
                    if (existingSymbol == symbol) {
                        exists = true;
                        break;
                    }
                }
                
                if (!exists) {
                    automaton[currentSection].push_back(symbol);
                }
            }
            else if (!currentSection.empty()) {
                std::vector<std::string> parts = splitString(line);
                
                // Check if rule/state already exists
                bool exists = false;
                for (const auto& existingParts : automaton[currentSection]) {
                    if (existingParts == parts) {
                        exists = true;
                        break;
                    }
                }
                
                if (!exists) {
                    automaton[currentSection].push_back(parts);
                }
            }
        }
        
        inputFile.close();
        return true;
    }
    
    // Function for checking valid automaton
    bool isValidAutomaton() const {
        std::vector<std::string> requiredSections = {"states", "sigma", "rules"};
        
        for (const auto& section : requiredSections) {
            if (automaton.find(section) == automaton.end()) {
                return false;
            }
        }
        
        return true;
    }
    
    bool isValidRule(const std::vector<std::string>& rule) const {
        // Get all state names
        std::vector<std::string> states;
        for (const auto& state : automaton.at("states")) {
            states.push_back(state[0]);
        }
        
        // Extract sigma values
        std::vector<std::string> sigmaValues;
        for (const auto& symbol : automaton.at("sigma")) {
            sigmaValues.push_back(symbol[0]);
        }
        
        // Rule should have 5 components
        if (rule.size() != 5) {
            return false;
        }
        
        // First and fifth components should be valid states
        if (std::find(states.begin(), states.end(), rule[0]) == states.end() ||
            std::find(states.begin(), states.end(), rule[4]) == states.end()) {
            return false;
        }
        
        // Second, third, and fourth components should be valid sigma values
        if (std::find(sigmaValues.begin(), sigmaValues.end(), rule[1]) == sigmaValues.end() ||
            std::find(sigmaValues.begin(), sigmaValues.end(), rule[2]) == sigmaValues.end() ||
            std::find(sigmaValues.begin(), sigmaValues.end(), rule[3]) == sigmaValues.end()) {
            return false;
        }
        
        return true;
    }
    
    // Function for checking all valid rules
    bool allValidRules() const {
        const auto& rules = automaton.at("rules");
        
        for (const auto& rule : rules) {
            if (!isValidRule(rule)) {
                return false;
            }
        }
        
        return true;
    }
    
    // Function for checking valid first rule, must have the start_state on the first state position
    bool isValidFirstRule() const {
        const auto& rules = automaton.at("rules");
        if (rules.empty()) {
            return false;
        }
        
        std::string startState;
        for (const auto& state : automaton.at("states")) {
            if (state.size() > 1 && state[1] == "S") {
                startState = state[0];
            }
        }
        
        return rules[0][0] == startState;
    }
    
    // Function for checking valid input
    bool isValidInput(const std::vector<std::string>& input) const {
        // An empty input is valid
        if (input.empty()) {
            return true;
        }
        
        std::vector<std::string> sigmaValues;
        for (const auto& symbol : automaton.at("sigma")) {
            sigmaValues.push_back(symbol[0]);
        }
        
        for (const auto& value : input) {
            if (std::find(sigmaValues.begin(), sigmaValues.end(), value) == sigmaValues.end()) {
                return false;
            }
        }
        
        return true;
    }
    
    // Getter for a section of the automaton
    std::vector<std::vector<std::string>> getSection(const std::string& section) const {
        if (automaton.find(section) != automaton.end()) {
            return automaton.at(section);
        }
        return std::vector<std::vector<std::string>>();
    }
    
    // Function to apply a rule given the current state, stack, and rule
    bool applyRule(const std::vector<std::string>& rule, std::string& currState, std::vector<std::string>& stack) {
        // For pop value "e", we don't need to pop anything
        // For any other pop value, we need to check if it matches the top of the stack
        if (rule[2] != "e") {
            if (stack.empty() || stack.back() != rule[2]) {
                return false;  // Can't apply this rule
            }
            // Pop the value
            stack.pop_back();
        }
        
        // Push value (if not "e")
        if (rule[3] != "e") {
            stack.push_back(rule[3]);
        }
        
        // Update current state
        currState = rule[4];
        
        // Print current state and stack
        std::cout << currState << " -> ";
        std::cout << "[";
        for (size_t j = 0; j < stack.size(); ++j) {
            std::cout << stack[j];
            if (j < stack.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
        
        return true;
    }
    
    // Helper to apply epsilon transitions
    bool applyEpsilonTransitions(std::string& currState, std::vector<std::string>& stack) {
        bool epsilonTransitionApplied = false;
        
        for (const auto& rule : automaton.at("rules")) {
            if (currState == rule[0] && rule[1] == "e") {
                if ((rule[2] == "e") || (!stack.empty() && stack.back() == rule[2])) {
                    bool applied = applyRule(rule, currState, stack);
                    if (applied) {
                        epsilonTransitionApplied = true;
                        // Recursively try more epsilon transitions
                        if (applyEpsilonTransitions(currState, stack)) {
                            return true;
                        }
                        break;
                    }
                }
            }
        }
        
        return epsilonTransitionApplied;
    }
    
    // PDA execution function
    void startPDA() {
        // Check valid automaton and rules
        if (!isValidAutomaton() || !allValidRules()) {
            std::cout << "Nu e automat valid, verificati fisierul de input si incercati din nou!" << std::endl;
            return;
        }
        
        // Get input string
        std::string inputLine;
        std::cout << "Enter input string (space-separated symbols): ";
        std::getline(std::cin, inputLine);
        
        std::vector<std::string> inputString = splitString(inputLine);
        std::vector<std::string> stack;
        
        // Check valid input
        if (isValidInput(inputString)) {
            std::string currState;
            std::vector<std::string> finalStates;
            
            // Searching for start and final states
            for (const auto& state : automaton.at("states")) {
                if (state.size() > 1) {
                    if (state[1] == "S") {
                        currState = state[0];
                    }
                    else if (state[1] == "F") {
                        finalStates.push_back(state[0]);
                    }
                }
            }
            
            // Check valid first rule
            if (isValidFirstRule()) {
                std::cout << currState << " -> ";
                
                // Apply initial epsilon transitions before processing input
                applyEpsilonTransitions(currState, stack);
                
                // Iterating through input
                for (size_t i = 0; i < inputString.size(); i++) {
                    const std::string& value = inputString[i];
                    bool processedInput = false;
                    
                    // Process the actual input symbol
                    for (const auto& rule : automaton.at("rules")) {
                        if (currState == rule[0] && rule[1] == value) {
                            bool applied = applyRule(rule, currState, stack);
                            if (applied) {
                                processedInput = true;
                                // Apply epsilon transitions after this input
                                applyEpsilonTransitions(currState, stack);
                                break;
                            }
                        }
                    }
                    
                    if (!processedInput) {
                        std::cout << "No valid transition for input symbol " << value << " from state " << currState << std::endl;
                        std::cout << "Input rejected!" << std::endl;
                        return;
                    }
                }
                
                // After processing all input, apply remaining epsilon transitions
                applyEpsilonTransitions(currState, stack);
                
                // Final stack state
                std::cout << "Final stack: [";
                for (size_t i = 0; i < stack.size(); ++i) {
                    std::cout << stack[i];
                    if (i < stack.size() - 1) {
                        std::cout << ", ";
                    }
                }
                std::cout << "]" << std::endl;
                
                // Check if current state is a final state
                if (std::find(finalStates.begin(), finalStates.end(), currState) != finalStates.end()) {
                    std::cout << "Input accepted - reached final state!" << std::endl;
                } else {
                    std::cout << "Input rejected - not in a final state!" << std::endl;
                }
            }
            else {
                std::cout << "Nu e automat valid, verificati fisierul de input si incercati din nou!" << std::endl;
            }
        }
        else {
            std::cout << "Input invalid!" << std::endl;
        }
    }
};

int main() {
    PushdownAutomaton pda;
    std::string filename;
    
    std::cout << "Enter PDA definition filename: ";
    std::cin >> filename;
    std::cin.ignore(); // Clear the newline character
    
    if (pda.loadAutomaton(filename)) {
        // Verify automaton before running
        if (pda.isValidAutomaton() && pda.allValidRules() && pda.isValidFirstRule()) {
            pda.startPDA();
        } else {
            std::cout << "Nu e automat valid, verificati fisierul de input si incercati din nou!" << std::endl;
        }
    } else {
        std::cout << "Failed to load automaton from file: " << filename << std::endl;
    }
    
    return 0;
}