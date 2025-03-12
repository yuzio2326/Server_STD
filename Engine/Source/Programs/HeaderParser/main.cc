#include "parser.h"
#include "handler.h"
#include "options.h"
#include <tclap/CmdLine.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <thread>
#include <omp.h>
#include "Tasks/FillContent.h"
using namespace std;
using namespace rapidjson;
//----------------------------------------------------------------------------------------------------

std::string remove_extension(const std::string& filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
}

// header-parser example1.h -c SCLASS -e SENUM -f SFUNC -p SPROPERTY
//----------------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
	//_sleep(5000);
    Options options;
    //std::vector<std::string> inputFiles;
    std::string inputFile;
    try
    {
        using namespace TCLAP;

        CmdLine cmd("Header Parser");

        ValueArg<std::string> enumName("e", "enum", "The name of the enum macro", false, "ENUM", "", cmd);
        ValueArg<std::string> className("c", "class", "The name of the class macro", false, "CLASS", "", cmd);
        MultiArg<std::string> functionName("f", "function", "The name of the function macro", false, "", cmd);
        ValueArg<std::string> propertyName("p", "property", "The name of the property macro", false, "PROPERTY", "", cmd);
        MultiArg<std::string> customMacro("m", "macro", "Custom macro names to parse", false, "", cmd);
        UnlabeledValueArg<std::string> inputFileArg("inputFile", "The file to process", true, "", "", cmd);

        cmd.parse(argc, argv);

        inputFile = inputFileArg.getValue();
        options.classNameMacro = className.getValue();
        options.enumNameMacro = enumName.getValue();
        options.functionNameMacro = functionName.getValue();
        options.customMacros = customMacro.getValue();
        options.propertyNameMacro = propertyName.getValue();
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return -1;
    }
    std::ifstream fin;
    fin.open(inputFile);

    std::vector<std::string> Files;
	while (!fin.eof())
	{
		std::string line;
		getline(fin, line);
		if (!line.empty())
		{
			Files.push_back(line);
		}
	}
	fin.close();

	const int FileSize = (int)Files.size();
	bool bBuildFailed = false;
#pragma omp parallel for
    for (int i = 0; i < FileSize; ++i)
    {
		std::cout << "tid: " << std::this_thread::get_id() << std::endl;
		std::ifstream Stream(Files[i]);
		std::cerr << "try open " << Files[i] << std::endl;
		if (!Stream.is_open())
		{
			std::cerr << "Could not open " << Files[i] << std::endl;
			bBuildFailed = true;
		}
		else
		{
			std::stringstream buffer;
			buffer << Stream.rdbuf();

			Parser HeaderParser(options);
			if (HeaderParser.Parse(buffer.str().c_str()))
			{
				std::string ParserOutput = HeaderParser.result();
				std::cout << HeaderParser.result() << std::endl;

				std::string NewFileName = remove_extension("HeaderParserGenerated/" + Files[i]) + ".generated.h";
				std::ofstream NewOutFile;
				const std::string NewDirectories = NewFileName.substr(0, NewFileName.find_last_of("\\/"));
				std::filesystem::create_directories(NewDirectories);
				
				Document document;
				document.Parse(ParserOutput.c_str()); 

				bool bFindClass = false;
				for (Value::ConstValueIterator ItDoc = document.Begin(); ItDoc != document.End(); ++ItDoc)
				{
					bool bObject = ItDoc->IsObject();
					if (bObject)
					{
						if (ItDoc->HasMember("type"))
						{
							const std::string FindType = (*ItDoc)["type"].GetString();
							if (FindType == "class")
							{
								if (bFindClass)
								{
									// one header Class > 2
									cout << "[Error] There should be only one SCLASS";
									bBuildFailed = true;
								}

								if (bFindClass == false)
								{
									bFindClass = true;
									NewOutFile.open(NewFileName);
								}

								string FileContent;
								FFillContentTask FillContentTask;
								FillContentTask.RunTask(ItDoc, FileContent);

								NewOutFile.write(FileContent.c_str(), FileContent.size());
							}
						}
					}
					else
					{
						std::cerr << "ItDoc->IsObject() Failed : " << Files[i] << std::endl;
						bBuildFailed = true;
						assert(false);
					}
				}
			}
			else
			{
				std::cerr << "Parse Failed : " << Files[i] << std::endl;
				bBuildFailed = true;
				assert(false);
			}
		}
    }
    
	if (bBuildFailed)
	{
		return -1;
	}

    return 0;
}
