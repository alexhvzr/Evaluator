//
// Created by Alex Hayden van Zuiden-Rylander on 2019-03-04.
//

// Public routine that performs the evaluation. Examines the postfix machine
// to see if a single result is left and if so, return it; otherwise prints
// error.
template<class NumericType>
void printVector(vector<NumericType> &v) {
    cout << "postFixStack = ";
    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
    }
    cout << endl;
}

template<class NumericType>
void printpostFixVarStack(vector<NumericType> &v) {
    cout << "postfixVarStack = ";

    for (int i = 0; i < v.size(); i++) {
        (char(v[i]) == ' ') ? cout << "?" : cout << char(v[i]) << " ";
    }
    cout << endl;
}

template<class NumericType>
void printOS(vector<TokenType> &v) {
    cout << "Op stack = ";
    for (int i = 0; i < v.size(); i++) {
        TokenType t = v[i];
        cout << t << " ";
    }
    cout << endl;
}

template<class NumericType>
NumericType Evaluator<NumericType>::getValue() {

    Tokenizer<NumericType> tok(str);
    Token<NumericType> lastToken;

    do {
        lastToken = tok.getToken();
        processToken(lastToken);
    } while (lastToken.getType() != EOL);

    if (postFixStack.empty()) {
        cerr << "Missing operand!" << endl;
        return 0;
    }

    NumericType theResult = postFixStack.back();
    postFixStack.pop_back();
    if (!postFixStack.empty())
        cerr << "Warning: missing operators!" << endl;

    return theResult;
}

// After token is read, use operator precedence parsing algorithm to process
// it; missing opening parentheses are detected here.
template<class NumericType>
void Evaluator<NumericType>::
processToken(const Token<NumericType> &lastToken) {
    TokenType topOp;
    TokenType lastType = lastToken.getType();

    switch (lastType) {
        case VALUE:
            postFixStack.push_back(lastToken.getValue());
            postFixVarStack.push_back(' ');
            return;

        case VAR_A:
            postFixStack.push_back(var_a);
            postFixVarStack.push_back('a');
            return;

        case VAR_B:
            postFixStack.push_back(var_b);
            postFixVarStack.push_back('b');
            return;

        case VAR_C:
            postFixStack.push_back(var_c);
            postFixVarStack.push_back('c');
            return;

        case CPAREN:
            while ((topOp = opStack.back()) != OPAREN &&
                   topOp != EOL) {
                arithmeticOp(topOp);
            }

            if (topOp == OPAREN)
                opStack.pop_back(); // get rid of opening parenthesis

            else
                cerr << "Missing open parenthesis" << endl;
            break;

        default: // general operator case
            while (PREC_TABLE[lastType].inputSymbol <= PREC_TABLE[topOp = opStack.back()].topOfStack)
                arithmeticOp(topOp);

            if (lastToken.getType() != EOL)
                opStack.push_back(lastType);

            break;
    }
}

template<class NumericType>
void Evaluator<NumericType>::unaryOp(TokenType topOp) {

    NumericType var = getTop();

    if (topOp == UN_MINUS) {
        postFixStack.push_back(var * -1);
    } else if (topOp == UN_PLUS) {
        (var < 0) ? postFixStack.push_back(var * -1) : postFixStack.push_back(var);
    } else if (topOp == NOT) {
        (var == 1) ? postFixStack.push_back(0) : postFixStack.push_back(1);
    } else if (topOp == BIT_COMP) {
        postFixStack.push_back(~var);
    }

    opStack.pop_back();

}

template<class NumericType>
void Evaluator<NumericType>::arithmeticOp(TokenType topOp) {
    if (topOp == UN_PLUS || topOp == UN_MINUS || topOp == BIT_COMP || topOp == NOT)
        unaryOp(topOp);
    else
        binaryOp(topOp);
}


// Process an operator by taking two items off the postfix stack, applying
// the operator, and pushing the result.
// Print error if missing closing parenthesis or division by 0.
template<class NumericType>
void Evaluator<NumericType>::binaryOp(TokenType topOp) {
    if (topOp == OPAREN) {
        cerr << "Unbalanced parentheses" << endl;
        opStack.pop_back();
        return;
    }


    NumericType rhs = getTop(); // 30
    NumericType lhs = getTop(); // 0

    getVariable(); // a
    char var = getVariable();  // /0

    // the original operators
    if (topOp == PLUS)
        postFixStack.push_back(lhs + rhs);
    else if (topOp == MINUS)
        postFixStack.push_back(lhs - rhs);
    else if (topOp == MULT)
        postFixStack.push_back(lhs * rhs);
    else if (topOp == DIV)
        if (rhs != 0)
            postFixStack.push_back(lhs / rhs);
        else {
            cerr << "Division by zero" << endl;
            postFixStack.push_back(lhs);
        }

        // C++ operators
    else if (topOp == MODULUS)
        postFixStack.push_back(lhs % rhs);
    else if (topOp == SHIFT_L)
        postFixStack.push_back(lhs << rhs);
    else if (topOp == SHIFT_R)
        postFixStack.push_back(lhs >> rhs);
    else if (topOp == LT)
        postFixStack.push_back(lhs < rhs);
    else if (topOp == LE)
        postFixStack.push_back(lhs <= rhs);
    else if (topOp == GT)
        postFixStack.push_back(lhs > rhs);
    else if (topOp == GE)
        postFixStack.push_back(lhs >= rhs);
    else if (topOp == EQUAL)
        postFixStack.push_back(lhs == rhs);
    else if (topOp == NOTEQUAL)
        postFixStack.push_back(lhs != rhs);
    else if (topOp == BIT_AND)
        postFixStack.push_back(lhs & rhs);
    else if (topOp == BIT_EOR)
        postFixStack.push_back(lhs ^ rhs);
    else if (topOp == BIT_IOR)
        postFixStack.push_back(lhs | rhs);
    else if (topOp == LOG_AND)
        postFixStack.push_back(lhs && rhs);
    else if (topOp == LOG_OR)
        postFixStack.push_back(lhs || rhs);
    else if (topOp == ASSIGN) {

        switch (var) {
            case 'a':
                var_a = rhs;
                postFixStack.push_back(var_a);
                postFixVarStack.push_back('a');
                break;
            case 'b':
                var_b = rhs;
                postFixStack.push_back(var_b);
                postFixVarStack.push_back('b');
                break;
            case 'c':
                var_c = rhs;
                postFixStack.push_back(var_c);
                postFixVarStack.push_back('c');
                break;
        }
    }
    postFixVarStack.push_back(' ');
    opStack.pop_back();
}

// top and pop the postfix machine stack; return the result.
// If the stack is empty, print an error message.
template<class NumericType>
NumericType Evaluator<NumericType>::getTop() {
    if (postFixStack.empty()) {
        cerr << "postFixStack is empty." << endl;
        return 0;
    }

    NumericType tmp = postFixStack.back();
    postFixStack.pop_back();

    return tmp;
}

template<class NumericType>
char Evaluator<NumericType>::getVariable() {
    if (postFixVarStack.empty()) {
        cerr << "Empty Variable Stack" << endl;
        return 0;
    }
    char tmp = postFixVarStack.back();
    postFixVarStack.pop_back();

    return tmp;
}
