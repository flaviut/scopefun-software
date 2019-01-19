////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 David Košenina
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <vector>
#include <string>

#define MAX_FILE_SIZE 10*1024*1024

using namespace std;

char input[MAX_FILE_SIZE]  = { 0 };
char output[MAX_FILE_SIZE] = { 0 };

void addFiles(vector<wstring>& names,wstring dir)
{
   wstring path = dir;
   path.append(L"*.*");
   WIN32_FIND_DATA fd = { 0 };
   HANDLE handle = FindFirstFile( path.c_str(), &fd);
   do {
      if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
      {
         if( wcscmp(L".", fd.cFileName) != 0 && wcscmp(L"..", fd.cFileName) != 0 )
         {
            wstring newdir = dir;
            newdir.append(fd.cFileName);
            newdir.append(L"/");
            addFiles( names, newdir );
         }
      }
      else
      {
         wstring fileName = dir;
         fileName.append(fd.cFileName);
         names.push_back(fileName);
      }
   } while (::FindNextFile(handle, &fd));
}

void main()
{

   printf("begin");

   // find files
   vector<wstring> names;
   vector<wstring> dir;
   addFiles( names, L"../../source/" );
   addFiles( names, L"../../bin/data/shader21/" );
   addFiles (names, L"../../bin/data/shader32/" );

   // read header
   DWORD bytesRead = 0;
   HANDLE handle     = CreateFile(L"license.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   DWORD  headerSize = GetFileSize(handle, NULL);
   if (headerSize > MAX_FILE_SIZE)
   {
      printf("max header file size exceeded");
      headerSize = 0;
   }
   else
   {
      BOOL ret = ReadFile(handle, output, headerSize, &bytesRead, NULL);
   }
   CloseHandle(handle);

   WCHAR currentDir[1024] = { 0 };
   int ret = GetCurrentDirectory( 1024, (WCHAR*) currentDir);
   wstring workingDir = (WCHAR*)currentDir;

   // write output
   wstring cpp = L".cpp";
   wstring hpp = L".h";
   wstring vs  = L".ps";
   wstring ps  = L".vs";
   for (int i = 0; i < (int)names.size(); i++)
   {
      wstring file = names[i];
      size_t  last = file.find_last_of('.');
      wstring fend = file.substr( last, file.length() - last );
      if ( fend == cpp || fend == hpp || fend == vs || fend == ps )
      {
         // input read
         wstring path = workingDir + L"\\" + file;
         HANDLE handle = CreateFile( path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
         DWORD  inputSize = GetFileSize(handle, NULL);
         if (inputSize + headerSize> MAX_FILE_SIZE)
         {
            printf("max file size exceeded");
            break;
         }
         BOOL ret = ReadFile(handle, input, inputSize, &bytesRead, NULL);
         CloseHandle(handle);

         // find header insert place
         DWORD insertPos;
         if( fend == cpp || fend == hpp )
         {
            for (int c = 0; c < (int)inputSize;c++ )
            {
               if (input[c] == '#')
               {
                  insertPos = c;
                  break;
               }
            }
         }
         else
         {
            string code = (char*)input;
            int     len = (int)strlen("////////////////////////////////////////////////////////////////////////////////");
            insertPos   = (int)code.find("////////////////////////////////////////////////////////////////////////////////", len );
            insertPos  += len;
         }

         // copy
         memcpy_s(&output[headerSize], MAX_FILE_SIZE, &input[insertPos], inputSize - insertPos);

         // output write
         DWORD written = 0;
         handle = CreateFile(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
         ret = WriteFile( handle, output, headerSize + inputSize - insertPos, &written, NULL);
         CloseHandle(handle);
      }
   }
   printf("end");
   getchar();
}