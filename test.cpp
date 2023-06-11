//
//  test.cpp
//  Profiler
//
//  Created by Jonathan Maletic.
//  Copyright 2023 Kent State University. All rights reserved.
//  Spring 2023
//  srcML 1.0
//
//  Modified by: Zaz Brown
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "ASTree.hpp"

// Simple function to exercise/test copy-ctor, dtor, swap, assignment.
//
void testCopyAssign(srcML p, std::string codeText) {
    std::stringstream copiedCodeStream;
    copiedCodeStream << p;
    std::string copiedCodeText = copiedCodeStream.str();
    REQUIRE(copiedCodeText == codeText);
    std::cout << "The orginal code: " <<std::endl;
    std::cout << "------------------------------------------------" <<std::endl;
    std::cout << p;   //Should print out same code as printed in main.
    std::cout << "------------------------------------------------" <<std::endl;
    std::cout << "No code: " <<std::endl;
    p = srcML();      //Should print out nothing.
    std::cout << p;
    std::cout << "------------------------------------------------" <<std::endl;
    std::cout << "Foo: " <<std::endl;
    srcML foo;
    std::ifstream inFile("foo.cpp.xml");
    inFile >> foo;
    inFile.close();
    p.swap(foo);
    std::cout << p;
    std::cout << "------------------------------------------------" <<std::endl;
}

void testCopyAssignAST() {
    std::string a = "ayy";
    std::string b = "bee";
    std::string c = "sea";

    auto astA = std::make_unique<AST>(token, a);
    auto astB = std::make_unique<AST>(token, b);
    auto astC = std::make_unique<AST>(token, c);
    AST astAB = AST(category, "ayybees");
    // create astAB with two elements in child: astA and astB
    astAB.child.push_back(std::move(astA));
    astAB.child.push_back(std::move(astB));
    // swap astAB and astA
    astAB.swap(*astC);
}


// Input:  One or more srcML files, main must be first file
// Output: One or more cpp files (with p- prefix) that are instrumented
//
// Example: profile main.cpp.xml utils.cpp.xml
// Results: p-main.cpp p-utlis.cpp
//

TEST_CASE("monolithic test") {
    srcML                     code;         //Source code to be profiled.
    std::vector<std::string>  inputName   = {"simple.cpp.xml"};
    std::vector<std::string>  fileName    = {"simple.cpp"};
    std::vector<std::string>  profileName = {"simple_cpp"};

    //std::string temp = "simple.cpp.xml";
    //inputName.push_back(temp);                        //Put in list

    std::string in0 = inputName[0];
    std::string temp = in0.substr(0, in0.find(".xml"));         //Remove .xml
    fileName.push_back(temp);                         //Put in list
    std::replace(temp.begin(), temp.end(), '.', '_'); //convert . to _
    profileName.push_back(temp);                      //Put in list

    std::ifstream inFile(in0.c_str());    //Read in the main
    inFile >> code;
    inFile.close();

    code.mainHeader(profileName, fileName);   //Add in main header
    code.mainReport(profileName);             //Add in the report
    code.functionCount(profileName[0]);       //Count funciton invocations
    code.lineCount(profileName[0]);           //Count line invocations

    std::string outFileName;
    outFileName = "p-" + fileName[0];
    std::ofstream outFile(outFileName.c_str());
    outFile << code << std::endl;
    outFile.close();

    for (unsigned i = 1; i < inputName.size(); ++i) { //Read rest of the files
        inFile.open(inputName[i].c_str());
        inFile >> code;
        inFile.close();

        code.fileHeader(profileName[i]);       //Add in file header info
        code.functionCount(profileName[i]);    //Count funciton invocations
        code.lineCount(profileName[i]);        //Count line invocations

        outFileName = "p-" + fileName[i];
        outFile.open(outFileName.c_str());
        outFile << code << std::endl;
        outFile.close();
    }

    std::cout << "The code in main: " <<std::endl;
    std::cout << "------------------------------------------------" <<std::endl;
    std::cout << code << std::endl;
    std::cout << "------------------------------------------------" <<std::endl;
    std::stringstream codeText;
    codeText << code;
    std::cout << codeText.str() << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!" <<std::endl;
    testCopyAssign(code, codeText.str());
    testCopyAssignAST();
}
