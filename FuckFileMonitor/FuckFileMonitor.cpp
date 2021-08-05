// FuckFileMonitor.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <io.h>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>

struct FileInformation
{
    size_t encSize;
    size_t picSize;
    char picName[64];
};

std::vector<std::string> split(std::string srcStr, const std::string& delim)
{
	int nPos = 0;
	std::vector<std::string> vec;
	nPos = srcStr.find(delim.c_str());
	while (-1 != nPos)
	{
		std::string temp = srcStr.substr(0, nPos);
		vec.push_back(temp);
		srcStr = srcStr.substr(nPos + 1);
		nPos = srcStr.find(delim.c_str());
	}
	vec.push_back(srcStr);
	return vec;
}

std::string getCurrentDirectory()
{
    char moduleFileName[MAX_PATH * 4] = { 0 };
    GetModuleFileNameA(NULL, moduleFileName, MAX_PATH * 4);

    std::string workingFolder(moduleFileName);
    size_t pos = workingFolder.find_last_of("\\");
    return workingFolder.substr(0, pos + 1);
}

std::string getPathFileName(const std::string& path)
{
    std::string name(path);
    auto pos = name.rfind('\\');
    if (std::string::npos != pos)
    {
        name = name.substr(pos + 1);
    }
    else
    {
        auto pos = name.rfind('/');
        if (std::string::npos != pos)
        {
            name = name.substr(pos + 1);
        }
    }
    return name;
}

bool getDirectoryFiles(const std::string& dir, std::vector<std::string>& files)
{
    struct _finddata_t fileinfo;
    auto hFile = _findfirst((dir + "\\*.*").c_str(), &fileinfo);
    if (hFile != -1)
    {
        do
        {
            if (!(fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    files.push_back(dir + "\\" + fileinfo.name);
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
    return !files.empty();
}

bool getFileContent(const std::string& path, size_t offset, char** buffer, size_t* size)
{
    std::ifstream in(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!in.is_open())
    {
        printf("open file fail!\n");
        return false;
    }
    auto fsize = in.tellg();
    if (fsize >= ULONG_MAX || fsize <= offset)
    {
        printf("file size over memory! f:%d, o:%d\n", (size_t)fsize, offset);
        return false;
    }

    if (size)
    {
        *size = (size_t)fsize;
    }

    if (buffer)
    {
        auto s = (int)fsize - offset;
        auto b = new char[s];
        in.seekg(offset, std::ios::beg);
        in.read(b, s);

        *buffer = b;
        if (size)
        {
            *size = s;
        }
    }
    
    in.close();
    return true;
}

bool setFileContent(const std::string& path, char* buffer, size_t size)
{
    if (!buffer || !size)
    {
        printf("parameter fail!\n");
        return false;
    }
    std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::app);
    if (!out.is_open())
    {
        printf("open write file fail!\n");
        return false;
    }
    out.write(buffer, size);
    out.close();
    return true;
}

bool encrypt(const std::string& pic, const std::string& zip, const std::string& save)
{
    size_t size = 0;
    char* buf = nullptr;
    bool result = false;
    do
    {
        FileInformation fileInfo = { 0 };
        std::string name = getPathFileName(zip);
        int length = name.length() < sizeof(fileInfo.picName) ? name.length() : sizeof(fileInfo.picName);
        name.copy(fileInfo.picName, length);

        if (!getFileContent(pic, 0, &buf, &size))
        {
            printf("get file size fail!\n");
            break;
        }

        fileInfo.picSize = size;
        if (!setFileContent(save, buf, size))
        {
            printf("set file content fail!\n");
            break;
        }

        if (buf)
        {
            size = 0;
            delete[] buf;
        }

        if (!getFileContent(zip, 0, &buf, &size))
        {
            printf("get file size fail!\n");
            break;
        }

        fileInfo.encSize = size;
        if (!setFileContent(save, buf, size))
        {
            printf("set file content fail!\n");
            break;
        }

        if (!setFileContent(save, (char*)&fileInfo, sizeof(fileInfo)))
        {
            printf("set file content fail!\n");
            break;
        }

        result = true;

    } while (false);
    
    if (buf)
    {
        size = 0;
        delete[] buf;
    }
    return result;
}

bool decrypt(const std::string & zip, const std::string& save)
{
    size_t size = 0;
    char* buf = nullptr;
    bool result = false;
    do
    {
        FileInformation fileInfo = { 0 };
        if (!getFileContent(zip, 0, &buf, &size))
        {
            printf("get file content fail!\n");
            return false;
        }

        auto pFileInfo = (FileInformation*)& buf[size - sizeof(fileInfo)];
        if (!pFileInfo)
        {
            printf("get file information fail!\n");
            return false;
        }

        if (pFileInfo->encSize >= size || pFileInfo->picSize >= size)
        {
            printf("analyze file information fail!\n");
            return false;
        }

        std::string picName(pFileInfo->picName);
        if (picName.empty())
        {
            printf("get file information name fail!\n");
            return false;
        }

        if (!setFileContent(save, &buf[pFileInfo->picSize], pFileInfo->encSize))
        {
            printf("set file content fail!\n");
            return false;
        }

        result = true;

    } while (false);

    if (buf)
    {
        size = 0;
        delete[] buf;
    }
    return result;
}

int main(int argv, char* argc[])
{
    if (argv < 4)
    {
        printf("parameter fail! - exe encrypt pic.png enc_files dec_files \n");
        return 1;
    }
    bool enc = false;
    if (0 == _stricmp("encrypt", argc[1]))
    {
        enc = true;
    }
    else if (0 == _stricmp("decrypt", argc[1]))
    {
        enc = false;
    }
    else
    {
        printf("parameter fail! - exe encrypt pic.png enc_files dec_files \n");
        return 1;
    }

    std::vector<std::string> picfiles;
    if (enc)
    {
        if (!getDirectoryFiles(argc[2], picfiles) || picfiles.empty())
        {
            printf("get picfiles fail!\n");
            return 1;
        }
    }

    std::vector<std::string> files;
    if (!getDirectoryFiles(argc[3], files) || files.empty())
    {
        printf("get files fail!\n");
        return 1;
    }
    
    for (size_t i = 0, j = 0, k = 0; i < files.size(); i++, j++)
    {        
        if (enc)
        {
            if (j >= picfiles.size())
            {
                j = 0;
                k++;
            }
			auto tmp = split(getPathFileName(picfiles[j]), ".");
			if (tmp.empty() || tmp.size() < 2)
			{
				continue;
			}
            std::string name = std::string(argc[4]) + "\\" + tmp[0] + std::to_string(k) + "." + tmp[1];
            printf("encrypt %s - %s - %s\n", picfiles[j].c_str(), files[i].c_str(), name.c_str());

            if (encrypt(picfiles[j], files[i], name))
            {
                printf("encrypt %s - %s - %s done!\n", picfiles[j].c_str(), files[i].c_str(), name.c_str());
            }
        }
        else
        {
            char save[512] = { 0 };
            sprintf_s(save, sizeof(save), "%s/decrypt.7z.%05ld", argc[4], i + 1);
            printf("decrypt %s - %s\n", files[i].c_str(), save);

            if (decrypt(files[i], save))
            {
                printf("decrypt %s - %s done!\n", files[i].c_str(), save);
            }
        }
    }

    return 0;
}