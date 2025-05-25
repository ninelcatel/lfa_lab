''' Format the input file should follow:
Every section is marked by [start] and [end], everything in between is parsed
Each section MUST have a defined keyword 
Keyword is in {alphabet,states,delta,firstState,acceptedStates}
Exameple of a section:

[start]
keyword:alphabet
0
1
[end]

After each keyword we have the following format:
    i) alphabet: any 1 character long symbol 
    ii) states: any length of a character
    iii) firstState: any length of a character, MUST be in States and is UNIQUE
    iv) acceptedStates: any length of characters, MUST be in States and could be multiple
    v) delta: (state0,symbol)=state1 , state is in states and symbol is in alphabet
        if NFA and (state0,symbol)=state1,state2,state3,...,state_n
each definition it must be followed by a newline

[eof] is marked as EndOfFile and anything placed after it wont be relevant to the automaton
'#' is used for comments
'''


DFA = {}
NFA = {}
delta = {}

def epsilon_closure(states):
    closure = set(states)
    stack = list(states)
    while stack:
        state = stack.pop()
        if (state, 'ε') in delta:
            for next_state in delta[(state, 'ε')]:
                if next_state not in closure:
                    closure.add(next_state)
                    stack.append(next_state)
    return closure

def createAutomaton(toParse):
    lines = toParse.readlines()
    i = 0
    isNFA = checkNFA(lines)
    # Temporarily store delta transitions in a list to pass to deltaFunc
    delta_transitions = [] 
    while i < len(lines):
        line = lines[i].strip()
        if line == '[eof]':
            break
        if line.startswith('keyword'):
            keyword = line.split(':')[1].strip()
            # If the keyword is 'delta', we collect the raw transitions
            # instead of adding them to the automaton dictionary.
            if keyword == 'delta':
                i, transitions = collect_delta_transitions(i + 1, lines)
                delta_transitions.extend(transitions)
            else:
                i = append(i + 1, lines, keyword, isNFA)
        else:
            i += 1
    deltaFunc(delta_transitions) 
    return isNFA

def collect_delta_transitions(i, lines):
    transitions = []
    while i < len(lines):
        line = lines[i].strip()
        if line == '[end]':
            break
        transitions.append(line)
        i += 1
    return i + 1, transitions

def append(i, lines, keyword, isNFA):
    automaton = NFA if isNFA else DFA
    while i < len(lines):
        line = lines[i].strip()
        if line == '[end]':
            break
        if keyword in automaton:
            automaton[keyword].append(line)
        else:
            automaton[keyword] = [line]
        i += 1
    return i + 1

def deltaFunc(transitions_list):
    global delta
    delta = {} # Clear delta for a fresh start

    for entry in transitions_list:
        parts = entry.split('=')
        if len(parts) < 2: # Skip incorrect delta entries
            continue
        left = parts[0].strip()
        right = parts[1].strip().split(',')

        state_input_raw = left[1:-1] # Remove parentheses
        if ',' not in state_input_raw: # Skip incorrect definition
            continue 
        
        state_input = state_input_raw.split(',')
        state = state_input[0].strip()
        symbol = state_input[1].strip()
        key = (state, symbol)

        if key not in delta:
            delta[key] = set()
        delta[key].update(right)
        
def checkNFA(lines):
    canSearch = False
    for line in lines:
        line = line.strip()
        if canSearch:
            if '=' in line:
                left = line.split('=')[0].strip()
                # Check for epsilon transitions or multiple next states
                if '(ε)' in left or len(line.split('=')[1].split(',')) > 1:
                    return True
            if line == '[end]':
                return False
        elif line.startswith('keyword') and line.split(':')[1].strip() == 'delta':
            canSearch = True
    return False

# Run Parser 
fileName=input()
if not '.txt' in fileName:
    fileName += '.txt'
toParse = open(fileName, "r")
isNFA = createAutomaton(toParse)
toParse.close()

# Debug Print 
print("--- Automaton ---")
automaton = NFA if isNFA else DFA
for key in automaton:
    print(f"{key}: {automaton[key]}")

print("--- Delta ---")
for key in delta:
    print(f"{key}: {delta[key]}")

print("--- Simulation ---")
# Simulation 
input_str = input("Enter input string: ")

if isNFA:
    if 'firstState' not in NFA or not NFA['firstState']:
        print("Error: NFA 'firstState' not defined.")
        exit()
    
    currentStates = epsilon_closure([NFA['firstState'][0]])

    for symbol in input_str:
        nextStates = set()
        for state in currentStates:
            if (state, symbol) in delta:
                nextStates.update(delta[(state, symbol)])
        currentStates = epsilon_closure(nextStates)

    if 'acceptedStates' not in NFA:
        print("Error: NFA 'acceptedStates' not defined.")
        print(False)
    else:
        print(any(state in NFA['acceptedStates'] for state in currentStates))
else: # DFA
    if 'firstState' not in DFA or not DFA['firstState']:
        print("Error: DFA 'firstState' not defined.")
        exit()

    currentState = DFA['firstState'][0]

    for symbol in input_str:
        key = (currentState, symbol)
        if key in delta:
            currentState = list(delta[key])[0]
        else:
            currentState = None
            break
            
    if 'acceptedStates' not in DFA:
        print("Error: DFA 'acceptedStates' not defined.")
        print(False)
    else:
        print(currentState in DFA['acceptedStates'])