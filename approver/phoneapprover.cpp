/*
 * <one line to give the library's name and an idea of what it does.>
 * SPDX-FileCopyrightText: 2020 Bhushan Shah <bshah@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "phoneapprover.h"

#include <TelepathyQt/ChannelClassSpec>
#include <TelepathyQt/ChannelDispatchOperation>

#include "dispatchoperation.h"

static inline Tp::ChannelClassSpecList channelClassSpecList()
{
    return Tp::ChannelClassSpecList() << Tp::ChannelClassSpec::audioCall();
}

PhoneApprover::PhoneApprover(QObject *parent)
    : QObject(parent)
    , AbstractClientApprover(channelClassSpecList())
{
}

void PhoneApprover::addDispatchOperation(const Tp::MethodInvocationContextPtr<> &context, const Tp::ChannelDispatchOperationPtr &dispatchOperation)
{
    new DispatchOperation(dispatchOperation, this);
    context->setFinished();
}
