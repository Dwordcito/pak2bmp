// PAK2BMP.cpp: define el punto de entrada de la aplicación de consola.
//

#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

//taked from https://raw.githubusercontent.com/marcosd4h/sysmonx/master/src/common/LibCommon/cmdparser.h
#include "CmdParser.h"

using namespace std;

typedef struct stBrushtag
{
	short sx;
	short sy;
	short szx;
	short szy;
	short pvx;
	short pvy;
} stBrush;

constexpr auto SpriteHeader = 24ul;
constexpr auto ImageHeader = 100ul;


bool GetFileNameListFromFolder(const std::string & folder, std::vector<std::string> & files_in_folder)
{
	bool ret_val{ false };
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(string(folder + "/*.*").c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				std::cout << fd.cFileName << std::endl;
				files_in_folder.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
		ret_val = true;
	}
	return ret_val;
}


bool ExtractImagesFromPakFile(const std::string & input_path, const std::string & input_file_name, const std::string & output_path)
{
	bool ret_val{ true };
	FILE* pFile;
	char* buffer;
	FILE* pFileWrite, * pFileWriteRects;
	stBrush m_stBrush;

	std::string input_file_path = input_path;
	input_file_path.append(input_file_name);

	pFile = fopen(input_file_path.c_str(), "rb");
	if (nullptr != pFile)
	{
		uint32_t sprite_position{ 0u };
		fseek(pFile, SpriteHeader, SEEK_SET);
		fread(&sprite_position, 4, 1, pFile);
		int sprite_quantity = (sprite_position - SpriteHeader) / 8;

		for (int i = 0; i < sprite_quantity; i++)
		{
			rewind(pFile);
			fseek(pFile, SpriteHeader + i * 8, SEEK_SET);
			fread(&sprite_position, 4, 1, pFile);

			uint32_t frames_quantity;
			fseek(pFile, sprite_position + ImageHeader, SEEK_SET);
			fread(&frames_quantity, 4, 1, pFile);


			std::string file_output_rect = output_path;
			file_output_rect.append(input_file_name);
			file_output_rect.append(".");
			file_output_rect.append(std::to_string(i));
			file_output_rect.append(".rect");

			pFileWriteRects = fopen(file_output_rect.c_str(), "wb");
			if (nullptr != pFileWriteRects)
			{
				buffer = new char[frames_quantity * sizeof(stBrushtag) + 1];
				ZeroMemory(buffer, sizeof(buffer));
				for (uint32_t u = 0; u < frames_quantity; u++)
				{
					fread(buffer, sizeof(m_stBrush), 1, pFile);
					fwrite(buffer, sizeof(char), sizeof(stBrushtag), pFileWriteRects);
				}

				fclose(pFileWriteRects);
				delete[] buffer;
			}
			else
			{
				std::cout << "Error to open rect file, to write frame structure: " << file_output_rect << " errno: " << errno << endl;
			}

			uint32_t bitmap_size = { 0u };
			fseek(pFile, sprite_position + (ImageHeader + 10 + (sizeof(stBrushtag) * frames_quantity)), SEEK_SET);
			fread(&bitmap_size, 4, 1, pFile);

			buffer = new char[bitmap_size + 1];
			memset(buffer, 0, bitmap_size + 1);

			fseek(pFile, -6, SEEK_CUR);
			fread(buffer, 1, bitmap_size, pFile);

			std::string file_output_image = output_path;
			file_output_image.append(input_file_name);
			file_output_image.append(".");
			file_output_image.append(std::to_string(i));
			file_output_image.append(".bmp");

			pFileWrite = fopen(file_output_image.c_str(), "wb");
			if (nullptr != pFileWrite)
			{
				fwrite(buffer, sizeof(char), bitmap_size, pFileWrite);
				fclose(pFileWrite);
			}
			else
			{
				std::cout << "Error to open image file, to write image file: " << file_output_image << " errno: " << errno <<endl;
			}
			delete[] buffer;
		}
		fclose(pFile);
	}
	else
	{
		std::cout << "Error to open sprite file: " << input_file_path << endl;
	}
	return ret_val;
}
int main(int argc, char *argv[])
{
	std::vector <std::string> files;

	std::unique_ptr<CmdArgsParser> cmd_parser = make_unique<CmdArgsParser>();
	std::string input_string;
	std::string output_string;

	cmd_parser->Initialize(argc, argv);
	if (cmd_parser->WasOptionRequested("-i")
		&& cmd_parser->WasOptionRequested("-o"))
	{
		cmd_parser->GetOptionValue("-i", input_string);
		cmd_parser->GetOptionValue("-o", output_string);
		if (GetFileNameListFromFolder(input_string, files))
		{
			for (auto it = files.begin(); it != files.end(); ++it)
			{
				input_string.append("\\");
				output_string.append("\\");
				ExtractImagesFromPakFile(input_string, *it, output_string);
			}
		}
	}
	else
	{
		std::cout << "Error " << endl;
		std::cout << "example paktobmp.exe -i c:\\temp\\input -o c:\\temp\\output" << endl;
	}
	
    return 0;
}

