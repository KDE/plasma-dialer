// SPDX-FileCopyrightText: 2026 Micah Stanley <stanleymicah@proton.me>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "sharedutils.h"

#include <QProcess>
#include <QStandardPaths>
#include <QDebug>

SharedUtils::SharedUtils(QObject *parent) : QObject(parent)
{
}

void SharedUtils::launchCallScreen()
{
    QString executable = QStandardPaths::findExecutable(QStringLiteral("plasma-call-screen"));

    if (executable.isEmpty()) {
        qWarning() << "Could not find executable.";
        return;
    }

    QProcess::startDetached(executable, QStringList());
}
