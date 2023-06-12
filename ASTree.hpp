//
//  ASTree.hpp
//  Abstract Syntax Tree
//
//  Created by Jonathan Maletic
//  Copyright 2023 Kent State University. All rights reserved.
//  Spring 2023
//  srcML 1.0
//
//  Modified by:
//

#ifndef INCLUDES_ASTree_H_
#define INCLUDES_ASTree_H_

#include <filesystem>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>


bool                     isStopTag (std::string);
std::string              readUntil (std::istream&, char);
std::string              unEscape  (std::string);
std::vector<std::string> tokenize  (const std::string& s);


// AST nodes can be one of three things.
// category   - internal node of some syntactic category
// token      - a source code token
// whitespace - blanks, tabs, line returns, etc.
//
enum nodes {category, token, whitespace};

// An AST is either a:
//     -Syntactic category node
//     -Token node
//     -Whitespace node
//
// CLASS INV: if (nodeType == category)
//            than (child != 0) && (text == "")
//            if ((nodeType == token) || (nodeType == whitespace))
//            then (child == 0) && (text != "")
//
class AST {
public:
                         AST           () {};
                         AST           (nodes t) : nodeType(t) {};
                         AST           (nodes t, const std::string&);
                         ~AST          ();
                         AST           (const AST&);
    void                 swap          (AST&);
    AST&                 operator=     (AST);

    std::shared_ptr<AST> getChild      (std::string) const;
    std::shared_ptr<AST> findTagWithName (const std::string&, const std::string&) const;
    std::string          findName      () const;
    void                 mainHeader    (const std::vector<std::string>&,
                                        const std::vector<std::filesystem::path>&);
    void                 fileHeader    (const std::string&);
    void                 mainReport    (const std::vector<std::string>&);
    void                 functionCount (const std::string&);
    void                 lineCount     (const std::string&);
    std::ostream&        print         (std::ostream&) const;
    std::istream&        read          (std::istream&);

// private:
    std::string          getName       () const;
    nodes                           nodeType;   //Category, Token, or Whitespace
    std::string                     tag,        //Category: the tag name and
                                    closeTag;   //          closing tag.
    std::list<std::shared_ptr<AST>> child;      //Category: A list of subtrees.
    std::string                     text;       //Token/Whitespace: the text.
};


// srcML is an internal data structure for a srcML input file.
// CLASS INV: Assigned(tree)
//
class srcML {
public:
            srcML         () : tree(0) {};
            ~srcML        () {delete tree;};
            srcML         (const srcML&);
    void    swap          (srcML&);
    srcML&  operator=     (srcML);
    void    mainHeader    (const std::vector<std::string>&,
                           const std::vector<std::filesystem::path>&);
    void    fileHeader    (const std::string&);
    void    mainReport    (const std::vector<std::string>&);
    void    functionCount (const std::string&);
    void    lineCount     (const std::string&);

    friend  std::istream& operator>>(std::istream&, srcML&);
    friend  std::ostream& operator<<(std::ostream&, const srcML&);

private:
    std::string header;
    AST*        tree;
};


#endif
