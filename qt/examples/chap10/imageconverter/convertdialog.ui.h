void ConvertDialog::init()
{
    process = 0;
    QStringList imageFormats = QImage::outputFormatList();
    targetFormatComboBox->insertStringList(imageFormats);
    fileFilters = tr("Images") + " (*."
                  + imageFormats.join(" *.").lower() + ")";
}

void ConvertDialog::browse()
{
    QString initialName = sourceFileEdit->text();
    if (initialName.isEmpty())
        initialName = QDir::homeDirPath();
    QString fileName =
            QFileDialog::getOpenFileName(initialName, fileFilters,
                                         this);
    fileName = QDir::convertSeparators(fileName);
    if (!fileName.isEmpty()) {
        sourceFileEdit->setText(fileName);
        convertButton->setEnabled(true);
    }
}

void ConvertDialog::convert()
{
    QString sourceFile = sourceFileEdit->text();
    targetFile = QFileInfo(sourceFile).dirPath() + QDir::separator()
                 + QFileInfo(sourceFile).baseName();
    targetFile += ".";
    targetFile += targetFormatComboBox->currentText().lower();
    convertButton->setEnabled(false);
    outputTextEdit->clear();

    process = new QProcess(this);
    process->addArgument("convert");
    if (enhanceCheckBox->isChecked())
        process->addArgument("-enhance");
    if (monochromeCheckBox->isChecked())
        process->addArgument("-monochrome");
    process->addArgument(sourceFile);
    process->addArgument(targetFile);
    connect(process, SIGNAL(readyReadStderr()),
            this, SLOT(updateOutputTextEdit()));
    connect(process, SIGNAL(processExited()),
            this, SLOT(processExited()));
    process->start();
}

void ConvertDialog::updateOutputTextEdit()
{
    QByteArray data = process->readStderr();
    QString text = outputTextEdit->text() + QString(data);
    outputTextEdit->setText(text);
}

void ConvertDialog::processExited()
{
    if (process->normalExit()) {
        outputTextEdit->append(tr("File %1 created")
                               .arg(targetFile));
    } else {
        outputTextEdit->append(tr("Conversion failed"));
    }
    delete process;
    process = 0;
    convertButton->setEnabled(true);
}
