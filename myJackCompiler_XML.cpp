#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <io.h>

using namespace std;

class JackTokenizer
{
private:
    ifstream ist;
    string fileBuffer;

    string removeLineBlank(string &);
    string removeComments(string &);

public:
    JackTokenizer(string &);

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
        }
        else
        {
            s += file[i];
        }
    }
    return s;
}

JackTokenizer::JackTokenizer(string &path)
{
    ist = ifstream(path.c_str());

    if (!ist)
        throw runtime_error("cannot open input file");

    while (!ist.eof())
    {
        string line;
        getline(ist, line);
        line = removeLineBlank(line);
        line += '\n';
        fileBuffer += line;
    }

    fileBuffer = removeComments(fileBuffer);
    cout << fileBuffer << endl;
}

class CompilationEngine
{
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

CompilationEngine::CompilationEngine(JackTokenizer &jt, string &s)
{
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
        string outputPath = filepath.substr(0, filepath.size() - 4) + ".xml";
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
    //cout << ss << endl;
    if (ss == ".jack")
        return true;
    else
        return false;
}

void getAllFiles(string &path, vector<string> &files)
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
        cout << files[i] << endl;
        JackAnalyzer analyzer(files[i]);
        analyzer.beginAnalyzing();
    }
}