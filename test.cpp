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

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "profileFiles.hpp"
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
    std::vector<std::filesystem::path> inFilePaths;
    inFilePaths.push_back(std::filesystem::path("tests/test.cpp"));
    std::cout << inFilePaths[0] << std::endl;
    std::shared_ptr<srcML> code = profileFiles(inFilePaths);

    SECTION("srcML copy assign") {
        std::stringstream codeText;
        codeText << code;
        testCopyAssign(*code, codeText.str());
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
        std::shared_ptr<AST> childAn = astC->getChild(a);
        REQUIRE(childAn == nullptr);

        std::shared_ptr<AST> childA = astAB->getChild(a);
        REQUIRE(childA != nullptr);
        REQUIRE(childA->tag == a);
    }

    SECTION("AST::swap") {
        // swap astAB and astC
        astAB->swap(*astC);

        // astAB should now be leaf "sea"
        std::shared_ptr<AST> childAfromAB = astAB->getChild(a);
        REQUIRE(childAfromAB == nullptr);

        // astC should now be "ayybees", containing astA and astB
        std::shared_ptr<AST> childAfromC = astC->getChild(a);
        REQUIRE(childAfromC != nullptr);
        REQUIRE(childAfromC->tag == a);
    }

    SECTION("AST::=") {
        std::shared_ptr<AST> astCopy = astAB;

        // check original is unaltered
        std::shared_ptr<AST> child = astAB->getChild(a);
        REQUIRE(child != nullptr);
        REQUIRE(child->tag == a);

        // check the copy is the same as the original
        std::shared_ptr<AST> copyChild = astCopy->getChild(a);
        REQUIRE(copyChild != nullptr);
        REQUIRE(copyChild->tag == a);
    }
}
