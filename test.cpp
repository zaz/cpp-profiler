//
//  main.cpp
//  Profiler
//
//  Created by Jonathan Maletic.
//  Copyright 2023 Kent State University. All rights reserved.
//  Spring 2023
//  srcML 1.0
//
//  Modified by: Zaz Brown
//

#include "catch.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "ASTree.hpp"

// Simple function to exercise/test copy-ctor, dtor, swap, assignment.
//
void testCopyAssign(srcML p) {
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
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Input file(s) are required." << std::endl;
        std::cerr << "       The main must be the first argument followed by ";
        std::cerr << "any other .cpp files.  For example:" << std::endl;
        std::cerr << "profiler main.cpp.xml file1.cpp.xml file2.cpp.xml";
        std::cerr << std::endl << std::endl;
        return(1);
    }

    srcML                     code;           //Source code to be profiled.
    std::vector<std::string>  inputName;      //Input file names (foo.cpp.xml)
    std::vector<std::string>  fileName;       //File names       (foo.cpp)
    std::vector<std::string>  profileName;    //Profile names    (foo_cpp)

    for (int i = 1; i < argc; ++i) {
        std::string temp;
        temp = argv[i];                                   //Get foo.cpp.xml
        inputName.push_back(temp);                        //Put in list
        temp = temp.substr(0, temp.find(".xml"));         //Remove .xml
        fileName.push_back(temp);                         //Put in list
        std::replace(temp.begin(), temp.end(), '.', '_'); //convert . to _
        profileName.push_back(temp);                      //Put in list
    }

    std::ifstream inFile(inputName[0].c_str());    //Read in the main
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
    testCopyAssign(code);
    testCopyAssignAST();

    return 0;
}
