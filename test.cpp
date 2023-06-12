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
void testCopyAssign(srcML p, std::string codeText) {
    std::stringstream copiedCodeStream;
    copiedCodeStream << p;
    std::string copiedCodeText = copiedCodeStream.str();
    REQUIRE(copiedCodeText == codeText);

    p = srcML();
    std::stringstream emptyCodeStream;
    emptyCodeStream << p;
    REQUIRE(emptyCodeStream.str() == "");
}

TEST_CASE("monolithic test") {
    srcML                     code;       //Source code to be profiled.
    std::vector<std::string>  inputName = {"simple.cpp.xml"};
    std::vector<std::string>  fileName;
    std::vector<std::string>  profileName;

    for (auto const& input : inputName) {
        std::string temp = input.substr(0, input.find(".xml"));  //Remove .xml
        fileName.push_back(temp);                                //Put in list
        std::replace(temp.begin(), temp.end(), '.', '_');        //convert . to _
        profileName.push_back(temp);                             //Put in list
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

    SECTION("srcML copy assign") {
        std::stringstream codeText;
        codeText << code;
        testCopyAssign(code, codeText.str());
    }
}

TEST_CASE("test parsing of foo and srcML::swap") {
    std::stringstream fooStream;
    std::ifstream fooFile("foo.cpp");
    fooStream << fooFile.rdbuf();
    fooFile.close();

    srcML foo;
    std::ifstream inFile("foo.cpp.xml");
    inFile >> foo;
    inFile.close();

    std::stringstream fooParsedStream;
    fooParsedStream << foo;

    REQUIRE(fooStream.str() == fooParsedStream.str());

    SECTION("srcML::swap") {
        srcML initiallyEmpty;
        initiallyEmpty.swap(foo);

        std::stringstream parsedStreamForInitiallyEmpty;
        parsedStreamForInitiallyEmpty << initiallyEmpty;
        REQUIRE(fooStream.str() == parsedStreamForInitiallyEmpty.str());

        std::stringstream fooParsedStreamNowEmpty;
        fooParsedStreamNowEmpty << foo;
        REQUIRE("" == fooParsedStreamNowEmpty.str());
    }
}

TEST_CASE("AST") {
    std::string a = "ayy";
    std::string b = "bee";
    std::string c = "sea";

    auto astA = std::make_shared<AST>(category, a);
    auto astB = std::make_shared<AST>(category, b);
    auto astC = std::make_shared<AST>(category, c);

    // create astAB with two elements in child: astA and astB
    auto astAB = std::make_shared<AST>(category, "ayybees");
    astAB->child.push_back(std::move(astA));
    astAB->child.push_back(std::move(astB));

    SECTION("AST::getChild") {
        std::shared_ptr<AST> childAn;
        childAn = astC->getChild(a);
        REQUIRE(childAn == nullptr);

        std::shared_ptr<AST> childA;
        childA = astAB->getChild(a);
        REQUIRE(childA != nullptr);
        REQUIRE(childA->tag == a);
    }

    SECTION("AST::swap") {
        // swap astAB and astC
        astAB->swap(*astC);

        // astAB should now be leaf "sea"
        std::shared_ptr<AST> childAfromAB;
        childAfromAB = astAB->getChild(a);
        REQUIRE(childAfromAB == nullptr);

        // astC should now be "ayybees", containing astA and astB
        std::shared_ptr<AST> childAfromC;
        childAfromC = astC->getChild(a);
        REQUIRE(childAfromC != nullptr);
        REQUIRE(childAfromC->tag == a);
    }
}
