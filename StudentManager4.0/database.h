#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>
#include <QList>
#include <QMutex>

// 学生数据结构体
struct StudentInfo {
    int id = 0;               // 数据库主键
    QString studentId;        // 学号
    QString name;             // 姓名
    QString gender;           // 性别
    int age = 0;              // 年龄
    QString className;        // 班级
    QString phone;            // 电话
    QString email;            // 邮箱
    QString address;          // 地址
    QString enrollmentDate;   // 入学日期

    QVariantMap toMap() const {
        return {
            {"id", id},
            {"studentId", studentId},
            {"name", name},
            {"gender", gender},
            {"age", age},
            {"className", className},
            {"phone", phone},
            {"email", email},
            {"address", address},
            {"enrollmentDate", enrollmentDate}
        };
    }
};

// 用户数据结构体
struct UserInfo {
    int id = 0;
    QString username;
    QString password;
    QString role; // "admin" 或 "user"
};

class Database : public QObject
{
    Q_OBJECT
public:
    static Database* instance();
    bool initialize();

    // ---- 用户管理 ----
    bool verifyLogin(const QString &username, const QString &password, QString &role);
    bool addUser(const QString &username, const QString &password, const QString &role);
    bool deleteUser(int userId);
    QList<UserInfo> getAllUsers();
    bool userExists(const QString &username);

    // ---- 学生CRUD ----
    bool addStudent(const StudentInfo &stu);
    bool updateStudent(const StudentInfo &stu);
    bool deleteStudent(int id);
    QList<StudentInfo> getAllStudents();
    QList<StudentInfo> searchStudents(const QString &keyword);
    StudentInfo getStudentById(int id);
    bool studentIdExists(const QString &studentId);

    // ---- 统计 ----
    QMap<int, int> getAgeDistribution();        // 年龄 -> 人数
    QMap<QString, int> getGenderDistribution(); // 性别 -> 人数
    int getTotalCount();
    double getAvgAge();

    // ---- 数据库备份还原 ----
    bool backupDatabase(const QString &filePath);
    bool restoreDatabase(const QString &filePath);

private:
    explicit Database(QObject *parent = nullptr);
    static Database *m_instance;
    QSqlDatabase m_db;
    QMutex m_mutex;

    bool createTables();
    bool insertDefaultUsers();
};

#endif // DATABASE_H
