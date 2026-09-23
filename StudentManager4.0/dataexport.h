#ifndef DATAEXPORT_H
#define DATAEXPORT_H

#include <QString>
#include <QList>
#include "database.h"

// 同步导出：返回 true 表示成功，errorMsg 仅在失败时有内容
bool exportStudentsToCsv(const QString &filePath, const QList<StudentInfo> &data, QString &errorMsg);

// 同步导入：返回成功导入的记录数，失败返回 -1；skipped 为因重复学号跳过的条数
int importStudentsFromCsv(const QString &filePath, QString &errorMsg, int &skipped);

#endif // DATAEXPORT_H
