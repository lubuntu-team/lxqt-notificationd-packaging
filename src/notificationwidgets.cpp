/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QComboBox>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QLabel>

#include "notificationwidgets.h"

#include <QtDebug>


NotificationActionsWidget::NotificationActionsWidget(const QStringList& actions, QWidget *parent)
    : QWidget(parent)
{
    for (int i = 0; i < actions.count(); ++i)
    {
        if (i == actions.count()-1)
        {
            qDebug() << "NotificationActionsWidget actions has contains pairs (id, value, id, value...) got odd count:" << actions.count() << "Actions:" << actions;
            m_actionMap[actions.at(i)] = actions.at(i);
        }
        else
        {
            m_actionMap[actions.at(i)] = actions.at(i+1);
        }
        ++i; // move to the next ID
    }

    // if there is only one action let's take it as a default one
    if (m_actionMap.count() == 1)
        m_defaultAction = m_actionMap[m_actionMap.keys().at(0)];

    qDebug() << "NotificationActionsWidget processed actions:" << m_actionMap;
}


NotificationActionsButtonsWidget::NotificationActionsButtonsWidget(const QStringList& actions, QWidget *parent)
    : NotificationActionsWidget(actions, parent)
{
    QHashIterator<QString,QString> it(m_actionMap);
    QHBoxLayout *l = new QHBoxLayout();
    setLayout(l);

    QButtonGroup *group = new QButtonGroup(this);

    while (it.hasNext())
    {
        it.next();
        QPushButton *b = new QPushButton(it.value(), this);
        l->addWidget(b);
        group->addButton(b);
        if (it.key() == "default")
        {
            b->setFocus(Qt::OtherFocusReason);
            m_defaultAction = it.key();
        }
    }
    connect(group, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(actionButtonActivated(QAbstractButton*)));
}

void NotificationActionsButtonsWidget::actionButtonActivated(QAbstractButton* button)
{
    emit actionTriggered(m_actionMap.key(button->text()));
}


NotificationActionsComboWidget::NotificationActionsComboWidget(const QStringList& actions, QWidget *parent)
    : NotificationActionsWidget(actions, parent)
{
    QHashIterator<QString,QString> it(m_actionMap);
    QHBoxLayout *l = new QHBoxLayout();
    setLayout(l);

    l->addWidget(new QLabel(tr("Actions:"), this));
    m_comboBox = new QComboBox(this);
    int currentIndex = -1;
    while (it.hasNext())
    {
        it.next();
        m_comboBox->addItem(it.value(), it.key());
        if (it.key() == "default")
        {
            currentIndex = m_comboBox->count()-1;
            m_defaultAction = it.key();
        }
    }
    l->addWidget(m_comboBox);

    if (currentIndex != -1)
        m_comboBox->setCurrentIndex(currentIndex);

    QPushButton *b = new QPushButton(tr("OK"), this);
    l->addWidget(b);
    connect(b, SIGNAL(clicked()),
            this, SLOT(actionComboBoxActivated()));
}

void NotificationActionsComboWidget::actionComboBoxActivated()
{
    if (!m_comboBox)
        return;
    int ix = m_comboBox->currentIndex();
    if (ix == -1)
        return;
    emit actionTriggered(m_actionMap.key(m_comboBox->itemText(ix)));
}
