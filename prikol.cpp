#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <string>

using namespace std;

#define ID3_MAX_SIZE 128
#define ID3_MAX_SIZE_ENH 227

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

struct ID3_v1_Enh_Tags
{
    char signature[4];
    char Title2[60];
    char Artist2[60];
    char Album2[60];
    char Speed;
    char Genre[30];
    char StartTime[6];
    char EndTime[6];
};

streamoff id3_file_offset(ifstream& file)
{
    file.seekg(0, ios::end);
    return file.tellg() - static_cast<streamoff>(ID3_MAX_SIZE);
}

streamoff id3_file_offset_2(ifstream& file)
{
    file.seekg(0, ios::end);
    return file.tellg() - static_cast<streamoff>(ID3_MAX_SIZE_ENH);
}

int main()
{
    //cout << "Enter MP3 file name: ";
    string mp3_name;
    //mp3_name = "target.mp3";
    //mp3_name = "input.mp3";
    mp3_name = "[muzrecord.com]_luis-fonsi-daddy-yankee-despacito.mp3";
    //cin >> mp3_name;
    ifstream file(mp3_name, ios::binary);
    if (!file)
    {
        cout << "Unable to open file for reading." << endl;
        return 1;
    }

    cout << "Select ID3 version (V1/V1_Enh/V1.1): ";
    string ID3_controller;
    cin >> ID3_controller;
    if (ID3_controller == "V1") {
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
            cout << "Genre:       " << unsigned short(id3Tag->genre) << endl;
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

            int new_genre;
            cin >> new_genre;
            id3Tag_new.genre = unsigned char(new_genre);

            memcpy(newID3, &id3Tag_new, sizeof(id3Tag_new));
            for (int i = 3; i < 127; i++) {
                if (i == 3)
                    i += strlen(id3Tag_new.name);
                if (i == 33)
                    i += strlen(id3Tag_new.artist);
                if (i == 63)
                    i += strlen(id3Tag_new.album);
                if (i == 93)
                    i += strlen(id3Tag_new.year);
                if (i == 97)
                    i += strlen(id3Tag_new.description);
                newID3[i] = '\0';
            }
            memcpy(buffer + fileSize - 128, newID3, 128);

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
    else if (ID3_controller == "V1_Enh") {
        char buf[ID3_MAX_SIZE_ENH] = { 0 };
        file.seekg(id3_file_offset_2(file), ios::beg);
        file.read(buf, ID3_MAX_SIZE_ENH);

        ID3_v1_Enh_Tags* id3Tag = reinterpret_cast<ID3_v1_Enh_Tags*>(buf);
        if (id3Tag)
        {
            cout << "Current ID3 Enh:" << endl;
            cout << "Name:        " << id3Tag->Title2 << endl;
            cout << "Artist:      " << id3Tag->Artist2 << endl;
            cout << "Album:       " << id3Tag->Album2 << endl;
            cout << "Speed: " << id3Tag->Speed << endl;
            cout << "Genre:       " << id3Tag->Genre << endl;
            cout << "Start time: " << id3Tag->StartTime << endl;
            cout << "End time: " << id3Tag->EndTime << endl;   
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

            char newID3[ID3_MAX_SIZE_ENH] = { 0 };

            ID3_v1_Enh_Tags id3Tag_new;
            id3Tag_new.signature[0] = 'T';
            id3Tag_new.signature[1] = 'A';
            id3Tag_new.signature[2] = 'G';
            id3Tag_new.signature[3] = '+';
            cout << "New ID3:" << endl;
            cout << "Enter new name: ";
            cin.ignore();
            cin.getline(id3Tag_new.Title2, sizeof(id3Tag_new.Title2));
            cout << "Enter new artist: ";
            cin.getline(id3Tag_new.Artist2, sizeof(id3Tag_new.Artist2));
            cout << "Enter new album: ";
            cin.getline(id3Tag_new.Album2, sizeof(id3Tag_new.Album2));
            cout << "Enter new speed: ";
            cin>> id3Tag_new.Speed;
            cin.ignore();
            cout << "Enter new genre: ";
            cin.getline(id3Tag_new.Genre, sizeof(id3Tag_new.Genre));
            cout << "Enter new start time: ";
            cin.getline(id3Tag_new.StartTime, sizeof(id3Tag_new.StartTime));
            cout << "Enter new end time: ";
            cin.getline(id3Tag_new.EndTime, sizeof(id3Tag_new.EndTime));
            memcpy(newID3, &id3Tag_new, sizeof(id3Tag_new));
            for (int i = 4; i < 227; i++) {
                if (i == 4)
                    i += strlen(id3Tag_new.Title2);
                if (i == 64)
                    i += strlen(id3Tag_new.Artist2);
                if (i == 124)
                    i += strlen(id3Tag_new.Album2);
                if (i == 184)
                    i += 2;
                if (i == 215)
                    i += strlen(id3Tag_new.StartTime);
                if (i == 221) {
                    i += strlen(id3Tag_new.EndTime);
                    if (i > 227)
                        break;
                }
                newID3[i] = '\0';
            }
            memcpy(buffer + fileSize - 227, newID3, 227);
            // Записываем измененный буфер в целевой файл
            targetFile.write(buffer, fileSize);

            // Освобождаем память, закрываем файлы
            delete[] buffer;
            sourceFile.close();
            targetFile.close();

            cout << "The MP3 file has been successfully overwritten!" << endl;

            return 0;
        }
        else return 0;
    }
    else if (ID3_controller == "V1.1") {
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
            cout << "Track: " << unsigned short(id3Tag->Track) << endl;
            cout << "Genre:       " << unsigned short(id3Tag->genre) << endl;
        }
        file.close();

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

        ID3TagV1_1 id3Tag_new;
        id3Tag_new.signature[0] = 'T';
        id3Tag_new.signature[1] = 'A';
        id3Tag_new.signature[2] = 'G';
        //id3Tag_new.zeroByte='\0';
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
            if (i == 3)
                i += strlen(id3Tag_new.name);
            if (i == 33)
                i += strlen(id3Tag_new.artist);
            if (i == 63)
                i += strlen(id3Tag_new.album);
            if (i == 93)
                i += strlen(id3Tag_new.year);
            if (i == 97)
                i += strlen(id3Tag_new.description);
            if (i == 126)
                i += 1;
            if (i == 127)
                break;
            newID3[i] = '\0';
        }
        memcpy(buffer + fileSize - 128, newID3, 128);

        // Записываем измененный буфер в целевой файл
        targetFile.write(buffer, fileSize);

        // Освобождаем память, закрываем файлы
        delete[] buffer;
        sourceFile.close();
        targetFile.close();

        cout << "The MP3 file has been successfully overwritten!" << endl;

        return 0;
    }
    else {
        cout << "Wrong version!" << endl;
        return 1;
    }
        
}