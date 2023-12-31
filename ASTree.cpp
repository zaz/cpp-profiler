//
//  ASTree.cpp
//  Abstract Syntax Tree
//
//  Created by Jonathan Maletic
//  Copyright 2023 Kent State University. All rights reserved.
//  Spring 2023
//  srcML 1.0
//
//  Modified by: Zaz Brown
//

#include "ASTree.hpp"


// Copy constructor for srcML
//
srcML::srcML(const srcML& actual) {
    header = actual.header;
    if (actual.tree)
        tree = new AST(*(actual.tree));
    else
        tree = 0;
}

// Constant time swap for srcML
//
void srcML::swap(srcML& b) {
    std::string t_header = header;
    header = b.header;
    b.header = t_header;

    AST *temp = tree;
    tree = b.tree;
    b.tree = temp;
}

// Assignment for srcML
//
srcML& srcML::operator=(srcML rhs) {
    swap(rhs);
    return *this;
}

// Reads in and constructs a srcML object.
//
std::istream& operator>>(std::istream& in, srcML& src){
    char ch;
    if (!in.eof()) in >> ch;
    src.header = readUntil(in, '>');
    if (!in.eof()) in >> ch;
    if (src.tree) delete src.tree;
    src.tree = new AST(category, readUntil(in, '>'));
    src.tree->read(in);
    return in;
}

// Prints out a srcML object
//
std::ostream& operator<<(std::ostream& out, const srcML& src){
    if (src.tree) src.tree->print(out);
    return out;
}

//  Adds in the includes and profile object declarations
//
void srcML::mainHeader(const std::vector<std::string>& profileName,
                       const std::vector<std::filesystem::path>& filePath    ) {
    tree->mainHeader(profileName, filePath);
}

//  Adds in the includes and declares profile object declarations
//
void srcML::fileHeader(const std::string& profileName) {
    tree->fileHeader(profileName);
}

// Adds in the report to the main
//
void srcML::mainReport(const std::vector<std::string>& profileName) {
        tree->mainReport(profileName);
}

///  Inserts a profile.count() into each function body.
//
void srcML::functionCount(const std::string& profileName) {
    tree->functionCount(profileName);
}

// Inserts a profile.count() for each statement.
//
void srcML::lineCount(const std::string& profileName) {
    tree->lineCount(profileName);
}


//
// AST::
//

// Constructs a category, token, or whitespace node for the tree.
//
AST::AST(nodes t, const std::string& s) {
    nodeType = t;
    switch (nodeType) {
        case category:
            tag = s;
            break;
        case token:
            text = unEscape(s);
            break;
        case whitespace:
            text = s;
            break;
    }
}


// Destructor for AST
//
AST::~AST() {
    child.clear();
    // shared_ptr recursively deletes all children
}


// Copy Constructor for AST
// Recursively traverse actual and copy each node into this.
//
AST::AST(const AST& actual) {
    this->nodeType = actual.nodeType;
    this->tag = actual.tag;
    this->closeTag = actual.closeTag;
    this->text = actual.text;
    for (auto const& c : actual.child) {
        std::shared_ptr<AST> temp(new AST(*c));
        this->child.push_back(temp);
    }
}


// Constant time swap for AST
// Swap all the top level childern (pointers to AST)
//
void AST::swap(AST& rhs) {
    using std::swap;

    swap(nodeType, rhs.nodeType);
    swap(tag, rhs.tag);
    swap(closeTag, rhs.closeTag);
    swap(text, rhs.text);
    swap(child, rhs.child);
}

// Assignment for AST
//
AST& AST::operator=(AST rhs) {
    swap(rhs);
    return *this;
}


// REQUIRES: this->nodetype == category && tagName is valid srcML tag
// ENSURES: RETVAL == this->child[i] where this->child[i]->tag == tagName
//
// IMPORTANT for milestone 2 and 3
//
std::shared_ptr<AST> AST::getChild(std::string tagName) const {
    for (auto& c : child) {
        if (c->tag == tagName) {
            return c;
        }
    }
    return nullptr;
}


// REQUIRES: this->tag == "name"
// Returns the full name of a <name> node
//  There are two types of names in srcML.  A simple name (e.g., foo) and a
//   name with a scope (e.g., std::bar).  This returns the correct
//   one from <name> AST node.
//
// IMPORTANT for milestone 3
//
std::string AST::getName() const {
    if (this->tag != "name") {
        throw std::invalid_argument("AST::getName() called on non-name node");
    }
    std::string result;
    if (child.front()->tag != "name") {
        result = child.front()->text;   //A simple name (e.g., main)
    } else {                            //A complex name (e.g., stack::push).
        result = child.front()->child.front()->text;
        result += "::";
        result += child.back()->child.front()->text;
    }
    return result;
}

// finds the contents of the name tag inside this
// does not recurse
std::string AST::findName() const {
    return this->getChild("name")->getName();
}

std::shared_ptr<AST> AST::findTag(const std::string& tagName) const {
    auto pos = this->child.begin();
    for (auto& c : child) {
        if (c->tag == tagName) {
            return(c);
            //this->child.insert(pos, profiler);
        }
        ++pos;
    }
}

std::shared_ptr<AST> AST::findTagWithName(const std::string& tagName,
                                          const std::string& name) const {
    auto pos = this->child.begin();
    for (auto& c : child) {
        if (c->tag == tagName && c->findName() == name) {
            return(c);
            //this->child.insert(pos, profiler);
        }
        ++pos;
    }
}

//  Adds above the main, in the main file:
//  1. #include "profile.hpp"
//  2. All needed profile object declarations
//      example: profile foo_cpp("foo.cpp");
//
void AST::mainHeader(const std::vector<std::string>& profileName,
                     const std::vector<std::filesystem::path>& filePath) {

    // not technically correct srcML, but it produces the correct text
    auto profiler = std::make_shared<AST>(category, "profiler");
    auto include = std::make_shared<AST>(token, "#include \"profile.hpp\"\n");
    profiler->child.push_back(include);
    for (unsigned i = 0; i < profileName.size(); ++i) {
        auto profile = std::make_shared<AST>(token,
            "profile " + profileName[i] + "(\"" + filePath[i].string() + "\");\n" );
        profiler->child.push_back(profile);
    }

    auto pos = this->child.begin();
    for (auto& c : child) {
        if (c->tag == "function" && c->findName() == "main") {
            this->child.insert(pos, profiler);
            break;
        }
        ++pos;
    }
}


// Adds in each file (except main file):
// 1. #include "profile.hpp"
// 2. All needed external profile object declarations
//     example: extern profile thisfile_cpp;
// TODO: factor out for loop
//
void AST::fileHeader(const std::string& profileName) {

    // not technically correct srcML, but it produces the correct text
    auto profiler = std::make_shared<AST>(category, "profiler");
    auto include = std::make_shared<AST>(token, "#include \"profile.hpp\"\n");
    profiler->child.push_back(include);
    auto extrn = std::make_shared<AST>(token,
                                       "extern profile " + profileName + ";\n");
    profiler->child.push_back(extrn);

    auto pos = this->child.begin();
    for (auto& c : child) {
        if (c->tag == "function" && c->findName() == "main") {
            this->child.insert(pos, profiler);
            break;
        }
        ++pos;
    }
}


// Adds in the report to the main file
// Assumes only one return in the main
//
void AST::mainReport(const std::vector<std::string>& profileName) {
    //TODO: IMPLEMENT
    //Find the function with name of "main" in the file
    AST main = *findTagWithName("function", "main");
    // -Get the "block" node of this function
    // -Set an iterator to the child.end() of "block"
    // -Decrement it once (to skip the "}")
    // -Search backwards for a "return" tag
    // -Insert the report statements before the return
}


// Adds a node to the function block to count the times each function
//  is executed
//  Assumes no nested functions
//
void AST::functionCount(const std::string& profileName) {
    //TODO: IMPLEMENT
    // for each child in the "unit"
    //     if child is a function, constructor, destructor
    //        Find the function name (use AST::getName())
    //        Find <block>, then find <block_content>
    //        Insert the count as first child in <block_content>
    //        Example: main1_cpp.count(__LINE__, "main");
}


// Adds in a node to count the number of times each statement is executed
//   Do not count breaks, returns, throw, declarations, etc.
//   Assumes all construts (for, while, if) have an explicit block { }
//
void AST::lineCount(const std::string& profileName) {
    //TODO: IMPLEMENT
    // Recursively check for expr_stmt within all category nodes <block>
    // Very similar to AST::print
    // For each child:
    //   If it is a category node
    //       If it is a expr_stmt insert a count after it
    //          Example: foo_cpp.count(__LINE__);
    //       Else call lineCount if not a stop tag  See isStopTag()
    //   Else (token or whitespace) do nothing
}

// Returns TRUE if the tag (syntactic category) is not to be profiled
//
// IMPORTANT for milestone 3
//
bool isStopTag(std::string tag) {
    if (tag == "condition"             ) return true; //Remove for challenge
    if (tag == "type"                  ) return true;
    if (tag == "name"                  ) return true;
    if (tag == "return"                ) return true;
    if (tag == "break"                 ) return true;
    if (tag == "continue"              ) return true;
    if (tag == "parameter_list"        ) return true;
    if (tag == "decl_stmt"             ) return true;
    if (tag == "argument_list"         ) return true;
    if (tag == "init"                  ) return true;
    if (tag == "cpp:include"           ) return true;
    if (tag == "macro"                 ) return true;
    if (tag == "comment type=\"block\"") return true;
    if (tag == "comment type=\"line\"" ) return true;
    return false;
}


// Print an AST
// Preorder traversal that prints out leaf nodes only (tokens & whitesapce)
//
std::ostream& AST::print(std::ostream& out) const {
    for (std::list<std::shared_ptr<AST>>::const_iterator i = child.begin(); i != child.end(); ++i) {
        if ((*i)->nodeType != category)
            out << (*i)->text;   //Token or whitespace node, print it
        else
            (*i)->print(out);    //Category node, recursive call
    }
    return out;
}


// Read in and construct AST
// REQUIRES: '>' was previous charater read
//           && this == new AST(category, "TagName")
//
std::istream& AST::read(std::istream& in) {
    std::shared_ptr<AST> subtree;
    std::string temp, Lws, Rws;
    char ch;
    if (!in.eof()) in.get(ch);
    while (!in.eof()) {
        if (ch == '<') {                      //Found a tag
            temp = readUntil(in, '>');
            if (temp[0] == '/') {
                closeTag = temp;
                break;                        //Found close tag, stop recursion
            }
            subtree.reset(new AST(category, temp));                  //Create a new node
            subtree->read(in);                               //Read it in
            in.get(ch);
            child.push_back(subtree);                        //Add it to child
        } else {                                             //Found a token
            temp = std::string(1, ch) + readUntil(in, '<');  //Read it in.
            std::vector<std::string> tokenList = tokenize(temp);
            for (std::vector<std::string>::const_iterator i=tokenList.begin();
                 i != tokenList.end();
                 ++i) {
                if (isspace((*i)[0])) {
                    subtree.reset(new AST(whitespace, *i));
                } else {
                    subtree.reset(new AST(token, *i));
                }
                child.push_back(subtree);
            }
            ch = '<';
        }
    }
    return in;
}



// Utilities for AST::read()

// Reads until a key is encountered.  Does not include ch.
// REQUIRES: in.open()
// ENSURES: RetVal[i] != key for all i.
std::string readUntil(std::istream& in, char key) {
    std::string result;
    char ch;
    in.get(ch);
    while (!in.eof() && (ch != key)) {
        result += ch;
        in.get(ch);
    }
    return result;
}

// Converts escaped XML charaters back to charater form
// REQUIRES: s == "&lt;"
// ENSURES:  RetVal == "<"
std::string unEscape(std::string s) {
    std::size_t pos = 0;
    while ((pos = s.find("&gt;"))  != s.npos) { s.replace(pos, 4, ">"); }
    while ((pos = s.find("&lt;"))  != s.npos) { s.replace(pos, 4, "<"); }
    while ((pos = s.find("&amp;")) != s.npos) { s.replace(pos, 5, "&"); }
    return s;
}

// Given: s == "   a + c  "
// RetVal == {"   ", "a", " ", "+", "c", " "}
std::vector<std::string> tokenize(const std::string& s) {
    std::vector<std::string> result;
    std::string temp = "";
    unsigned i = 0;
    while (i < s.length()) {
        while (isspace(s[i]) && (i < s.length())) {
            temp.push_back(s[i]);
            ++i;
        }
        if (temp != "") {
            result.push_back(temp);
            temp = "";
        }
        while (!isspace(s[i]) && (i < s.length())) {
            temp.push_back(s[i]);
            ++i;
        }
        if (temp != "") {
            result.push_back(temp);
            temp = "";
        }
    }
    return result;
}
