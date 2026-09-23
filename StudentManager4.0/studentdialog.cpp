#include "studentdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGroupBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QLabel>

StudentDialog::StudentDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void StudentDialog::setupUI()
{
    setWindowTitle("学生信息");
    setFixedSize(600, 680);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(28, 24, 28, 24);
    mainLayout->setSpacing(16);

    // ---- 标题 ----
    QLabel *titleLabel = new QLabel("学生信息编辑");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #1F2937; padding: 4px 0 12px 0;");
    mainLayout->addWidget(titleLabel);

    // ---- 辅助: 创建一行「标签 + 输入控件」----
    auto createRow = [](const QString &labelText, QWidget *input, QWidget *parent) -> QHBoxLayout* {
        QHBoxLayout *row = new QHBoxLayout;
        row->setSpacing(14);
        QLabel *label = new QLabel(labelText, parent);
        label->setFixedSize(90, 42);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setStyleSheet("font-size: 15px; font-weight: bold; color: #374151;");
        row->addWidget(label);
        row->addWidget(input, 1);
        return row;
    };

    // ========== 基本信息 ==========
    QGroupBox *basicGroup = new QGroupBox("基本信息");
    QVBoxLayout *basicLayout = new QVBoxLayout(basicGroup);
    basicLayout->setSpacing(14);
    basicLayout->setContentsMargins(18, 20, 18, 18);

    m_studentIdEdit = new QLineEdit;
    m_studentIdEdit->setPlaceholderText("例如: 2024001");
    m_studentIdEdit->setFixedHeight(42);
    m_studentIdEdit->setStyleSheet("font-size: 15px;");
    basicLayout->addLayout(createRow("学号:", m_studentIdEdit, basicGroup));

    m_nameEdit = new QLineEdit;
    m_nameEdit->setPlaceholderText("请输入姓名");
    m_nameEdit->setFixedHeight(42);
    m_nameEdit->setStyleSheet("font-size: 15px;");
    basicLayout->addLayout(createRow("姓名:", m_nameEdit, basicGroup));

    m_genderCombo = new QComboBox;
    m_genderCombo->addItems({"男", "女"});
    m_genderCombo->setFixedHeight(42);
    m_genderCombo->setStyleSheet("font-size: 15px;");
    basicLayout->addLayout(createRow("性别:", m_genderCombo, basicGroup));

    m_ageSpin = new QSpinBox;
    m_ageSpin->setRange(15, 30);
    m_ageSpin->setValue(20);
    m_ageSpin->setSuffix(" 岁");
    m_ageSpin->setFixedHeight(42);
    m_ageSpin->setStyleSheet("font-size: 15px;");
    basicLayout->addLayout(createRow("年龄:", m_ageSpin, basicGroup));

    m_classEdit = new QLineEdit;
    m_classEdit->setPlaceholderText("例如: 计算机科学2024级1班");
    m_classEdit->setFixedHeight(42);
    m_classEdit->setStyleSheet("font-size: 15px;");
    basicLayout->addLayout(createRow("班级:", m_classEdit, basicGroup));

    mainLayout->addWidget(basicGroup);

    // ========== 联系方式 ==========
    QGroupBox *contactGroup = new QGroupBox("联系方式");
    QVBoxLayout *contactLayout = new QVBoxLayout(contactGroup);
    contactLayout->setSpacing(14);
    contactLayout->setContentsMargins(18, 20, 18, 18);

    m_phoneEdit = new QLineEdit;
    m_phoneEdit->setPlaceholderText("例如: 13800138000");
    m_phoneEdit->setFixedHeight(42);
    m_phoneEdit->setStyleSheet("font-size: 15px;");
    QRegularExpression phoneReg("^1[3-9]\\d{9}$");
    m_phoneEdit->setValidator(new QRegularExpressionValidator(phoneReg, this));
    contactLayout->addLayout(createRow("电话:", m_phoneEdit, contactGroup));

    m_emailEdit = new QLineEdit;
    m_emailEdit->setPlaceholderText("例如: student@school.edu.cn");
    m_emailEdit->setFixedHeight(42);
    m_emailEdit->setStyleSheet("font-size: 15px;");
    contactLayout->addLayout(createRow("邮箱:", m_emailEdit, contactGroup));

    m_addressEdit = new QLineEdit;
    m_addressEdit->setPlaceholderText("请输入地址");
    m_addressEdit->setFixedHeight(42);
    m_addressEdit->setStyleSheet("font-size: 15px;");
    contactLayout->addLayout(createRow("地址:", m_addressEdit, contactGroup));

    m_enrollmentDateEdit = new QDateEdit(QDate::currentDate());
    m_enrollmentDateEdit->setCalendarPopup(true);
    m_enrollmentDateEdit->setDisplayFormat("yyyy-MM-dd");
    m_enrollmentDateEdit->setFixedHeight(42);
    m_enrollmentDateEdit->setStyleSheet("font-size: 15px;");
    contactLayout->addLayout(createRow("入学日期:", m_enrollmentDateEdit, contactGroup));

    mainLayout->addWidget(contactGroup);

    // ========== 按钮 ==========
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(18);

    QPushButton *cancelBtn = new QPushButton("取消");
    cancelBtn->setFixedHeight(46);
    cancelBtn->setStyleSheet(
        "QPushButton { background-color: #E5E7EB; color: #374151; font-size: 15px;"
        "font-weight: bold; border-radius: 8px; padding: 10px 32px; }"
        "QPushButton:hover { background-color: #D1D5DB; }"
    );

    QPushButton *submitBtn = new QPushButton("保存");
    submitBtn->setFixedHeight(46);
    submitBtn->setObjectName("btnExport");
    submitBtn->setStyleSheet(
        "QPushButton { font-size: 15px; font-weight: bold; padding: 10px 36px; }"
    );

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(submitBtn);
    mainLayout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(submitBtn, &QPushButton::clicked, this, &StudentDialog::onSubmit);
}

void StudentDialog::setStudent(const StudentInfo &stu)
{
    m_originalStu = stu;
    m_isEditMode = (stu.id > 0);

    setWindowTitle(m_isEditMode ? "编辑学生信息" : "添加学生信息");

    m_studentIdEdit->setText(stu.studentId);
    m_nameEdit->setText(stu.name);

    if (stu.gender == "女") {
        m_genderCombo->setCurrentIndex(1);
    } else {
        m_genderCombo->setCurrentIndex(0);
    }

    m_ageSpin->setValue(stu.age > 0 ? stu.age : 20);
    m_classEdit->setText(stu.className);
    m_phoneEdit->setText(stu.phone);
    m_emailEdit->setText(stu.email);
    m_addressEdit->setText(stu.address);

    if (!stu.enrollmentDate.isEmpty()) {
        m_enrollmentDateEdit->setDate(QDate::fromString(stu.enrollmentDate, "yyyy-MM-dd"));
    }
}

StudentInfo StudentDialog::getStudent() const
{
    StudentInfo stu = m_originalStu;
    stu.studentId = m_studentIdEdit->text().trimmed();
    stu.name = m_nameEdit->text().trimmed();
    stu.gender = m_genderCombo->currentText();
    stu.age = m_ageSpin->value();
    stu.className = m_classEdit->text().trimmed();
    stu.phone = m_phoneEdit->text().trimmed();
    stu.email = m_emailEdit->text().trimmed();
    stu.address = m_addressEdit->text().trimmed();
    stu.enrollmentDate = m_enrollmentDateEdit->date().toString("yyyy-MM-dd");
    return stu;
}

bool StudentDialog::validateInput()
{
    if (m_studentIdEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "验证失败", "学号不能为空!");
        m_studentIdEdit->setFocus();
        return false;
    }
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "验证失败", "姓名不能为空!");
        m_nameEdit->setFocus();
        return false;
    }
    if (m_classEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "验证失败", "班级不能为空!");
        m_classEdit->setFocus();
        return false;
    }
    return true;
}

void StudentDialog::onSubmit()
{
    if (!validateInput()) return;
    accept();
}
