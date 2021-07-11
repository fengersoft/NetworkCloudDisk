#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H
#include <QString>
struct FileUser
{
    QString uid;
    QString uuid;
    QString parentUuid;
    QString name;
    QString md5;
    QString fileType;
    QString dirName;
    int fileSize = 0;
    int isFolder;
    int isLeft = 1;
};

#endif // DATASTRUCTS_H
