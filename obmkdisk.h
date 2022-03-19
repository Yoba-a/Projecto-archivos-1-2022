#ifndef OBMKDISK_H
#define OBMKDISK_H
#include <QString>
#include "string"
#include "structs.h"
#include "fdisk.h"
using namespace std;


class obmkdisk
{

public:
    obmkdisk();

    int size;
    QString line;
    QString fechacreacion;
    QString unity;
    QString fit;
    bool sizeNegativo = false;
    void mostrardatos(obmkdisk *disco);
    void eliminarDisco(obmkdisk *disco);
    bool file_exists (QString path);
    QString obtainPath(QString path);
    QString obtainAbsPath(QString path);
    bool findConfigFile(QString path);
    int sizeOfDisk(int size , QString unity);
    string currentDateTime ();
    void insertar_mbr(mbr *mbr_disk,QString abPath);
    void insertar_ebr(ebr *mbr_disk,QString abPath,int inicio);

};

#endif // OBMKDISK_H
