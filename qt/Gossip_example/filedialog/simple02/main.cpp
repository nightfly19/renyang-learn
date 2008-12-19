#include <qapplication.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QFileDialog* fd = new QFileDialog(0, "file dialog", TRUE);

    fd->setDir("~/");     // 工作目錄
    fd->setShowHiddenFiles(true);  // 顯示隱藏檔案
    fd->setMode(QFileDialog::ExistingFile);  // 顯示目前存在的檔案
    fd->setFilter("Images (*.png *.xpm *.jpg)");  // 副檔名過濾
    fd->setViewMode(QFileDialog::Detail);  // 顯示詳細資訊

    QString fileName;
    if (fd->exec() == QDialog::Accepted) {
        fileName = fd->selectedFile();
        QMessageBox::information(0, "Get Selected File",
             fileName, "OK", "", "", 0, 1);
    }

    return 0;
}
