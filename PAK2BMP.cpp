// PAK2BMP.cpp: define el punto de entrada de la aplicación de consola.
//

#include <Windows.h>
#include <vector>

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



vector<string> get_all_files_names_within_folder(string folder)
{
	vector<string> names;
	char search_path[600];
	string str;
	sprintf(search_path, "%s/*.*", folder.c_str());
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				str = fd.cFileName;
				if (str.find("PAK2BMP")) {
					names.push_back(str);
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

DWORD getFileSize(string filename)
{
	FILE *p_file = NULL;
	p_file = fopen(filename.c_str(), "rb");
	fseek(p_file, 0, SEEK_END);
	DWORD size = ftell(p_file);
	fclose(p_file);
	return size;
}
int main(int argc, char *argv[])
{
	DWORD dwSize;
	FILE * pFile;
	int spritecantidad;
	char cNewName[512];
	char* buffer;
	FILE * pFileWrite, *pFileWriteRects;
	string * sFullDirectory;
	stBrush m_stBrush;
	UINT iIndiceSprite, iPosicionComienzoSprite, iCantidadDeFrames, iBmpSize;
	vector <string> archivos = get_all_files_names_within_folder(argv[1]);
	for (UINT j = 0; j < archivos.size(); j++) {
		sFullDirectory = new string(argv[1] + ("\\"+archivos[j]));
		dwSize = getFileSize(*sFullDirectory);
		pFile = fopen(sFullDirectory->c_str(), "rb");
		iIndiceSprite = 0;
		iPosicionComienzoSprite = 0;
		iCantidadDeFrames = 0;
		iBmpSize = 0;
		spritecantidad = 20;
		
		if (pFile == NULL) {
			fputs("File error", stderr); exit(1);
		}

		for (int i = 0; i < spritecantidad; i++) {
			iIndiceSprite = i;
			rewind(pFile);
			fseek(pFile, 24 + iIndiceSprite * 8, SEEK_SET);
			fread(&iPosicionComienzoSprite, 4, 1, pFile);

			if (i == 0) {
				spritecantidad = (iPosicionComienzoSprite - 24) / 8;
			}

			fseek(pFile, iPosicionComienzoSprite + 100, SEEK_SET);
			fread(&iCantidadDeFrames, 4, 1, pFile);

			buffer = new char[iCantidadDeFrames*12 + 1];
			ZeroMemory(buffer, sizeof(buffer));
			ZeroMemory(cNewName, sizeof(cNewName));
			sprintf(cNewName, "%s\\PAK2BMP\\%s%d.rect",argv[1], archivos[j].c_str(), i);
			pFileWriteRects = fopen(cNewName, "wb");
			
			for (UINT u = 0; u < iCantidadDeFrames; u++) {
				fread(&m_stBrush, sizeof(m_stBrush), 1, pFile);
				fwrite(buffer, sizeof(char), 12, pFileWriteRects);
			}

			fclose(pFileWriteRects);
			delete buffer;
			

			fseek(pFile, iPosicionComienzoSprite + (110 + (12 * iCantidadDeFrames)), SEEK_SET);
			fread(&iBmpSize, 4, 1, pFile);

			buffer = new char[iBmpSize + 1];
			ZeroMemory(buffer, sizeof(buffer));

			fseek(pFile, -6, SEEK_CUR);
			fread(buffer, 1, iBmpSize, pFile);

			ZeroMemory(cNewName, sizeof(cNewName));
			sprintf(cNewName, "D:\\Octavio\\Paks\\PAK2BMP\\%s%d.bmp", archivos[j].c_str(), i);
			pFileWrite = fopen(cNewName, "wb");
			fwrite(buffer, sizeof(char), iBmpSize, pFileWrite);
			fclose(pFileWrite);
			delete buffer;
		}
		fclose(pFile);
		delete sFullDirectory;
		sFullDirectory = NULL;
	}
	
    return 0;
}

