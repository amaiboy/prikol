#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <string>

//test

using namespace std;

#define ID3_MAX_SIZE 128

struct ID3Tag
{
    char signature[3];
    char name[30];
    char artist[30];
    char album[30];
    char year[4];
    char description[30];
    unsigned char genre;
};

struct ID3TagV1_1
{
    char signature[3];
    char name[30];
    char artist[30];
    char album[30];
    char year[4];
    char description[28];
    char zeroByte = '\0';
    char Track;
    unsigned char genre;
};

streamoff id3_file_offset(ifstream& file)
{
    file.seekg(0, ios::end);
    return file.tellg() - static_cast<streamoff>(ID3_MAX_SIZE);
}

string printID3Versions(const string& filename) 
{
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cout << "Не удалось открыть файл" << endl;
        return "0";
    }

    char header[10];
    file.read(header, 10);

    if (file.gcount() < 10) {
        cout << "Неверный формат файла" << std::endl;
        return"0";
    }

    if (string(header, 3) != "ID3") {
        cout << "ID3 тег не найден" << std::endl;
        return "0";
    }

    cout << "Версии ID3-тегов:" << endl;
    if (header[3] == 3 && header[4] == 0) {
        cout << "ID3v2.3" << endl;
    }
    else if (header[3] == 4 && header[4] == 0) {
        cout << "ID3v2.4" << endl;
    }

    // Перемещаемся в конец файла и ищем ID3v1 тег
    file.seekg(id3_file_offset(file), ios::beg);

    char id3v1Header[3];
    file.read(id3v1Header, 3);
    if (string(id3v1Header, 3) == "TAG") {
        file.seekg(0, ios::beg);
        char buf[ID3_MAX_SIZE] = { 0 };
        file.seekg(id3_file_offset(file), ios::beg);
        file.read(buf, ID3_MAX_SIZE);

        if (buf[125] == '\0') {
            if (buf[126] != '\0') {
                cout << "ID3v1.1" << endl;
                return "v1.1";
            }
            else {
                cout << "ID3v1.0" << endl;
                return "v1";
            }
        }
    }
    file.close();
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    string mp3_name;
    ifstream file;
    do
    {
        cout << "Enter path:";
        getline(cin, mp3_name);
        file.open(mp3_name, ios::binary);
        if (!file)
        {
            cout << "Unable to open file for reading, try again" << endl;
        }
    } while(!file);
    int version_detection; 
    string ID3_controller; 
    cout << "you want to select the version manually or have the program detect it. (1-detect program), (2-manually):";
    cin >> version_detection;
    if (version_detection == 1) 
    { 
       ID3_controller = printID3Versions(mp3_name);
   
    }
    else if (version_detection == 2)
    { 
        cout << "Select ID3 version (V1/V1.1): ";
        cin >> ID3_controller; 
    }
    else 
    {
        cout << "Wrong variant";
        return 1;
    }
    
    if (ID3_controller == "V1" || ID3_controller=="v1") {
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
            cout << "Year:        " << string(id3Tag->year, 4) << endl;
            cout << "Description: " << id3Tag->description << endl;
            cout << "Genre:       " << unsigned short(id3Tag->genre) << endl;
        }
        file.close();

        cout << "Do you want to change ID3?" << endl;
        cout << "Press Y/y or N/n" << endl;
        char controller;
        cin >> controller;
        if (controller == 'Y' || controller == 'y') {
            cout << "Okay, let's change it!" << endl;
            ifstream sourceFile(mp3_name, ios::binary);
            ofstream targetFile("target.mp3", ios::binary);

            sourceFile.seekg(0, ios::end);
            streampos fileSize = sourceFile.tellg();
            sourceFile.seekg(0, ios::beg);

            char* buffer = new char[fileSize];

            
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

            int new_genre;
            cin >> new_genre;
            id3Tag_new.genre = unsigned char(new_genre);

            memcpy(newID3, &id3Tag_new, sizeof(id3Tag_new));
            for (int i = 3; i < 128; i++) {
                if (newID3[i] == (-52))
                    newID3[i] = '\0';
            }
            memcpy(buffer + fileSize - 128, newID3, 128);

            
            targetFile.write(buffer, fileSize);

           
            delete[] buffer;

            sourceFile.close();
            remove(mp3_name.c_str());
            targetFile.close();
            rename("target.mp3", mp3_name.c_str());

            cout << "The MP3 file has been successfully overwritten!" << endl;

            return 0;
        }
        else
            return 0;
    }
    
    else if (ID3_controller == "V1.1" || ID3_controller == "v1.1") {
        char buf[ID3_MAX_SIZE] = { 0 };
        file.seekg(id3_file_offset(file), ios::beg);
        file.read(buf, ID3_MAX_SIZE);

        ID3TagV1_1* id3Tag = reinterpret_cast<ID3TagV1_1*>(buf);
        if (id3Tag)
        {
            cout << "Current ID3:" << endl;
            cout << "Name:        " << id3Tag->name << endl;
            cout << "Artist:      " << id3Tag->artist << endl;
            cout << "Album:       " << id3Tag->album << endl;
            cout << "Year:        " << std::string(id3Tag->year, 4) << endl;
            cout << "Description: " << id3Tag->description << endl;
            cout << "Track:       " << unsigned short(id3Tag->Track) << endl;
            cout << "Genre:       " << unsigned short(id3Tag->genre) << endl;
        }
        file.close();

        cout << "Press Y/y or N/n" << endl;
        char controller;
        cin >> controller;
        if (controller == 'Y' || controller == 'y')
        {
            cout << "Okay, let's change it!" << endl;
            ifstream sourceFile(mp3_name, ios::binary);
            ofstream targetFile("target.mp3", ios::binary);

            sourceFile.seekg(0, ios::end);
            streampos fileSize = sourceFile.tellg();
            sourceFile.seekg(0, ios::beg);

            
            char* buffer = new char[fileSize];

           
            sourceFile.read(buffer, fileSize);

            char newID3[ID3_MAX_SIZE] = { 0 };

            ID3TagV1_1 id3Tag_new;
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
            cout << "Enter new track: ";
            int new_track;
            cin >> new_track;
            id3Tag_new.Track = unsigned char(new_track);
            cout << "Enter new genre: ";

            int new_genre;
            cin >> new_genre;
            id3Tag_new.genre = unsigned char(new_genre);

            memcpy(newID3, &id3Tag_new, sizeof(id3Tag_new));
            for (int i = 3; i < 128; i++) {
                if (newID3[i] == (-52))
                    newID3[i] = '\0';
            }
            memcpy(buffer + fileSize - 128, newID3, 128);

           
            targetFile.write(buffer, fileSize);

         
            delete[] buffer;
            sourceFile.close();
            remove(mp3_name.c_str());
            targetFile.close();
            rename("target.mp3", mp3_name.c_str());
            cout << "The MP3 file has been successfully overwritten!" << endl;

            return 0;
        }
        else
            return 0;
    }
  else {
      cout << "Wrong version!" << endl;
      return 1;
    }
        
}
