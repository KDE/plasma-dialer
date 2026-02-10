// SPDX-FileCopyrightText: 2026 Micah Stanley <stanleymicah@proton.me>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

class SharedUtils : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit SharedUtils(QObject *parent = nullptr);

    Q_INVOKABLE void launchCallScreen();
};
