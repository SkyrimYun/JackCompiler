#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <io.h>

using namespace std;

#define KEYWORD 0
#define SYMBOL 1
#define IDENTIFIER 2
#define INT_CONST 3
#define STRING_CONST 4
#define CLASS 5
#define METHOD 6
#define FUNCTION 7
#define CONSTRUCTOR 8
#define INT 9
#define BOOLEAN 10
#define CHAR 11
#define VOID 12
#define VAR 13
#define STATIC 14
#define FIELD 15
#define LET 16
#define DO 17
#define IF 18
#define ELSE 19
#define WHILE 20
#define RETURN 21
#define TRUE 22
#define FALSE 23
#define NULL 24
#define THIS 25
#define INVALID 26

class JackTokenizer
{
private:
    struct Token
    {
        string val;
        int type;

        Token(){};
        void set(string &s, int t)
        {
            val = s;
            type = t;
        }
        void reset()
        {
            val = "";
            type = -1;
        }
    };

    string fileBuffer;
    Token curToken;
    int index = 0;
    vector<string> keywords{
        "class",
        "method",
        "function",
        "constructor",
        "int",
        "boolean",
        "char",
        "void",
        "var",
        "static",
        "field",
        "let",
        "do",
        "if",
        "else",
        "while",
        "return",
        "true",
        "false",
        "null",
        "this",
    };
    vector<char> symbolList{'{', '}', '(', ')', '[', ']', '.', ',', '.', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '_', '~'};

    string removeLineBlank(string &);
    string removeComments(string &);
    bool isSymbol(char);
    bool isKeyword(string &);
    char getNextCharacter();
    void unget();

public:
    JackTokenizer(string &);
    JackTokenizer(){};

    //are there more tokens in the input
    bool hasMoreTokens();

    //gets the next token from the input, and makes it the current token.
    //this method should only be called if hasMoreTokens is true
    void advance();

    //return the type of the current token
    int tokenType();

    //returns the keyword which is the current token, as a constant
    //this method should be called only if tokenType is KEYWORD
    int keyWord();

    //returns the character which is the current token
    //should only be called if tokenType is SYMBOL
    char symbol();

    //returns the identifier which is the current token
    //should be called only if tokenType is IDENTIFIER
    string identifier();

    //returns the inter value of the current token.
    //should be called only if tokenType is INT_CONST.
    int intVal();

    //returns the string value of the current token, without the two enclosing double quotes.
    //should be called only if tokenType is STRING_CONST
    string stringVal();

    string tokenVal()
    {
        return curToken.val;
    }
};

string JackTokenizer::removeLineBlank(string &line)
{
    string re_line;
    stringstream lst(line);
    while (!lst.eof())
    {
        string s;
        lst >> s;
        re_line += (s + " ");
    }
    return re_line;
}

string JackTokenizer::removeComments(string &file)
{
    string s;
    for (int i = 0; i < file.size(); i++)
    {
        if (file[i] == '/' && file[i + 1] == '/')
        {
            while (file[i] != '\n')
            {
                i++;
            }
        }
        else if (file[i] == '/' && file[i + 1] == '*')
        {
            while (!(file[i] == '/' && file[i - 1] == '*'))
            {
                i++;
            }
            i += 2; //jump over '\n'
        }
        else
        {
            s += file[i];
        }
    }
    return s;
}

char JackTokenizer::getNextCharacter()
{
    char c = fileBuffer[index];
    index++;
    return c;
}

void JackTokenizer::unget()
{
    index--;
}

bool JackTokenizer::isSymbol(char c)
{
    for (char s : symbolList)
    {
        if (c == s)
            return true;
    }
    return false;
}

bool JackTokenizer::isKeyword(string &s)
{
    for (string ss : keywords)
    {
        if (s == ss)
            return true;
    }
    return false;
}

JackTokenizer::JackTokenizer(string &path)
{
    ifstream ist = ifstream(path.c_str());

    if (!ist)
        throw runtime_error("cannot open input file");

    while (!ist.eof())
    {
        string line;
        getline(ist, line);
        line = removeLineBlank(line);
        if (line[0] != ' ')
        {
            line += '\n';
            fileBuffer += line;
        }
    }

    fileBuffer = removeComments(fileBuffer);
    //cout << fileBuffer.size() << endl;
    //cout << fileBuffer << endl;
}

bool JackTokenizer::hasMoreTokens()
{
    return index < fileBuffer.size();
}

void JackTokenizer::advance()
{
    if (hasMoreTokens())
    {
        char c = getNextCharacter();
        string curValue;

        //handle string constant
        if (c == '"')
        {
            curValue += c;
            do
            {
                c = getNextCharacter();
                curValue += c;
            } while (c != '"');
            curToken.set(curValue, STRING_CONST);
        }
        //handle keyword or indentifier
        else if (isalpha(c))
        {
            while (c != ' ' && (!isSymbol(c) || c == '_'))
            {
                curValue += c;
                c = getNextCharacter();
            }
            if (isKeyword(curValue))
                curToken.set(curValue, KEYWORD);
            else
                curToken.set(curValue, IDENTIFIER);

            unget();
        }
        //handle integer constant
        else if (isdigit(c))
        {
            while (isdigit(c))
            {
                curValue += c;
                c = getNextCharacter();
            }
            curToken.set(curValue, INT_CONST);

            unget();
        }
        //handle symbol
        else if (isSymbol(c))
        {
            curValue = c;
            curToken.set(curValue, SYMBOL);
        }
        else if (isspace(c))
        {
            if (index == fileBuffer.size())
                curToken.set(curValue, NULL);
            if (hasMoreTokens())
                advance();
        }
        else
        {
            throw runtime_error("invalid input Token!");
        }
    }
}

int JackTokenizer::tokenType()
{
    return curToken.type;
}

int JackTokenizer::keyWord()
{
    if (curToken.type != KEYWORD)
        throw runtime_error("current token is not a keyword!");
    for (int i = 0; i < keywords.size(); i++)
    {
        if (curToken.val == keywords[i])
            return i + 5;
    }
}

char JackTokenizer::symbol()
{
    if (curToken.type != SYMBOL)
        throw runtime_error("current token is not a symbol!");
    return curToken.val[0];
}

string JackTokenizer::identifier()
{
    if (curToken.type != IDENTIFIER)
        throw runtime_error("current token is not a identifier!");
    return curToken.val;
}

int JackTokenizer::intVal()
{
    if (curToken.type != INT_CONST)
        throw runtime_error("current token is not a integer constant!");
    stringstream sToInt(curToken.val);
    int ret;
    sToInt >> ret;
    return ret;
}

string JackTokenizer::stringVal()
{
    if (curToken.type != STRING_CONST)
        throw runtime_error("current token is not a string constant!");
    return curToken.val;
}

class CompilationEngine
{
private:
    JackTokenizer tokenizer;
    ofstream ost;

    void writeLine(string, int);
    void writeXML(int);

public:
    CompilationEngine(JackTokenizer &, string &);

    //compiles a complete class
    void CompileClass();

    //comppiles a static variable declaration, or a field declaration
    void CompileClassVarDec();

    //compiles a complete method, function, or constructor
    void CompileSubroutineDec();

    //compiles a (possibly empty) parameter list. does not handle the enclosing "()"
    void CompileParameterList();

    //compiles a subroutine's body
    void CompileSubroutineBody();

    //compiles a var declaration
    void CompileVarDec();

    //compiles a sequence of statements. does not handle the enclosing "{}"
    void compileStatements();

    //compiles a let statement
    void CompiileLet();

    //compiles an if statement, possibly with a trailing else caluse
    void CompileIf();

    //compiles a while statement
    void CompiileWhile();

    //compiles a do statement
    void CompiileDo();

    //compiles a return statement
    void CompiileReturn();

    //compiles a expression statement
    void CompiileExpression();

    /*compiles a term. 
    if the current token is an identifier, the routine must distinguish between a variable, an array entry, or a subroutine call.
    a single look-ahead token, which may be one of '[', '(', or '.', suffices to distinguish between the possibilities.
    any other token is not part of this term and should not be advanced over */
    void CompileTerm();

    //compiles a (possibly empty) comma-separated list of expressions
    void CompileExpressionList();
};

void CompilationEngine::writeLine(string s, int n)
{
    for (int i = 0; i < n; i++)
    {
        s = '\t' + s;
    }
    ost << s << endl;
}

void CompilationEngine::writeXML(int n)
{
    switch (tokenizer.tokenType())
    {
    case KEYWORD:
    {
        writeLine("<keyword> " + tokenizer.tokenVal() + " </keyword>", n);
        break;
    }
    case IDENTIFIER:
    {
        writeLine("<identifier> " + tokenizer.identifier() + " </identifier>", n);
        break;
    }
    case SYMBOL:
    {
        if (tokenizer.symbol() == '"')
        {
            writeLine("&quot;", n);
        }
        else if (tokenizer.symbol() == '<')
        {
            writeLine("&lt;", n);
        }
        else if (tokenizer.symbol() == '>')
        {
            writeLine("&gt;", n);
        }
        else if (tokenizer.symbol() == '&')
        {
            writeLine("&amp;", n);
        }
        else
        {
            writeLine("<symbol> " + tokenizer.tokenVal() + " </symbol>", n);
        }
        break;
    }
    case INT_CONST:
    {
        writeLine("<integerConstant> " + tokenizer.tokenVal() + " </integerConstant>", n);
        break;
    }
    case STRING_CONST:
    {
        writeLine("<stringConstant> " + tokenizer.stringVal() + " </stringConstant>", n);
        break;
    }
    }
}

CompilationEngine::CompilationEngine(JackTokenizer &jt, string &s)
{
    tokenizer = jt;
    cout << s << endl;
    ost = ofstream(s);
    CompileClass();
}

void CompilationEngine::CompileClass()
{
    writeLine("<class>", 0);
    while (tokenizer.hasMoreTokens())
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == KEYWORD)
        {
            int keyword = tokenizer.keyWord();
            if (keyword == FUNCTION || keyword == CONSTRUCTOR || keyword == METHOD)
            {
                CompileSubroutineDec();
            }
            else if (keyword == STATIC || keyword == FIELD)
            {
                CompileClassVarDec();
            }
            else
            {
                writeXML(1);
            }
        }
        else
        {
            writeXML(1);
            cout << tokenizer.tokenVal() << endl;
        }
    }
    writeLine("</class>", 0);
}

void CompilationEngine::CompileClassVarDec()
{
    writeLine("<classVarDec>", 1);
    writeXML(2);
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ';'))
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == KEYWORD || tokenizer.tokenType() == SYMBOL || tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML(2);
        }
    }
    writeLine("</classVarDec>", 1);
}

void CompilationEngine::CompileSubroutineDec()
{
    writeLine("<subroutineDec>", 1);
    writeXML(2);
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '('))
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == KEYWORD || tokenizer.tokenType() == SYMBOL || tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML(2);
        }
    }

    if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '(')
    {
        CompileParameterList();
    }

    tokenizer.advance();

    if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '{')
    {
        CompileSubroutineBody();
    }
    writeLine("</subroutineDec>", 1);
}

void CompilationEngine::CompileParameterList()
{
    writeLine("<parameterList>", 2);
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ')'))
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == KEYWORD || tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML(3);
        }

        if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() != ')')
        {
            writeXML(3);
        }
    }
    writeLine("</parameterList>", 2);
    writeXML(2);
}

void CompilationEngine::CompileSubroutineBody()
{
    writeLine("<subroutineBody>", 2);
    writeXML(3);
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '}'))
    {
        tokenizer.advance();

        if (tokenizer.tokenType() == KEYWORD)
        {
            if (tokenizer.keyWord() == VAR)
            {
                CompileVarDec();
            }
            else
            {
                //compileStatements();
            }
        }
    }
    writeXML(3);
    writeLine("</subroutineBody>", 2);
}

void CompilationEngine::CompileVarDec()
{
    writeLine("<varDec>", 3);
    writeXML(4);
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ';'))
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == KEYWORD || tokenizer.tokenType() == SYMBOL || tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML(4);
        }
    }
    writeLine("</varDec>", 3);
}

class JackAnalyzer
{
private:
    string filepath;

public:
    JackAnalyzer(string &path) : filepath(path){};

    void beginAnalyzing()
    {
        JackTokenizer tokenizer(filepath);
        string outputPath = filepath.substr(0, filepath.size() - 4) + "xml";
        CompilationEngine engine(tokenizer, outputPath);
    }
};

bool checkjack(string &s)
{
    string ss;

    if (s.size() < 6)
        return false;

    for (int i = s.size() - 5; i < s.size(); i++)
    {
        ss.push_back(s[i]);
    }
    if (ss == ".jack")
        return true;
    else
        return false;
}

void getAllFiles(string path, vector<string> &files)
{

    if (checkjack(path))
    {
        files.push_back(path);
    }
    else
    {
        long hFile = 0;

        struct _finddata_t fileinfo;
        string p;

        if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
        {
            do
            {
                string check;
                check.append(fileinfo.name);
                if (checkjack(check))
                {
                    files.push_back(p.assign(path).append("/").append(fileinfo.name));
                }
            } while (_findnext(hFile, &fileinfo) == 0);
            _findclose(hFile);
        }
    }
}

int main()
{
    vector<string> files;

    string inputPath = "C:/Users/skyri/projects/JackCompiler";

    getAllFiles(inputPath, files);

    if (files.size() == 0)
        throw runtime_error("no vaild input file!");

    for (int i = 0; i < files.size(); i++)
    {
        JackAnalyzer analyzer(files[i]);
        analyzer.beginAnalyzing();
    }
}