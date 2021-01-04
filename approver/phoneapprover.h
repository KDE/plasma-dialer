/*
 * <one line to give the library's name and an idea of what it does.>
 * SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef PHONEAPPROVER_H
#define PHONEAPPROVER_H

#include <TelepathyQt/AbstractClientApprover>

class PhoneApprover : public QObject,  public Tp::AbstractClientApprover
{
    Q_OBJECT

public:
    PhoneApprover(QObject *parent = nullptr);
    
protected:
    void addDispatchOperation(const Tp::MethodInvocationContextPtr<>& context,
                              const Tp::ChannelDispatchOperationPtr& dispatchOperation) override;

};

#endif // PHONEAPPROVER_H
