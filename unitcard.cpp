#include "unitcard.h"
#include "ui_unitcard.h"

UnitCard::UnitCard(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::UnitCard)
{
    bg_image = new QLabel(this);
    bg_image->move(pos());
    bg_image->setFixedSize(size());

    ui->setupUi(this);




    ui->miscText->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum);
    ui->damageWidget->setVisible(false);
    ui->label_10->setVisible(false);
    ui->label_11->setVisible(false);
    ui->label_12->setVisible(false);
    ui->label_13->setVisible(false);
    ui->label_14->setVisible(false);
    ui->label_15->setVisible(false);

    ui->tagsLabel->setVisible(false);

    ui->label->setVisible(false);
    ui->label_2->setVisible(false);
    ui->label_3->setVisible(false);
    ui->label_4->setVisible(false);
    ui->label_cmd->setVisible(false);
    ui->label_def->setVisible(false);
    ui->label_save->setVisible(false);
    ui->label_move->setVisible(false);

}

UnitCard::~UnitCard()
{
    delete ui;
}

void UnitCard::parceFromText(QString _data)
{
    if (cardText == _data && !updateNeeded)
        return;

    updateNeeded = false;

    ui->damageWidget->setVisible(false);
    ui->label_10->setVisible(false);
    ui->label_11->setVisible(false);
    ui->label_12->setVisible(false);
    ui->label_13->setVisible(false);
    ui->label_14->setVisible(false);
    ui->label_15->setVisible(false);

    ui->tagsLabel->setVisible(false);

    ui->label->setVisible(false);
    ui->label_2->setVisible(false);
    ui->label_3->setVisible(false);
    ui->label_4->setVisible(false);
    ui->label_cmd->setVisible(false);
    ui->label_def->setVisible(false);
    ui->label_save->setVisible(false);
    ui->label_move->setVisible(false);

    if (m_abilities.size() > 0){
        for (auto& abl : m_abilities){
            abl->setVisible(false);
            ui->abilitiesLayout->removeWidget(abl);
            abl->deleteLater();
        }
        m_abilities.clear();
    }

    if (m_damageValueLabels.size() > 0){
        for (auto& dmg : m_damageValueLabels){
            dmg->setVisible(false);
            ui->damageLayout->removeWidget(dmg);
            dmg->deleteLater();
        }
        m_damageValueLabels.clear();
    }

    cardText = _data;

    QStringList lines = _data.split('\n', Qt::SkipEmptyParts);


    //ui->mainDataFrame->setVisible(false);
    //ui->mainDataFrame->setVisible(true);
    bool isUnit = true;
    {
        QString title = lines.at(0);
        QStringList nameParts = title.split(QRegularExpression("[-|–]"));
        if (nameParts.size() > 0){
            name = nameParts.at(0);
            if (nameParts.size() > 1){
                QString ptsStr = nameParts.at(1);
                int findPts = nameParts.at(1).indexOf("pts");
                int ptsVal = ptsStr.simplified().left(ptsStr.simplified().indexOf(' ')).toInt();
                ui->ptsCost->setText(QString("%1 pts   ").arg(ptsVal));
            }
            ui->unitName->setText(name);

        }
    }
    {
        QString type = lines.at(1);
        ui->unitType->setText(type);
        if (type.toLower().contains("character") || type.toLower().contains("support card")){
            isUnit = false;
        }
    }
    QString miscText;
    for (int i = 2; i < lines.size(); ++i){
        QString line = lines.at(i);
        if (line.startsWith("-S")){ // System
            ability_frame *nextSystem = new ability_frame(this);
            if (nextSystem->makeFromString(line)){
                ui->abilitiesLayout->addWidget(nextSystem);
                m_abilities.push_back(nextSystem);
            }
            else{
                nextSystem->deleteLater();
            }

        } else if (line.toLower().startsWith("tags") ||line.toLower().startsWith("special") ){ //tags, split with ,
            QString tags = line.mid( line.indexOf(':') +1);
            ui->tagsLabel->setText(QString("<b>%1</b>").arg(tags));
            ui->tagsLabel->setVisible(true);
        } else if (line.toLower().startsWith("damage")){ //Damage grid
            ui->damageWidget->setVisible(true);
            ui->label_10->setVisible(true);
            ui->label_11->setVisible(true);
            ui->label_12->setVisible(true);
            ui->label_13->setVisible(true);
            ui->label_14->setVisible(true);
            ui->label_15->setVisible(true);

            QStringList damageLines = line.split('[');
            for (int j = 1; j < damageLines.size(); ++j){
                QString dl = damageLines.at(j);
                int first = dl.mid(0,1).toInt();
                int last = dl.mid(dl.indexOf(']')-1, 1).toInt();
                QString damageText = dl.mid(dl.indexOf(']') + 1);
                damageText.remove(':');
                damageText.remove(',');

                bool isGreen = false;
                bool isRed = false;
                if (damageText.toLower().contains("green") || damageText.toLower().contains("pin")){
                    isGreen = true;
                    damageText.remove("green", Qt::CaseInsensitive);
                }
                if (damageText.toLower().contains("death") || damageText.toLower().contains("dead")){
                    isRed = true;
                }

                QLabel* damageLabel = new QLabel(damageText, this);
                damageLabel->setLineWidth(1);
                damageLabel->setFrameStyle(QFrame::Panel);
                damageLabel->setAlignment(Qt::AlignCenter);
                if (isGreen){
                    damageLabel->setStyleSheet("border: 2px dashed gray");
                }
                if (isRed){
                    damageLabel->setStyleSheet("border: 3px ridge black");
                }
                ui->damageLayout->addWidget(damageLabel, 1, first - 1, 1, last - first + 1);
                m_damageValueLabels.push_back(damageLabel);
            }
        } else if (line.toLower().startsWith("cmd")){
            if (isUnit){
                QStringList baseStatLine = line.split(QRegularExpression("[,|:|;]"));
                if (baseStatLine.size() >= 4){
                    int cmd = baseStatLine.at(0).simplified().mid(baseStatLine.at(0).simplified().indexOf(' ')).toInt();
                    QString def = baseStatLine.at(1).simplified().mid(baseStatLine.at(1).simplified().indexOf(' '));
                    QString save = baseStatLine.at(2).simplified().mid(baseStatLine.at(2).simplified().indexOf(' '));
                    QString move = baseStatLine.at(3).simplified().mid(baseStatLine.at(3).simplified().indexOf(' '));

                    ui->label_cmd->setText(QString::number(cmd));
                    ui->label_def->setText(def);
                    ui->label_save->setText(save);
                    ui->label_move->setText(move);

                    ui->mainDataFrame->setVisible(true);

                    ui->label->setVisible(true);
                    ui->label_2->setVisible(true);
                    ui->label_3->setVisible(true);
                    ui->label_4->setVisible(true);

                    ui->label_cmd->setVisible(true);
                    ui->label_def->setVisible(true);
                    ui->label_save->setVisible(true);
                    ui->label_move->setVisible(true);
                }
            }
            else{
                miscText += "<b>" + line + "</b>" + '\n' + "<br>";
            }
        } else if (line.toLower().startsWith("m_cc")){
            bool isOk = false;
            int cc = line.mid(line.lastIndexOf(":") + 1).toInt(&isOk);
            if (isOk)
                setCount(cc);
        } else{ //Other text
            miscText += line + '\n' + "<br>";
        }
    }
    ui->miscText->setText(miscText);
    if (miscText.size() <= 1){
        ui->miscText->setVisible(false);
    }

    if (m_abilities.size() > 0){
        for (auto& abl : m_abilities){
            abl->ensureTextVisible();
        }
    }
    setAdBackground(m_ADBGColor);
    setSystemBackground(m_systemBGColor);
}

void UnitCard::setCount(int _count)
{
    cardCount = _count;
    if (cardText.toLower().contains("m_cc")){
        int rem_first = cardText.toLower().indexOf("m_cc");
        int rem_last = cardText.toLower().indexOf('\n', rem_first);
        cardText.erase(cardText.begin() + rem_first , cardText.begin() + rem_last );
    }
    cardText.append(QString("m_cc: %1\n").arg(cardCount));
}

void UnitCard::addSystemStump()
{
    QString sys_templ("\n-S%1: [Free] Example system (token): Example descr\n");
    int lastSysPos = cardText.lastIndexOf("-S");
    int insertPos = cardText.size();;
    if (lastSysPos != -1){
        int nextLine = cardText.indexOf('\n',lastSysPos );
        if (nextLine != -1)
            insertPos = nextLine;
    }
    cardText.insert(insertPos, sys_templ.arg(m_abilities.size() + 1));
    ability_frame *nextSystem = new ability_frame(this);
    if (nextSystem->makeFromString(sys_templ.arg(m_abilities.size() + 1))){
        ui->abilitiesLayout->addWidget(nextSystem);
        m_abilities.push_back(nextSystem);
    }
    else{
        nextSystem->deleteLater();
    }
    updateNeeded = true;
    emit requestUpdateImage();
}

void UnitCard::addDamageStump()
{
    QString dmg_templ("Damage: [1]: dead, [2-3]: S1, [4-5]: ARM, [6]: Pin (green)\n");
    cardText.append("\n");
    cardText.append(dmg_templ);
    updateNeeded = true;
    emit requestUpdateImage();
}

void UnitCard::setBackgroundImage(QImage &_bg)
{
    QSize xxx = size();
    QPixmap bg_pix = QPixmap::fromImage(_bg.scaled(size()));
    //bg_image->setPixmap();
    QPalette pal;
    pal.setBrush(QPalette::Window, bg_pix);
    setPalette(pal);
}

void UnitCard::setCardBackground(const QColor &_color)
{
    ui->mainDataFrame->setStyleSheet(QString("QFrame { background: transparent; background-color: %1 }").arg(_color.name(QColor::HexArgb)));
    ui->damageWidget->setStyleSheet(QString("QFrame { background: transparent; background-color: %1 }").arg(_color.name(QColor::HexArgb)));
    ui->miscText->setStyleSheet(QString("QLabel { background: transparent; background-color: %1 }").arg(_color.name(QColor::HexArgb)));
    ui->tagsLabel->setStyleSheet(QString("QLabel { background: transparent; background-color: %1 }").arg(_color.name(QColor::HexArgb)));
}

void UnitCard::setAdBackground(const QColor &_color)
{
    m_ADBGColor = _color;
    for (auto& sys : m_abilities){
        sys->setAdBackground(_color);
    }
}

void UnitCard::setSystemBackground(const QColor &_color)
{
    m_systemBGColor = _color;
    for (auto& sys : m_abilities){
        sys->setSystemBackground(_color);
    }
}

