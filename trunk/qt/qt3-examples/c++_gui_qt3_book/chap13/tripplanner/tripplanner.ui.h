void TripPlanner::init()
{
    connect(&socket, SIGNAL(connected()),
            this, SLOT(sendRequest()));
    connect(&socket, SIGNAL(connectionClosed()),
            this, SLOT(connectionClosedByServer()));
    connect(&socket, SIGNAL(readyRead()),
            this, SLOT(updateListView()));
    connect(&socket, SIGNAL(error(int)),
            this, SLOT(error(int)));

    connect(&connectionTimer, SIGNAL(timeout()),
            this, SLOT(connectionTimeout()));
    connect(&progressBarTimer, SIGNAL(timeout()),
            this, SLOT(advanceProgressBar()));

    QDateTime dateTime = QDateTime::currentDateTime();
    dateEdit->setDate(dateTime.date());
    timeEdit->setTime(QTime(dateTime.time().hour(), 0));
}

void TripPlanner::advanceProgressBar()
{
    progressBar->setProgress(progressBar->progress() + 2);
}

void TripPlanner::connectToServer()
{
    listView->clear();

#if 1
    socket.connectToHost("localhost", 6178);
#else
    socket.connectToHost("tripserver.zugbahn.de", 6178);
#endif

    searchButton->setEnabled(false);
    stopButton->setEnabled(true);
    statusLabel->setText(tr("Connecting to server..."));

    connectionTimer.start(30 * 1000, true);
    progressBarTimer.start(200, false);

    blockSize = 0;
}

void TripPlanner::sendRequest()
{
    QByteArray block;
    QDataStream out(block, IO_WriteOnly);
    out.setVersion(5);
    out << (Q_UINT16)0 << (Q_UINT8)'S'
        << fromComboBox->currentText()
        << toComboBox->currentText() << dateEdit->date()
        << timeEdit->time();
    if (departureRadioButton->isOn())
        out << (Q_UINT8)'D';
    else
        out << (Q_UINT8)'A';
    out.device()->at(0);
    out << (Q_UINT16)(block.size() - sizeof(Q_UINT16));
    socket.writeBlock(block.data(), block.size());

    statusLabel->setText(tr("Sending request..."));
}

void TripPlanner::updateListView()
{
    connectionTimer.start(30 * 1000, true);

    QDataStream in(&socket);
    in.setVersion(5);

    for (;;) {
        if (blockSize == 0) {
            if (socket.bytesAvailable() < sizeof(Q_UINT16))
                break;
            in >> blockSize;
        }

        if (blockSize == 0xFFFF) {
            closeConnection();
            statusLabel->setText(tr("Found %1 trip(s)")
                                 .arg(listView->childCount()));
            break;
        }

        if (socket.bytesAvailable() < blockSize)
            break;

        QDate date;
        QTime departureTime;
        QTime arrivalTime;
        Q_UINT16 duration;
        Q_UINT8 changes;
        QString trainType;

        in >> date >> departureTime >> duration >> changes
           >> trainType;
        arrivalTime = departureTime.addSecs(duration * 60);

        new QListViewItem(listView,
                          date.toString(LocalDate),
                          departureTime.toString(tr("hh:mm")),
                          arrivalTime.toString(tr("hh:mm")),
                          tr("%1 hr %2 min").arg(duration / 60)
                                            .arg(duration % 60),
                          QString::number(changes),
                          trainType);
        blockSize = 0;
    }
}

void TripPlanner::closeConnection()
{
    socket.close();
    searchButton->setEnabled(true);
    stopButton->setEnabled(false);
    connectionTimer.stop();
    progressBarTimer.stop();
    progressBar->setProgress(0);
}

void TripPlanner::stopSearch()
{
    statusLabel->setText(tr("Search stopped"));
    closeConnection();
}

void TripPlanner::connectionTimeout()
{
    statusLabel->setText(tr("Error: Connection timed out"));
    closeConnection();
}

void TripPlanner::connectionClosedByServer()
{
    if (blockSize != 0xFFFF)
        statusLabel->setText(tr("Error: Connection closed by "
                                "server"));
    closeConnection();
}

void TripPlanner::error(int code)
{
    QString message;

    switch (code) {
    case QSocket::ErrConnectionRefused:
        message = tr("Error: Connection refused");
        break;
    case QSocket::ErrHostNotFound:
        message = tr("Error: Server not found");
        break;
    case QSocket::ErrSocketRead:
    default:
        message = tr("Error: Data transfer failed");
    }
    statusLabel->setText(message);
    closeConnection();
}
