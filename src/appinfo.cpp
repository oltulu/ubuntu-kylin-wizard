// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
/*
 * Copyright (C) 2019, National University of Defense Technology(NUDT) & Kylin Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: handsome_feng <jianfengli@ubuntukylin.com>
 */

#include <QFileInfo>
#include <QIcon>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

#include "appinfo.h"

static const QString ICON_PREFIX = "/usr/share/ubuntu-kylin-software-center/data/icons/";
static const QString DEFAULT_ICON = ICON_PREFIX + "ubufox.png";
static const QString SOFTWARE_DATABASE = "/usr/share/ubuntu-kylin-software-center/data/uksc.db";

QSqlDatabase AppInfo::db = QSqlDatabase::addDatabase("QSQLITE");

AppInfo::AppInfo(const QString & app_name)
    : name(app_name)
{
    QString icon_path = ICON_PREFIX + name + ".png";
    if (fileExist(icon_path))
        icon = QIcon(icon_path);
    else
        icon = QIcon(DEFAULT_ICON);

    summary = fetchSummary();
}

QString AppInfo::getName() const
{
    return name;
}

QIcon AppInfo::getIcon() const
{
    return icon;
}

QString AppInfo::getSummary() const
{
    return summary;
}

bool AppInfo::fileExist(QString file_name)
{
    QFileInfo file(file_name);
    if (file.exists() && file.isFile())
        return true;
    else
        return false;
}

QString AppInfo::fetchSummary()
{
    if (db.databaseName().isEmpty()) {
        db.setDatabaseName(SOFTWARE_DATABASE);
        if (!db.open()) {
            fprintf (stderr, "Open db failed: %s\n", qPrintable(db.lastError().text()));
        }
    }
    QString query_string = QString("select summary from application where app_name = '%1'").arg(name);
    QSqlQuery query(db);
    query.prepare(query_string);
    if (!query.exec()) {
        fprintf (stderr, "Exec query failed: %s\n", qPrintable(db.lastError().text()));
        return QString();
    }
    QString summary;
    while (query.next()) {
         summary = query.value(0).toString();
    }
    return summary;
}
