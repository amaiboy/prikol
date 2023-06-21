#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>

using namespace std;

#define ID3_MAX_SIZE 128

struct ID3Tag
{
    char signature[3];
    char name[30];
    char artist[30];
    char album[30];
    char year[4];
    char description[28];
    char genre[2];
};

streamoff id3_file_offset(ifstream& file)
{
    file.seekg(0, ios::end);
    return file.tellg() - static_cast<streamoff>(ID3_MAX_SIZE);
}

int main()
{
    //cout << "Enter MP3 file name: ";
    string mp3_name;
    //mp3_name = "[muzrecord.com]_luis-fonsi-daddy-yankee-despacito.mp3";
    mp3_name = "input.mp3";
    //cin >> mp3_name;
    ifstream file(mp3_name, ios::binary);
    if (!file)
    {
        cout << "Unable to open file for reading." << endl;
        return 1;
    }

    char buf[ID3_MAX_SIZE] = { 0 };
    file.seekg(id3_file_offset(file), ios::beg);
    file.read(buf, ID3_MAX_SIZE);

    ID3Tag* id3Tag = reinterpret_cast<ID3Tag*>(buf);
    if (id3Tag)
    {
        cout << "Current ID3:" << endl;
        cout << "Name:        " << id3Tag->name << endl;
        cout << "Artist:      " << id3Tag->artist << endl;
        cout << "Album:       " << id3Tag->album << endl;
        cout << "Year:        " << std::string(id3Tag->year, 4) << endl;
        cout << "Description: " << id3Tag->description << endl;
        cout << "Genre:       " << id3Tag->genre << endl;
    }
    file.close();

    cout << "Do you want to change ID3?" << endl;
    cout << "Press Y/y or N/n" << endl;
    char controller;
    cin >> controller;
    if (controller == 'Y' || controller == 'y') {
        cout << "Okay, let's change it!" << endl;
        ifstream sourceFile(mp3_name, std::ios::binary);
        ofstream targetFile("target.mp3", std::ios::binary);

            sourceFile.seekg(0, std::ios::end);
            streampos fileSize = sourceFile.tellg();
            sourceFile.seekg(0, std::ios::beg);

            //Создаем буфер для хранения содержимого файла
            char* buffer = new char[fileSize];

            // Считываем содержимое файла в буфер
            sourceFile.read(buffer, fileSize);

            char newID3[ID3_MAX_SIZE] = { 0 };

            ID3Tag id3Tag_new;
            id3Tag_new.signature[0] = 'T';
            id3Tag_new.signature[1] = 'A';
            id3Tag_new.signature[2] = 'G';
            cout << "New ID3:" << endl;
            cout << "Enter new name: ";
            cin.ignore();
            cin.getline(id3Tag_new.name, sizeof(id3Tag_new.name));
            cout << "Enter new artist: ";
            cin.getline(id3Tag_new.artist, sizeof(id3Tag_new.artist));
            cout << "Enter new album: ";
            cin.getline(id3Tag_new.album, sizeof(id3Tag_new.album));
            cout << "Enter new year: ";
            cin.getline(id3Tag_new.year, 5);
            cout << "Enter new description: ";
            cin.getline(id3Tag_new.description, sizeof(id3Tag_new.description));
            cout << "Enter new genre: ";
            cin.getline(id3Tag_new.genre, sizeof(id3Tag_new.genre));
            memcpy(newID3, &id3Tag_new, sizeof(id3Tag_new));
            memcpy(buffer + fileSize - 128, newID3, 128);
            
            for (int i = 3; i < 128; i++) {
                if (i == 3)
                    i += strlen(id3Tag_new.name);
                if (i == 33)
                    i += strlen(id3Tag_new.artist);
                if (i == 63)
                    i += strlen(id3Tag_new.album);
                if (i == 97)
                    i += strlen(id3Tag_new.year);
                if (i == 125)
                    i += strlen(id3Tag_new.genre);
                newID3[i] = '\0';
            }
            // Записываем измененный буфер в целевой файл
            targetFile.write(buffer, fileSize);

            // Освобождаем память, закрываем файлы
            delete[] buffer;
            sourceFile.close();
            targetFile.close();

            cout << "The MP3 file has been successfully overwritten!" << endl;
      
        return 0;
    }
    else
    return 0;
}