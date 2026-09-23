#include "database.h"
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QDebug>

Database* Database::m_instance = nullptr;

Database::Database(QObject *parent) : QObject(parent) {}

Database* Database::instance()
{
    if (!m_instance) {
        m_instance = new Database();
    }
    return m_instance;
}

bool Database::initialize()
{
    // 使用 SQLite 驱动
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("student_system.db");

    if (!m_db.open()) {
        qDebug() << "数据库打开失败:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功!";
    if (!createTables()) {
        return false;
    }
    insertDefaultUsers();
    return true;
}

bool Database::createTables()
{
    QSqlQuery query(m_db);

    // 用户表
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT UNIQUE NOT NULL,"
        "  password TEXT NOT NULL,"
        "  role TEXT NOT NULL DEFAULT 'user'"
        ")"
    );
    if (!ok) {
        qDebug() << "创建用户表失败:" << query.lastError().text();
        return false;
    }

    // 学生表
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS students ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  student_id TEXT UNIQUE NOT NULL,"
        "  name TEXT NOT NULL,"
        "  gender TEXT NOT NULL,"
        "  age INTEGER NOT NULL,"
        "  class_name TEXT NOT NULL,"
        "  phone TEXT,"
        "  email TEXT,"
        "  address TEXT,"
        "  enrollment_date TEXT"
        ")"
    );
    if (!ok) {
        qDebug() << "创建学生表失败:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Database::insertDefaultUsers()
{
    QSqlQuery query(m_db);
    // 检查是否已有用户
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() > 0) {
        return true; // 已有用户，不重复插入
    }

    // 插入默认管理员和普通用户
    query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");

    query.addBindValue("admin");
    query.addBindValue("admin123");
    query.addBindValue("admin");
    if (!query.exec()) {
        qDebug() << "插入默认管理员失败:" << query.lastError().text();
    }

    query.addBindValue("user");
    query.addBindValue("user123");
    query.addBindValue("user");
    if (!query.exec()) {
        qDebug() << "插入默认用户失败:" << query.lastError().text();
    }

    return true;
}

// ==================== 用户管理 ====================
bool Database::verifyLogin(const QString &username, const QString &password, QString &role)
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.prepare("SELECT role FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (query.exec() && query.next()) {
        role = query.value(0).toString();
        return true;
    }
    return false;
}

bool Database::addUser(const QString &username, const QString &password, const QString &role)
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(role);
    return query.exec();
}

bool Database::deleteUser(int userId)
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(userId);
    return query.exec();
}

QList<UserInfo> Database::getAllUsers()
{
    QMutexLocker locker(&m_mutex);
    QList<UserInfo> list;
    QSqlQuery query(m_db);
    query.exec("SELECT id, username, password, role FROM users ORDER BY id");
    while (query.next()) {
        UserInfo u;
        u.id = query.value(0).toInt();
        u.username = query.value(1).toString();
        u.password = query.value(2).toString();
        u.role = query.value(3).toString();
        list.append(u);
    }
    return list;
}

bool Database::userExists(const QString &username)
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

// ==================== 学生CRUD ====================
bool Database::addStudent(const StudentInfo &stu)
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO students (student_id, name, gender, age, class_name, phone, email, address, enrollment_date) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(stu.studentId);
    query.addBindValue(stu.name);
    query.addBindValue(stu.gender);
    query.addBindValue(stu.age);
    query.addBindValue(stu.className);
    query.addBindValue(stu.phone);
    query.addBindValue(stu.email);
    query.addBindValue(stu.address);
    query.addBindValue(stu.enrollmentDate);

    if (!query.exec()) {
        qDebug() << "添加学生失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::updateStudent(const StudentInfo &stu)
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE students SET student_id=?, name=?, gender=?, age=?, class_name=?, "
        "phone=?, email=?, address=?, enrollment_date=? WHERE id=?"
    );
    query.addBindValue(stu.studentId);
    query.addBindValue(stu.name);
    query.addBindValue(stu.gender);
    query.addBindValue(stu.age);
    query.addBindValue(stu.className);
    query.addBindValue(stu.phone);
    query.addBindValue(stu.email);
    query.addBindValue(stu.address);
    query.addBindValue(stu.enrollmentDate);
    query.addBindValue(stu.id);

    if (!query.exec()) {
        qDebug() << "更新学生失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::deleteStudent(int id)
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM students WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

QList<StudentInfo> Database::getAllStudents()
{
    QMutexLocker locker(&m_mutex);
    QList<StudentInfo> list;
    QSqlQuery query(m_db);
    query.exec("SELECT id, student_id, name, gender, age, class_name, phone, email, address, enrollment_date "
               "FROM students ORDER BY id");
    while (query.next()) {
        StudentInfo s;
        s.id = query.value(0).toInt();
        s.studentId = query.value(1).toString();
        s.name = query.value(2).toString();
        s.gender = query.value(3).toString();
        s.age = query.value(4).toInt();
        s.className = query.value(5).toString();
        s.phone = query.value(6).toString();
        s.email = query.value(7).toString();
        s.address = query.value(8).toString();
        s.enrollmentDate = query.value(9).toString();
        list.append(s);
    }
    return list;
}

QList<StudentInfo> Database::searchStudents(const QString &keyword)
{
    QMutexLocker locker(&m_mutex);
    QList<StudentInfo> list;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, student_id, name, gender, age, class_name, phone, email, address, enrollment_date "
        "FROM students WHERE student_id LIKE ? OR name LIKE ? OR class_name LIKE ? OR phone LIKE ? "
        "ORDER BY id"
    );
    QString like = "%" + keyword + "%";
    query.addBindValue(like);
    query.addBindValue(like);
    query.addBindValue(like);
    query.addBindValue(like);

    if (query.exec()) {
        while (query.next()) {
            StudentInfo s;
            s.id = query.value(0).toInt();
            s.studentId = query.value(1).toString();
            s.name = query.value(2).toString();
            s.gender = query.value(3).toString();
            s.age = query.value(4).toInt();
            s.className = query.value(5).toString();
            s.phone = query.value(6).toString();
            s.email = query.value(7).toString();
            s.address = query.value(8).toString();
            s.enrollmentDate = query.value(9).toString();
            list.append(s);
        }
    }
    return list;
}

StudentInfo Database::getStudentById(int id)
{
    QMutexLocker locker(&m_mutex);
    StudentInfo s;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, student_id, name, gender, age, class_name, phone, email, address, enrollment_date "
                  "FROM students WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        s.id = query.value(0).toInt();
        s.studentId = query.value(1).toString();
        s.name = query.value(2).toString();
        s.gender = query.value(3).toString();
        s.age = query.value(4).toInt();
        s.className = query.value(5).toString();
        s.phone = query.value(6).toString();
        s.email = query.value(7).toString();
        s.address = query.value(8).toString();
        s.enrollmentDate = query.value(9).toString();
    }
    return s;
}

bool Database::studentIdExists(const QString &studentId)
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM students WHERE student_id = ?");
    query.addBindValue(studentId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

// ==================== 统计 ====================
QMap<int, int> Database::getAgeDistribution()
{
    QMutexLocker locker(&m_mutex);
    QMap<int, int> dist;
    QSqlQuery query(m_db);
    query.exec("SELECT age, COUNT(*) FROM students GROUP BY age ORDER BY age");
    while (query.next()) {
        dist[query.value(0).toInt()] = query.value(1).toInt();
    }
    return dist;
}

QMap<QString, int> Database::getGenderDistribution()
{
    QMutexLocker locker(&m_mutex);
    QMap<QString, int> dist;
    QSqlQuery query(m_db);
    query.exec("SELECT gender, COUNT(*) FROM students GROUP BY gender");
    while (query.next()) {
        dist[query.value(0).toString()] = query.value(1).toInt();
    }
    return dist;
}

int Database::getTotalCount()
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM students");
    if (query.next()) return query.value(0).toInt();
    return 0;
}

double Database::getAvgAge()
{
    QMutexLocker locker(&m_mutex);
    QSqlQuery query(m_db);
    query.exec("SELECT AVG(age) FROM students");
    if (query.next()) return query.value(0).toDouble();
    return 0.0;
}

// ==================== 备份还原 ====================
bool Database::backupDatabase(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);
    // 先关闭当前连接，复制文件
    QString dbPath = QDir::currentPath() + "/student_system.db";
    m_db.close();

    bool ok = QFile::copy(dbPath, filePath);

    // 重新打开
    m_db.open();
    return ok;
}

bool Database::restoreDatabase(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);
    m_db.close();

    QString dbPath = QDir::currentPath() + "/student_system.db";
    QFile::remove(dbPath);
    bool ok = QFile::copy(filePath, dbPath);

    m_db.open();
    return ok;
}
