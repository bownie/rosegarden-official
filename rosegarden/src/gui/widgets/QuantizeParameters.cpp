/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
 
    This program is Copyright 2000-2006
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <richard.bown@ferventsoftware.com>
 
    The moral rights of Guillaume Laurent, Chris Cannam, and Richard
    Bown to claim authorship of this work have been asserted.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#include "QuantizeParameters.h"

#include <klocale.h>
#include "base/NotationTypes.h"
#include "base/Quantizer.h"
#include "gui/editors/notation/NotationStrings.h"
#include "gui/editors/notation/NotePixmapFactory.h"
#include <kcombobox.h>
#include <kconfig.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qwidget.h>


namespace Rosegarden
{

QuantizeParameters::QuantizeParameters(QWidget *parent,
                                       QuantizerType defaultQuantizer,
                                       bool showNotationOption,
                                       bool showAdvancedButton,
                                       QString configCategory,
                                       QString preamble) :
        QFrame(parent),
        m_configCategory(configCategory),
        m_standardQuantizations
        (BasicQuantizer::getStandardQuantizations())
{
    m_mainLayout = new QGridLayout(this,
                                   preamble ? 3 : 4, 2,
                                   preamble ? 10 : 0,
                                   preamble ? 5 : 4);

    int zero = 0;
    if (preamble) {
        QLabel *label = new QLabel(preamble, this);
        label->setAlignment(Qt::WordBreak);
        m_mainLayout->addMultiCellWidget(label, 0, 0, 0, 1);
        zero = 1;
    }

    QGroupBox *quantizerBox = new QGroupBox
                              (1, Horizontal, i18n("Quantizer"), this);

    m_mainLayout->addWidget(quantizerBox, zero, 0);
    QFrame *typeFrame = new QFrame(quantizerBox);

    QGridLayout *layout = new QGridLayout(typeFrame, 2, 2, 5, 3);
    layout->addWidget(new QLabel(i18n("Quantizer type:"), typeFrame), 0, 0);
    m_typeCombo = new KComboBox(typeFrame);
    m_typeCombo->insertItem(i18n("Grid quantizer"));
    m_typeCombo->insertItem(i18n("Legato quantizer"));
    m_typeCombo->insertItem(i18n("Heuristic notation quantizer"));
    layout->addWidget(m_typeCombo, 0, 1);

    m_notationTarget = new QCheckBox
                       (i18n("Quantize for notation only (leave performance unchanged)"),
                        typeFrame);
    layout->addMultiCellWidget(m_notationTarget, 1, 1, 0, 1);
    if (!showNotationOption)
        m_notationTarget->hide();

    QHBox *parameterBox = new QHBox(this);
    m_mainLayout->addWidget(parameterBox, zero + 1, 0);

    m_notationBox = new QGroupBox
                    (1, Horizontal, i18n("Notation parameters"), parameterBox);
    QFrame *notationFrame = new QFrame(m_notationBox);

    layout = new QGridLayout(notationFrame, 4, 2, 5, 3);

    layout->addWidget(new QLabel(i18n("Base grid unit:"), notationFrame),
                      1, 0);
    m_notationUnitCombo = new KComboBox(notationFrame);
    layout->addWidget(m_notationUnitCombo, 1, 1);

    layout->addWidget(new QLabel(i18n("Complexity:"),
                                 notationFrame), 0, 0);

    m_simplicityCombo = new KComboBox(notationFrame);
    m_simplicityCombo->insertItem(i18n("Very high"));
    m_simplicityCombo->insertItem(i18n("High"));
    m_simplicityCombo->insertItem(i18n("Normal"));
    m_simplicityCombo->insertItem(i18n("Low"));
    m_simplicityCombo->insertItem(i18n("Very low"));
    layout->addWidget(m_simplicityCombo, 0, 1);

    layout->addWidget(new QLabel(i18n("Tuplet level:"),
                                 notationFrame), 2, 0);
    m_maxTuplet = new KComboBox(notationFrame);
    m_maxTuplet->insertItem(i18n("None"));
    m_maxTuplet->insertItem(i18n("2-in-the-time-of-3"));
    m_maxTuplet->insertItem(i18n("Triplet"));
    /*
        m_maxTuplet->insertItem(i18n("4-Tuplet"));
        m_maxTuplet->insertItem(i18n("5-Tuplet"));
        m_maxTuplet->insertItem(i18n("6-Tuplet"));
        m_maxTuplet->insertItem(i18n("7-Tuplet"));
        m_maxTuplet->insertItem(i18n("8-Tuplet"));
    */
    m_maxTuplet->insertItem(i18n("Any"));
    layout->addWidget(m_maxTuplet, 2, 1);

    m_counterpoint = new QCheckBox(i18n("Permit counterpoint"), notationFrame);
    layout->addMultiCellWidget(m_counterpoint, 3, 3, 0, 1);

    m_gridBox = new QGroupBox
                (1, Horizontal, i18n("Grid parameters"), parameterBox);
    QFrame *gridFrame = new QFrame(m_gridBox);

    layout = new QGridLayout(gridFrame, 4, 2, 5, 3);

    layout->addWidget(new QLabel(i18n("Base grid unit:"), gridFrame), 0, 0);
    m_gridUnitCombo = new KComboBox(gridFrame);
    layout->addWidget(m_gridUnitCombo, 0, 1);

    m_swingLabel = new QLabel(i18n("Swing:"), gridFrame);
    layout->addWidget(m_swingLabel, 1, 0);
    m_swingCombo = new KComboBox(gridFrame);
    layout->addWidget(m_swingCombo, 1, 1);

    m_iterativeLabel = new QLabel(i18n("Iterative amount:"), gridFrame);
    layout->addWidget(m_iterativeLabel, 2, 0);
    m_iterativeCombo = new KComboBox(gridFrame);
    layout->addWidget(m_iterativeCombo, 2, 1);

    m_durationCheckBox = new QCheckBox
                         (i18n("Quantize durations as well as start times"), gridFrame);
    layout->addMultiCellWidget(m_durationCheckBox, 3, 3, 0, 1);

    m_postProcessingBox = new QGroupBox
                          (1, Horizontal, i18n("After quantization"), this);

    if (preamble) {
        m_mainLayout->addMultiCellWidget(m_postProcessingBox,
                                         zero, zero + 1,
                                         1, 1);
    } else {
        m_mainLayout->addWidget(m_postProcessingBox, zero + 3, 0);
    }

    bool advanced = true;
    m_advancedButton = 0;
    if (showAdvancedButton) {
        m_advancedButton =
            new QPushButton(i18n("Show advanced options"), this);
        m_mainLayout->addWidget(m_advancedButton, zero + 2, 0, Qt::AlignLeft);
        QObject::connect(m_advancedButton, SIGNAL(clicked()),
                         this, SLOT(slotAdvancedChanged()));
    }

    QFrame *postFrame = new QFrame(m_postProcessingBox);

    layout = new QGridLayout(postFrame, 4, 1, 5, 3);
    m_rebeam = new QCheckBox(i18n("Re-beam"), postFrame);
    m_articulate = new QCheckBox
                   (i18n("Add articulations (staccato, tenuto, slurs)"), postFrame);
    m_makeViable = new QCheckBox(i18n("Tie notes at barlines etc"), postFrame);
    m_deCounterpoint = new QCheckBox(i18n("Split-and-tie overlapping chords"), postFrame);

    layout->addWidget(m_rebeam, 0, 0);
    layout->addWidget(m_articulate, 1, 0);
    layout->addWidget(m_makeViable, 2, 0);
    layout->addWidget(m_deCounterpoint, 3, 0);

    QPixmap noMap = NotePixmapFactory::toQPixmap
                    (NotePixmapFactory::makeToolbarPixmap("menu-no-note"));

    int defaultType = 0;
    timeT defaultUnit =
        Note(Note::Demisemiquaver).getDuration();

    if (!m_configCategory) {
        if (defaultQuantizer == Notation)
            m_configCategory = "Quantize Dialog Notation";
        else
            m_configCategory = "Quantize Dialog Grid";
    }

    int defaultSwing = 0;
    int defaultIterate = 100;

    if (m_configCategory) {
        KConfig *config = kapp->config();
        config->setGroup(m_configCategory);
        defaultType =
            config->readNumEntry("quantizetype",
                                 (defaultQuantizer == Notation) ? 2 :
                                 (defaultQuantizer == Legato) ? 1 :
                                 0);
        defaultUnit =
            config->readNumEntry("quantizeunit", defaultUnit);
        defaultSwing =
            config->readNumEntry("quantizeswing", defaultSwing);
        defaultIterate =
            config->readNumEntry("quantizeiterate", defaultIterate);
        m_notationTarget->setChecked
        (config->readBoolEntry("quantizenotationonly",
                               defaultQuantizer == Notation));
        m_durationCheckBox->setChecked
        (config->readBoolEntry("quantizedurations", false));
        m_simplicityCombo->setCurrentItem
        (config->readNumEntry("quantizesimplicity", 13) - 11);
        m_maxTuplet->setCurrentItem
        (config->readNumEntry("quantizemaxtuplet", 3) - 1);
        m_counterpoint->setChecked
        (config->readBoolEntry("quantizecounterpoint", false));
        m_rebeam->setChecked
        (config->readBoolEntry("quantizerebeam", true));
        m_makeViable->setChecked
        (config->readBoolEntry("quantizemakeviable",
                               defaultQuantizer == Notation));
        m_deCounterpoint->setChecked
        (config->readBoolEntry("quantizedecounterpoint",
                               defaultQuantizer == Notation));
        m_articulate->setChecked
        (config->readBoolEntry("quantizearticulate", true));
        advanced = config->readBoolEntry("quantizeshowadvanced", false);
    } else {
        defaultType =
            (defaultQuantizer == Notation) ? 2 :
            (defaultQuantizer == Legato) ? 1 : 0;
        m_notationTarget->setChecked(defaultQuantizer == Notation);
        m_durationCheckBox->setChecked(false);
        m_simplicityCombo->setCurrentItem(2);
        m_maxTuplet->setCurrentItem(2);
        m_counterpoint->setChecked(false);
        m_rebeam->setChecked(true);
        m_makeViable->setChecked(defaultQuantizer == Notation);
        m_deCounterpoint->setChecked(defaultQuantizer == Notation);
        m_articulate->setChecked(true);
        advanced = false;
    }

    if (preamble || advanced) {
        m_postProcessingBox->show();
    } else {
        m_postProcessingBox->hide();
    }

    for (unsigned int i = 0; i < m_standardQuantizations.size(); ++i) {

        timeT time = m_standardQuantizations[i];
        timeT error = 0;

        QPixmap pmap = NotePixmapFactory::toQPixmap
                       (NotePixmapFactory::makeNoteMenuPixmap(time, error));
        QString label = NotationStrings::makeNoteMenuLabel(time, false, error);

        if (error == 0) {
            m_gridUnitCombo->insertItem(pmap, label);
            m_notationUnitCombo->insertItem(pmap, label);
        } else {
            m_gridUnitCombo->insertItem(noMap, QString("%1").arg(time));
            m_notationUnitCombo->insertItem(noMap, QString("%1").arg(time));
        }

        if (m_standardQuantizations[i] == defaultUnit) {
            m_gridUnitCombo->setCurrentItem(m_gridUnitCombo->count() - 1);
            m_notationUnitCombo->setCurrentItem
            (m_notationUnitCombo->count() - 1);
        }
    }

    for (int i = -100; i <= 200; i += 10) {
        m_swingCombo->insertItem(i == 0 ? i18n("None") : QString("%1%").arg(i));
        if (i == defaultSwing)
            m_swingCombo->setCurrentItem(m_swingCombo->count() - 1);
    }

    for (int i = 10; i <= 100; i += 10) {
        m_iterativeCombo->insertItem(i == 100 ? i18n("Full quantize") :
                                     QString("%1%").arg(i));
        if (i == defaultIterate)
            m_iterativeCombo->setCurrentItem(m_iterativeCombo->count() - 1);
    }

    switch (defaultType) {
    case 0:  // grid
        m_gridBox->show();
        m_swingLabel->show();
        m_swingCombo->show();
        m_iterativeLabel->show();
        m_iterativeCombo->show();
        m_notationBox->hide();
        m_durationCheckBox->show();
        m_typeCombo->setCurrentItem(0);
        break;
    case 1:  // legato
        m_gridBox->show();
        m_swingLabel->hide();
        m_swingCombo->hide();
        m_iterativeLabel->hide();
        m_iterativeCombo->hide();
        m_notationBox->hide();
        m_durationCheckBox->hide();
        m_typeCombo->setCurrentItem(1);
    case 2:  // notation
        m_gridBox->hide();
        m_notationBox->show();
        m_typeCombo->setCurrentItem(2);
        break;
    }

    connect(m_typeCombo, SIGNAL(activated(int)), SLOT(slotTypeChanged(int)));
}

Quantizer *
QuantizeParameters::getQuantizer() const
{
    //!!! Excessive duplication with
    // EventQuantizeCommand::makeQuantizer in editcommands.cpp

    int type = m_typeCombo->currentItem();
    timeT unit = 0;

    if (type == 0 || type == 1) {
        unit = m_standardQuantizations[m_gridUnitCombo->currentItem()];
    } else {
        unit = m_standardQuantizations[m_notationUnitCombo->currentItem()];
    }

    Quantizer *quantizer = 0;

    int swing = m_swingCombo->currentItem();
    swing *= 10;
    swing -= 100;

    int iterate = m_iterativeCombo->currentItem();
    iterate *= 10;
    iterate += 10;

    if (type == 0) {

        if (m_notationTarget->isChecked()) {
            quantizer = new BasicQuantizer
                        (Quantizer::RawEventData,
                         Quantizer::NotationPrefix,
                         unit, m_durationCheckBox->isChecked(),
                         swing, iterate);
        } else {
            quantizer = new BasicQuantizer
                        (Quantizer::RawEventData,
                         Quantizer::RawEventData,
                         unit, m_durationCheckBox->isChecked(),
                         swing, iterate);
        }
    } else if (type == 1) {
        if (m_notationTarget->isChecked()) {
            quantizer = new LegatoQuantizer
                        (Quantizer::RawEventData,
                         Quantizer::NotationPrefix, unit);
        } else {
            quantizer = new LegatoQuantizer
                        (Quantizer::RawEventData,
                         Quantizer::RawEventData,
                         unit);
        }
    } else {

        NotationQuantizer *nq;

        if (m_notationTarget->isChecked()) {
            nq = new NotationQuantizer();
        } else {
            nq = new NotationQuantizer
                 (Quantizer::RawEventData,
                  Quantizer::RawEventData);
        }

        nq->setUnit(unit);
        nq->setSimplicityFactor(m_simplicityCombo->currentItem() + 11);
        nq->setMaxTuplet(m_maxTuplet->currentItem() + 1);
        nq->setContrapuntal(m_counterpoint->isChecked());
        nq->setArticulate(m_articulate->isChecked());

        quantizer = nq;
    }

    if (m_configCategory) {
        KConfig *config = kapp->config();
        config->setGroup(m_configCategory);
        config->writeEntry("quantizetype", type);
        config->writeEntry("quantizeunit", unit);
        config->writeEntry("quantizeswing", swing);
        config->writeEntry("quantizeiterate", iterate);
        config->writeEntry("quantizenotationonly",
                           m_notationTarget->isChecked());
        if (type == 0) {
            config->writeEntry("quantizedurations",
                               m_durationCheckBox->isChecked());
        } else {
            config->writeEntry("quantizesimplicity",
                               m_simplicityCombo->currentItem() + 11);
            config->writeEntry("quantizemaxtuplet",
                               m_maxTuplet->currentItem() + 1);
            config->writeEntry("quantizecounterpoint",
                               m_counterpoint->isChecked());
            config->writeEntry("quantizearticulate",
                               m_articulate->isChecked());
        }
        config->writeEntry("quantizerebeam", m_rebeam->isChecked());
        config->writeEntry("quantizemakeviable", m_makeViable->isChecked());
        config->writeEntry("quantizedecounterpoint", m_deCounterpoint->isChecked());
    }

    return quantizer;
}

void
QuantizeParameters::slotAdvancedChanged()
{
    if (m_postProcessingBox->isVisible()) {
        if (m_advancedButton)
            m_advancedButton->setText(i18n("Show Advanced Options"));
        m_postProcessingBox->hide();
    } else {
        if (m_advancedButton)
            m_advancedButton->setText(i18n("Hide Advanced Options"));
        m_postProcessingBox->show();
    }
    adjustSize();
}

void
QuantizeParameters::showAdvanced(bool show)
{
    if (show) {
        m_postProcessingBox->show();
    } else {
        m_postProcessingBox->hide();
    }
    adjustSize();
}

void
QuantizeParameters::slotTypeChanged(int index)
{
    if (index == 0) {
        m_gridBox->show();
        m_swingLabel->show();
        m_swingCombo->show();
        m_iterativeLabel->show();
        m_iterativeCombo->show();
        m_durationCheckBox->show();
        m_notationBox->hide();
    } else if (index == 1) {
        m_gridBox->show();
        m_swingLabel->hide();
        m_swingCombo->hide();
        m_iterativeLabel->hide();
        m_iterativeCombo->hide();
        m_durationCheckBox->hide();
        m_notationBox->hide();
    } else {
        m_gridBox->hide();
        m_notationBox->show();
    }
}

}
#include "QuantizeParameters.moc"
