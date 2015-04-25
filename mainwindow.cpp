#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#ifdef Q_OS_WIN
#include <QDir>
#include <qt_windows.h>
#endif
#include <QDateTime>
#include <QMap>
#include <QXmlStreamReader>
#include <QtEndian>
#include <QtMath>
#include <QMessageBox>
#include <QProgressDialog>
#include <QClipboard>

qint64 first_sector = 0, last_sector = 0;

int bytes_per_sector;
qint64 root_directory_sector;

QString selectedDrive;

union timestamp {
    quint32 time;
    struct {
        unsigned int second : 5;
        unsigned int minute : 6;
        unsigned int hour : 5;
        unsigned int day : 5;
        unsigned int month : 4;
        unsigned int year : 7;
    };
};

enum ImageType {
    TypeUnknown,
    Type3DSGame,
    Type3DSDemo,
    Type3DSWare,
    TypeeShop
};

struct release {
    int id;
    QString name;
    QString publisher;
    QString region;
    QString languages;
    QString group;
    qint64 imagesize;
    QString serial;
    QString titleid;
    qint32 imgcrc;
    QString filename;
    QString releasename;
    qint64 trimmedsize;
    QString firmware;
    ImageType type;

    inline bool operator ==(const release other) const {
        if (other.filename.toLower().startsWith(filename.toLower())) {
            return true;
        }
        QString this_name(name), other_name(other.name);
        this_name.remove(QRegExp("[^a-zA-Z\\d\\s]"));
        other_name.remove(QRegExp("[^a-zA-Z\\d\\s]"));
        if (other_name.toLower().startsWith(this_name.toLower())) {
            return true;
        }
        QString this_releasename(releasename), other_releasename(other.releasename);
        this_releasename.remove(QRegExp("[^a-zA-Z\\d\\s]"));
        other_releasename.remove(QRegExp("[^a-zA-Z\\d\\s]"));
        if (other_releasename.toLower().startsWith(this_releasename.toLower())) {
            return true;
        }

        return false;
    }
};

QMap<int, release> release_db;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    QFileInfoList drives = QDir::drives();
    foreach (QFileInfo drive, drives) {
        QString volname(drive.absoluteFilePath());

        WCHAR szVolumeName[256] ;
        WCHAR szFileSystemName[256];
        DWORD dwSerialNumber = 0;
        DWORD dwMaxFileNameLength = 256;
        DWORD dwFileSystemFlags = 0;
        bool ret = GetVolumeInformation((WCHAR *) drive.absoluteFilePath().utf16(), szVolumeName, 256, &dwSerialNumber, &dwMaxFileNameLength, &dwFileSystemFlags, szFileSystemName, 256);
        if (ret) {
            volname = QString("%1 (%2)").arg(QString::fromUtf16((const ushort *) szVolumeName)).arg(volname);
        }

        if (GetDriveType((WCHAR *) drive.absoluteFilePath().utf16()) == DRIVE_REMOVABLE) {
            ui->comboBox->addItem(volname, QString("\\\\.\\%1").arg(drive.absoluteFilePath().remove(QRegExp("[^:]+$"))));
        }
    }
#else
#endif

    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->treeWidget->setDefaultDropAction(Qt::MoveAction);
    ui->treeWidget->setDropIndicatorShown(true);

    ui->treeWidget->setAlternatingRowColors(true);

    ui->treeWidget->header()->resizeSection(0, 100);
    ui->treeWidget->header()->resizeSection(1, 200);
    ui->treeWidget->header()->resizeSection(2, 100);

    ui->treeWidget->sortByColumn(-1);

    QFile file("3dsreleases.xml");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QProgressDialog progress("Loading 3DS Rom Database", "Cancel", 0, 0, this);
        progress.setValue(0);
        progress.show();

        qApp->processEvents();

        QXmlStreamReader xml(&file);

        release info;

        while (!xml.atEnd() && !xml.hasError()) {
            if (progress.wasCanceled()) {
                release_db.clear();

                break;
            }

            qApp->processEvents();

            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name().toString() == "id") {
                    info.id = xml.readElementText().toInt();
                }
                else if (xml.name().toString() == "name") {
                    info.name = xml.readElementText();
                }
                else if (xml.name().toString() == "publisher") {
                    info.publisher = xml.readElementText();
                }
                else if (xml.name().toString() == "region") {
                    info.region = xml.readElementText();
                }
                else if (xml.name().toString() == "languages") {
                    info.languages = xml.readElementText();
                }
                else if (xml.name().toString() == "group") {
                    info.group = xml.readElementText();
                }
                else if (xml.name().toString() == "imagesize") {
                    info.imagesize = xml.readElementText().toLongLong();
                }
                else if (xml.name().toString() == "serial") {
                    info.serial = xml.readElementText();
                }
                else if (xml.name().toString() == "titleid") {
                    info.titleid = xml.readElementText();
                }
                else if (xml.name().toString() == "imgcrc") {
                    info.imgcrc = xml.readElementText().toLong(NULL, 16);
                }
                else if (xml.name().toString() == "filename") {
                    info.filename = xml.readElementText();
                }
                else if (xml.name().toString() == "releasename") {
                    info.releasename = xml.readElementText();
                }
                else if (xml.name().toString() == "trimmedsize") {
                    info.trimmedsize = xml.readElementText().toLongLong();
                }
                else if (xml.name().toString() == "firmware") {
                    info.firmware = xml.readElementText();
                }
                else if (xml.name().toString() == "type") {
                    info.type = (ImageType) xml.readElementText().toInt();
                }
            }
            else if (xml.isEndElement()) {
                if (xml.name().toString() == "release") {
                    release_db.insert(info.id, info);
                }

                info = { 0 };
            }
            else if (xml.hasError()) {
                qDebug("XML Error: %s", xml.errorString().toStdString().c_str());
            }
            else if (xml.atEnd()) {

            }
        }

        file.close();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadButton_clicked()
{
    selectedDrive = ui->comboBox->currentData().toString();

    QFile file(selectedDrive);
    if (file.open(QIODevice::ReadOnly)) {
        QProgressDialog progress("Loading", "Cancel", 0, 0, this);
        progress.setValue(0);
        progress.show();

        ui->treeWidget->clear();
        ui->treeWidget->sortByColumn(-1);

        qApp->processEvents();

        quint8 buffer[0x100];

        if (file.read((char *) buffer, 0x100)) {
            char fsid[9];
            memcpy(fsid, buffer + 3, 8);

            if (QString(fsid).startsWith("EXFAT")) {
                int sector_per_cluster;
                quint32 bitmap_sector, root_directory_cluster;

                bytes_per_sector = qPow(2, *(quint8 *)(buffer + 0x6C));
                sector_per_cluster = qPow(2, *(quint8 *)(buffer + 0x6D));
                bitmap_sector = *(quint32 *)(buffer + 0x58);
                root_directory_cluster = *(quint32 *)(buffer + 0x60);

                quint32 first_cluster = bitmap_sector - (2 * sector_per_cluster);
                root_directory_sector = (first_cluster + (root_directory_cluster * sector_per_cluster)) * bytes_per_sector;

                qDebug("Root directory: %08x", root_directory_sector);

                if (!file.seek(root_directory_sector)) {
                    while (file.pos() < root_directory_sector) {
                        quint8 data[0x20];
                        file.read((char *) data, 0x20);
                    }
                }

                while (file.read((char *) buffer, 0x20) && buffer[0] != 0) {
                    if (progress.wasCanceled()) {
                        ui->treeWidget->clear();
                        ui->treeWidget->sortByColumn(-1);

                        break;
                    }

                    qApp->processEvents();

                    bool deleted = false;
                    quint8 stream_ext = 0xC0, fname_ext = 0xC1;

                    switch (buffer[0]) {
                    case 0x03:
                    case 0x81:
                    case 0x82:
                        break;

                    case 0x05:
                        deleted = true;
                        stream_ext = 0x40;
                        fname_ext = 0x41;

                    case 0x85: {
                        qint64 rec_offset = file.pos() - 0x20;
                        qDebug("Record entry: %08x", rec_offset);

                        if (first_sector == 0) {
                            first_sector = rec_offset;
                        }

                        timestamp created_date, last_accessed_date, modified_date;
                        created_date.time = *(quint32 *)(buffer + 8);
                        last_accessed_date.time = *(quint32 *)(buffer + 12);
                        modified_date.time = *(quint32 *)(buffer + 16);

                        file.read((char *) buffer, 0x20);
                        if (buffer[0] == stream_ext) {
                            quint16 namelen = qFromBigEndian(*(quint16 *)(buffer + 2));
                            quint64 filesize = *(quint64 *)(buffer + 8);

                            wchar_t fname[256];
                            memset(fname, 0, sizeof(fname));

                            wchar_t *ptrname = fname;
                            while (ptrname - fname < namelen) {
                                file.read((char *) buffer, 0x20);
                                if (buffer[0] == fname_ext) {
                                    memcpy(ptrname, buffer + 2, 0x20 - 2);

                                    while (*ptrname) {
                                        ptrname++;
                                    }
                                }
                                else {
                                    qDebug("Wrong entry %02x at %08x, expected %02x", buffer[0], file.pos() - 0x20, stream_ext);
                                    break;
                                }
                            }

                            qint64 next_offset = file.pos();

                            QString filename = QString::fromWCharArray(fname);

                            qDebug("Offset: %08x - %08x: %s", rec_offset, next_offset, filename.toStdString().c_str());

                            QDateTime created, last_accessed, modified;
                            created.setDate(QDate(created_date.year + 1980, created_date.month, created_date.day));
                            created.setTime(QTime(created_date.hour, created_date.minute, created_date.second * 2));
                            last_accessed.setDate(QDate(last_accessed_date.year + 1980, last_accessed_date.month, last_accessed_date.day));
                            last_accessed.setTime(QTime(last_accessed_date.hour, last_accessed_date.minute, last_accessed_date.second * 2));
                            modified.setDate(QDate(modified_date.year + 1980, modified_date.month, modified_date.day));
                            modified.setTime(QTime(modified_date.hour, modified_date.minute, modified_date.second * 2));

                            qDebug("Created date: %s", created.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());

                            QTreeWidgetItem *item = new QTreeWidgetItem();
                            item->setText(0, filename.length() > 0 ? filename : "-");
                            item->setData(4, Qt::DisplayRole, filesize);
                            item->setData(5, Qt::DisplayRole, created);
                            item->setData(6, Qt::DisplayRole, last_accessed);
                            item->setData(7, Qt::DisplayRole, modified);
                            item->setData(0, Qt::UserRole, rec_offset);
                            item->setData(0, Qt::UserRole + 1, next_offset);

                            QFileInfo fileInfo(filename);

                            if (deleted) {
                                item->setForeground(0, QBrush(Qt::red));
                            }
                            else {
                                release info;
                                info.name = fileInfo.completeBaseName();
                                info.filename = fileInfo.completeBaseName();
                                info.releasename = fileInfo.completeBaseName();

                                int release_id = release_db.key(info, -1);
                                if (release_id > -1) {
                                    item->setText(1, release_db[release_id].name);
                                    item->setText(2, release_db[release_id].titleid);
                                }
                                else {
                                    item->setText(1, fileInfo.completeBaseName());
                                    item->setText(2, "-");
                                }
                            }

                            item->setText(3, fileInfo.suffix().toUpper());

                            item->setFlags(item->flags() & ~Qt::ItemIsDropEnabled);

                            ui->treeWidget->addTopLevelItem(item);

                            if (deleted) {
                                item->setHidden(true);
                            }

                            if (!file.seek(next_offset)) {
                                while (file.pos() < next_offset) {
                                    quint8 data[0x20];
                                    file.read((char *) data, 0x20);
                                }
                            }

                            last_sector = next_offset;
                        }
                        else {
                            qDebug("Wrong entry %02x at %08x, expected %02x", buffer[0], file.pos() - 0x20, stream_ext);
                        }
                    }
                        break;

                    default:
                        qDebug("Unknown entry %02x at %08x", buffer[0], file.pos() - 0x20);
                        break;
                    }
                }

                qDebug("Offset: %08x - %08x", first_sector, last_sector);
            }
            else {
                QMessageBox::critical(this, "Error", "File format not supported", QMessageBox::Ok);
            }
        }

        file.close();
    }
    else {
        QMessageBox::critical(this, "Error", QString("Cannot open disk: %1").arg(selectedDrive.remove(QRegExp("[\\/.]+"))), QMessageBox::Ok);
    }
}

void MainWindow::on_saveButton_clicked()
{
    if (QMessageBox::warning(this, "Warning", "Writing directly to disk is a dangerous operation.\nDo you wish to continue?", QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
        QFile diskin(selectedDrive), filein("input"), fileout("output");
        if (!diskin.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, "Error", QString("Cannot open disk: %1").arg(selectedDrive.remove(QRegExp("[\\/.]+"))), QMessageBox::Ok);
        }
        if (!filein.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QMessageBox::critical(this, "Error", QString("Cannot open file: %1").arg("input"), QMessageBox::Ok);
        }

        QProgressDialog progress("Saving", "Cancel", 0, 0, this);
        progress.setValue(0);
        progress.show();

        qApp->processEvents();

        quint8 buffer[0x2000];

        if (diskin.isOpen() && filein.isOpen()) {
            qDebug("Record Entry: %08x - %08x", first_sector, last_sector);

            if (!diskin.seek(root_directory_sector)) {
                while (diskin.pos() < root_directory_sector) {
                    quint8 data[0x100];
                    diskin.read((char *) data, 0x100);
                }
            }

            while (diskin.read((char *) buffer, 0x100)) {
                if (progress.wasCanceled()) {
                    filein.resize(0);

                    break;
                }

                filein.write((char *) buffer, 0x100);

                if (diskin.pos() >= last_sector) {
                    break;
                }
            }

            diskin.close();
            filein.close();

            if (!filein.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, "Error", QString("Cannot open file: %1").arg("input"), QMessageBox::Ok);
            }
            if (!fileout.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                QMessageBox::critical(this, "Error", QString("Cannot open file: %1").arg("output"), QMessageBox::Ok);
            }

            if (filein.isOpen() && fileout.isOpen()) {
                while (filein.read((char *) buffer, 0x100)) {
                    if (progress.wasCanceled()) {
                        fileout.resize(0);

                        break;
                    }

                    fileout.write((char *) buffer, 0x100);

                    if (filein.pos() >= first_sector - root_directory_sector) {
                        break;
                    }
                }

                if (fileout.pos() >= first_sector - root_directory_sector) {
                    progress.setMaximum(ui->treeWidget->topLevelItemCount());

                    int i = 0;
                    for (; i < ui->treeWidget->topLevelItemCount(); i++) {
                        if (progress.wasCanceled()) {
                            fileout.resize(0);

                            break;
                        }

                        progress.setValue(i);

                        qApp->processEvents();

                        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);

                        qint64 rec_offset = item->data(0, Qt::UserRole).toLongLong() - root_directory_sector;
                        qint64 next_offset = item->data(0, Qt::UserRole + 1).toLongLong() - root_directory_sector;

                        qDebug("Offset: %08x - %08x: %s", rec_offset, next_offset, item->text(0).toStdString().c_str());

                        if (!filein.seek(rec_offset)) {
                            while (filein.pos() < rec_offset) {
                                quint8 data[0x20];
                                filein.read((char *) data, 0x20);
                            }
                        }

                        while (filein.read((char *) buffer, 0x20)) {
                            if (progress.wasCanceled()) {
                                fileout.resize(0);

                                break;
                            }

                            fileout.write((char *) buffer, 0x20);

                            if (filein.pos() >= next_offset) {
                                break;
                            }
                        }
                    }

                    progress.setValue(i);

                    memset(buffer, 0, sizeof(buffer));

                    while (fileout.pos() % bytes_per_sector > 0) {
                        fileout.write((char *) buffer, 0x20);
                    }
                }

                filein.close();
                fileout.close();

                if (!fileout.open(QIODevice::ReadOnly)) {
                    QMessageBox::critical(this, "Error", QString("Cannot open file: %1").arg("output"), QMessageBox::Ok);
                }
                if (!diskin.open(QIODevice::ReadWrite)) {
                    QMessageBox::critical(this, "Error", QString("Cannot open disk: %1").arg(selectedDrive.remove(QRegExp("[\\/.]+"))), QMessageBox::Ok);
                }

                if (fileout.isOpen() && diskin.isOpen()) {
                    progress.setMaximum(0);
                    progress.setValue(0);

                    if (!diskin.seek(root_directory_sector)) {
                        while (diskin.pos() < root_directory_sector) {
                            diskin.read((char *) buffer, bytes_per_sector);
                        }
                    }

                    if (diskin.pos() != root_directory_sector) {
                        QMessageBox::critical(this, "Error", "File operation error", QMessageBox::Ok);
                    }
                    else {
                        while (fileout.read((char *) buffer, bytes_per_sector)) {
                            if (progress.wasCanceled()) {
                                break;
                            }

                            qint64 ret = diskin.write((char *) buffer, bytes_per_sector);
                            if (ret > -1) {
                                qDebug("Write to Disk: (%lld) %08x", ret, diskin.pos() - ret);
                            }
                            else {
                                qDebug("Write error: (%d) %s", diskin.error(), diskin.errorString().toStdString().c_str());
                            }

                            if (diskin.pos() >= last_sector) {
                                break;
                            }
                        }

                        QClipboard *clipboard = QApplication::clipboard();
                        clipboard->setText(QString().sprintf("%08x", root_directory_sector));

                        QString message = QString().sprintf("Disk Data Offset %08x - %08x has been dumped to \n%s",
                                                            root_directory_sector, diskin.pos(), QFileInfo(fileout).absoluteFilePath().replace('/', QDir::separator()).toStdString().c_str());

                        QMessageBox::information(this, "Success", message, QMessageBox::Ok);
                    }

                    fileout.close();
                    diskin.close();
                }
            }
        }
    }
}
