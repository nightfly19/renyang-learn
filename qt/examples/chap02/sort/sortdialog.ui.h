void SortDialog::init()
{
    secondaryGroupBox->hide();
    tertiaryGroupBox->hide();
    setColumnRange('A', 'Z');
}

void SortDialog::setColumnRange(QChar first, QChar last)
{
    primaryColumnCombo->clear();
    secondaryColumnCombo->clear();
    tertiaryColumnCombo->clear();

    secondaryColumnCombo->insertItem(tr("None"));
    tertiaryColumnCombo->insertItem(tr("None"));

    primaryColumnCombo->setMinimumSize(
            secondaryColumnCombo->sizeHint());

    QChar ch = first;
    while (ch <= last) {
        primaryColumnCombo->insertItem(ch);
        secondaryColumnCombo->insertItem(ch);
        tertiaryColumnCombo->insertItem(ch);
        ch = ch.unicode() + 1;
    }
}
