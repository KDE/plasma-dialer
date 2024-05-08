// SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "call-model.h"
#include "callhistorydatabaseinterface.h"

class CallHistoryModel : public CallModel
{
    Q_OBJECT
public:
    CallHistoryModel(QObject *parent = nullptr);

    Q_INVOKABLE void addCall(const DialerTypes::CallData &callData);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(int index);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    org::kde::telephony::CallHistoryDatabase *m_databaseInterface;
    DialerTypes::CallDataVector m_calls;

    void fetchCalls();
};
