#ifndef MOUNT_H
#define MOUNT_H
#include <QCoreApplication>
#include <QString>
#include <QtCore/qglobal.h>
#include "obmkdisk.h"
#include "structs.h"


class mount
{

public:


    mount();

    QString path;
    QString name_partition;
     void resultado_mount(mount *f_mount);
    vector<char> alfabeto = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                                  'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

};

#endif // MOUNT_H
