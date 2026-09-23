#include "dataexport.h"
#include <QFile>
#include <QApplication>

// 简单的 CSV 行解析器（支持引号包裹的字段）
static QStringList parseCsvLine(const QString &line)
{
    QStringList fields;
    QString field;
    bool inQuotes = false;
    int len = line.length();

    for (int i = 0; i < len; i++) {
        QChar ch = line[i];
        if (inQuotes) {
            if (ch == '"') {
                if (i + 1 < len && line[i + 1] == '"') {
                    field += '"';
                    i++;
                } else {
                    inQuotes = false;
                }
            } else {
                field += ch;
            }
        } else {
            if (ch == '"') {
                inQuotes = true;
            } else if (ch == ',') {
                fields.append(field.trimmed());
                field.clear();
            } else {
                field += ch;
            }
        }
    }
    fields.append(field.trimmed());
    return fields;
}

// ==================== 同步导出 ====================
bool exportStudentsToCsv(const QString &filePath, const QList<StudentInfo> &data, QString &errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        errorMsg = QString("无法创建文件: %1").arg(filePath);
        return false;
    }

    // UTF-8 BOM（3 字节，Excel 靠它正确识别中文）
    file.write("\xEF\xBB\xBF");

    // 写表头
    file.write(QStringLiteral("序号,学号,姓名,性别,年龄,班级,电话,邮箱,地址,入学日期\n").toUtf8());

    auto escapeCsv = [](const QString &field) -> QString {
        if (field.contains(',') || field.contains('"') || field.contains('\n')) {
            QString escaped = field;
            escaped.replace("\"", "\"\"");
            return "\"" + escaped + "\"";
        }
        return field;
    };

    int total = data.size();
    for (int i = 0; i < total; i++) {
        const StudentInfo &stu = data[i];

        QString line = QStringLiteral("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10\n")
            .arg(i + 1)
            .arg(escapeCsv(stu.studentId))
            .arg(escapeCsv(stu.name))
            .arg(escapeCsv(stu.gender))
            .arg(stu.age)
            .arg(escapeCsv(stu.className))
            .arg(escapeCsv(stu.phone))
            .arg(escapeCsv(stu.email))
            .arg(escapeCsv(stu.address))
            .arg(escapeCsv(stu.enrollmentDate));

        file.write(line.toUtf8());
    }

    file.close();
    return true;
}

// ==================== 同步导入 ====================
int importStudentsFromCsv(const QString &filePath, QString &errorMsg, int &skipped)
{
    skipped = 0;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        errorMsg = QString("无法打开文件: %1").arg(filePath);
        return -1;
    }

    QByteArray rawData = file.readAll();
    file.close();

    // 去掉 BOM
    if (rawData.startsWith("\xEF\xBB\xBF")) {
        rawData = rawData.mid(3);
    }

    QString content = QString::fromUtf8(rawData);
    QStringList lines = content.split('\n');

    if (lines.isEmpty()) {
        errorMsg = "文件为空";
        return -1;
    }

    // 解析表头，找到各列的位置（兼容"序号"和非序号两种格式）
    QStringList header = parseCsvLine(lines[0].trimmed());
    auto findCol = [&](const char *name) -> int {
        for (int i = 0; i < header.size(); i++) {
            if (header[i].contains(QString::fromUtf8(name))) return i;
        }
        return -1;
    };

    int idxId       = findCol("学号");    // 必须
    int idxName     = findCol("姓名");    // 必须
    int idxGender   = findCol("性别");    // 必须
    int idxAge      = findCol("年龄");    // 必须
    int idxClass    = findCol("班级");    // 必须
    int idxPhone    = findCol("电话");    // 必须
    int idxEmail    = findCol("邮箱");    // 必须
    int idxAddr     = findCol("地址");    // 必须
    int idxEnroll   = findCol("入学日期"); // 可选

    if (idxId < 0 || idxName < 0 || idxGender < 0 || idxAge < 0 ||
        idxClass < 0 || idxPhone < 0 || idxEmail < 0 || idxAddr < 0) {
        errorMsg = "CSV 表头缺少必要列（学号/姓名/性别/年龄/班级/电话/邮箱/地址）";
        return -1;
    }

    int imported = 0;
    for (int i = 1; i < lines.size(); i++) {
        QString line = lines[i].trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = parseCsvLine(line);
        // 至少要有所有必需列的数据
        int minSize = qMax(idxId, qMax(idxName, qMax(idxGender,
                      qMax(idxAge, qMax(idxClass, qMax(idxPhone,
                      qMax(idxEmail, idxAddr)))))) + 1);
        if (fields.size() < minSize) continue;

        StudentInfo stu;
        stu.studentId  = fields[idxId].trimmed();
        stu.name       = fields[idxName].trimmed();
        stu.gender     = fields[idxGender].trimmed();
        stu.age        = fields[idxAge].trimmed().toInt();
        stu.className  = fields[idxClass].trimmed();
        stu.phone      = fields[idxPhone].trimmed();
        stu.email      = fields[idxEmail].trimmed();
        stu.address    = fields[idxAddr].trimmed();
        stu.enrollmentDate = (idxEnroll >= 0) ? fields[idxEnroll].trimmed() : QString();

        if (Database::instance()->studentIdExists(stu.studentId)) {
            skipped++;
        } else if (Database::instance()->addStudent(stu)) {
            imported++;
        }

        // 每 10 条记录让 UI 喘口气
        if (i % 10 == 0) {
            QApplication::processEvents();
        }
    }

    return imported;
}
