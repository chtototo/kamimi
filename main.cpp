#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QByteArray>
#include <QFont>
#include <QTextEdit>
#include <QFrame>

QByteArray xorCipher(QByteArray data, const QByteArray &key) {
    for (int i = 0; i < data.size(); ++i) {
        data[i] ^= key[i % key.size()];
    }
    return data;
}

bool writeFile(const QString &filePath, const QByteArray &data) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(data);
    file.close();
    return true;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("XOR Cipher");
    window.setFixedSize(1200, 600);

    QFont titleFont("Segoe UI", 24, QFont::Bold);
    QFont labelFont("Segoe UI", 10);

    QLabel *titleLabel = new QLabel("XOR Cipher");
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    QLineEdit *fileEdit = new QLineEdit;
    QPushButton *fileBtn = new QPushButton("📁");

    QLineEdit *keyEdit = new QLineEdit;
    QPushButton *keyBtn = new QPushButton("📁");

    QLineEdit *outEdit = new QLineEdit;
    QPushButton *outBtn = new QPushButton("📁");

    QPushButton *startBtn = new QPushButton("🔐 Encrypt / Decrypt");

    fileEdit->setPlaceholderText("Select file...");
    keyEdit->setPlaceholderText("Select key file...");
    outEdit->setPlaceholderText("Select output file...");

    fileEdit->setReadOnly(true);
    keyEdit->setReadOnly(true);
    outEdit->setReadOnly(true);

    fileEdit->setMinimumHeight(30);
    keyEdit->setMinimumHeight(30);
    outEdit->setMinimumHeight(30);
    fileBtn->setFixedWidth(40);
    keyBtn->setFixedWidth(40);
    outBtn->setFixedWidth(40);
    startBtn->setMinimumHeight(40);

    QTextEdit *fileContent = new QTextEdit;
    QTextEdit *keyContent = new QTextEdit;
    QTextEdit *outContent = new QTextEdit;
    outContent->setReadOnly(true);

    // fileContent->setFont(QFont("Segoe UI", 14));
    // keyContent->setFont(QFont("Segoe UI", 14));
    // outContent->setFont(QFont("Segoe UI", 14));

    fileContent->setFrameShape(QFrame::Box);
    keyContent->setFrameShape(QFrame::Box);
    outContent->setFrameShape(QFrame::Box);

    fileContent->setMinimumHeight(80);
    keyContent->setMinimumHeight(80);
    outContent->setMinimumHeight(80);

    QHBoxLayout *mainLayout = new QHBoxLayout(&window);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(titleLabel);
    leftLayout->addSpacing(15);

    auto addFileRow = [&](QLineEdit *edit, QPushButton *btn) {
        QHBoxLayout *row = new QHBoxLayout;
        row->addWidget(edit);
        row->addWidget(btn);
        leftLayout->addLayout(row);
        leftLayout->addSpacing(10);
    };

    addFileRow(fileEdit, fileBtn);
    addFileRow(keyEdit, keyBtn);
    addFileRow(outEdit, outBtn);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(startBtn);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(new QLabel("File Content:"));
    rightLayout->addWidget(fileContent);
    rightLayout->addWidget(new QLabel("Key Content:"));
    rightLayout->addWidget(keyContent);
    rightLayout->addWidget(new QLabel("Output Content:"));
    rightLayout->addWidget(outContent);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 2);

    QString filePath, keyPath, outPath;

    QObject::connect(fileBtn, &QPushButton::clicked, [&]() {
        filePath = QFileDialog::getOpenFileName(nullptr, "Select File");
        if (!filePath.isEmpty()) {
            fileEdit->setText(filePath);
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray data = file.readAll();
                fileContent->setPlainText(QString::fromUtf8(data));
                file.close();
            }
        }
    });

    QObject::connect(keyBtn, &QPushButton::clicked, [&]() {
        keyPath = QFileDialog::getOpenFileName(nullptr, "Select Key File");
        if (!keyPath.isEmpty()) {
            keyEdit->setText(keyPath);
            QFile file(keyPath);
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray data = file.readAll();
                keyContent->setPlainText(QString::fromUtf8(data));
                file.close();
            }
        }
    });

    QObject::connect(outBtn, &QPushButton::clicked, [&]() {
        outPath = QFileDialog::getSaveFileName(nullptr, "Select Output File");
        if (!outPath.isEmpty()) {
            outEdit->setText(outPath);
            QFile file(outPath);
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray data = file.readAll();
                outContent->setPlainText(QString::fromUtf8(data));
                file.close();
            } else {
                outContent->clear();
            }
        }
    });

    QObject::connect(startBtn, &QPushButton::clicked, [&]() {
        QByteArray inputData = fileContent->toPlainText().toUtf8();
        QByteArray keyData = keyContent->toPlainText().toUtf8();

        if (inputData.isEmpty()) {
            QMessageBox::critical(nullptr, "Error", "Input data is empty. Please provide a valid input.");
            return;
        }

        if (keyData.isEmpty()) {
            QMessageBox::critical(nullptr, "Error", "Key is empty. Please provide a valid key.");
            return;
        }

        QByteArray result = xorCipher(inputData, keyData);

        if (!outPath.isEmpty()) {
            if (writeFile(outPath, result)) {
                outContent->setPlainText(QString::fromUtf8(result));
                QMessageBox::information(nullptr, "Done", "Encryption complete.");
            } else {
                QMessageBox::critical(nullptr, "Error", "Failed to save output file.");
            }
        } else {
            outContent->setPlainText(QString::fromUtf8(result));
            QMessageBox::information(nullptr, "Done", "Encryption complete (output not saved to file).");
        }
    });


    window.show();
    return app.exec();
}
