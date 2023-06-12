//
//  profileFiles.cpp
//  Abstract Syntax Tree
//
//  Created by Jonathan Maletic
//  Copyright 2023 Kent State University. All rights reserved.
//  Spring 2023
//  srcML 1.0
//
//  Modified by: Zaz Brown
//

#include "profileFiles.hpp"

std::shared_ptr<srcML> profileFiles(const std::vector<std::filesystem::path>& inFilePaths) {
    srcML                              code;  //Source code to be profiled.
    std::vector<std::filesystem::path> xmlFilePaths;
    std::vector<std::filesystem::path> outFilePaths;
    std::vector<std::string>           profileNames;

    for (auto const& path : inFilePaths) {
        auto xmlFilePath = path;
        xmlFilePath += ".xml";
        xmlFilePaths.push_back(xmlFilePath);

        std::string name = path;
        std::replace(name.begin(), name.end(), '.', '_');
        // TODO: determine if we also need to replace "/"
        profileNames.push_back(name);

        auto outFile = path.parent_path();
        outFile /= "p-";
        outFile += path.filename();
        outFilePaths.push_back(outFile);
    }

    // Read in the main
    std::ifstream inFile(xmlFilePaths[0]);
    if (inFile.fail()) {
        throw std::runtime_error("Error: Could not open file "
                                 + xmlFilePaths[0].string());
    }
    inFile >> code;
    inFile.close();

    code.mainHeader(profileNames, inFilePaths);  //Add in main header
    code.mainReport(profileNames);               //Add in the report
    code.functionCount(profileNames[0]);         //Count funciton invocations
    code.lineCount(profileNames[0]);             //Count line invocations

    std::ofstream outFile(outFilePaths[0]);
    if (outFile.fail()) {
        throw std::runtime_error("Error: Could not open file "
                                 + outFilePaths[0].string());
    }
    outFile << code;
    outFile.close();

    // Read rest of the files
    for (unsigned i = 1; i < xmlFilePaths.size(); ++i) {
        inFile.open(xmlFilePaths[i]);
        if (inFile.fail()) {
            throw std::runtime_error("Error: Could not open file "
                                    + xmlFilePaths[i].string());
        }
        inFile >> code;
        inFile.close();

        code.fileHeader(profileNames[i]);       //Add in file header info
        code.functionCount(profileNames[i]);    //Count funciton invocations
        code.lineCount(profileNames[i]);        //Count line invocations

        outFile.open(outFilePaths[i]);
        if (outFile.fail()) {
            throw std::runtime_error("Error: Could not open file "
                                    + outFilePaths[i].string());
        }
        outFile << code;
        outFile.close();
    }

    return std::make_shared<srcML>(code);
}
