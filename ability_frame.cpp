#include "ability_frame.h"
#include "ui_ability_frame.h"

ability_frame::ability_frame(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ability_frame)
{
    ui->setupUi(this);

    //setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::MinimumExpanding);
    //ui->Descr->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
    ui->gridLayout->setAlignment(Qt::AlignTop);
    ui->adBoxLayout->setAlignment(Qt::AlignTop);

    setLayout(ui->gridLayout);
}

ability_frame::~ability_frame()
{
    delete ui;
}

void ability_frame::setIndex(int _count)
{
    ui->systemIdent->setText(QString("<b>S%1</b>").arg(_count));
}

void ability_frame::setName(QString _name)
{
    ui->SystemName->setText(_name);
}

void ability_frame::setDescr(QString _descr)
{
    ui->Descr->setText(_descr);
}

void ability_frame::setADValues(std::vector<std::pair<int, int> > _ads)
{
    for (int i = 0; i < m_adLabels.size(); ++i){
        m_adLabels[i]->deleteLater();
    }
    m_adLabels.clear();

    int adCount = 0;
    for (const auto adP : _ads){
        QString adStr = "Free";
        if (adP.first != 0 && adP.second != 0){
            if (adP.first == adP.second){
                adStr = QString("%1").arg(adP.first);
            }
            else{
                adStr = QString("%1-%2").arg(adP.first).arg(adP.second);
            }
        }
        QLabel *adLabel = new QLabel(adStr, this);
        adLabel->setFixedSize(32,32);
        adLabel->setFrameStyle(QFrame::Panel);
        adLabel->setLineWidth(2);
        adLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

        ui->adBoxLayout->addWidget(adLabel, adCount / 2, adCount % 2);
        ui->adBoxLayout->setAlignment(adLabel,Qt::AlignTop);

        m_adLabels.push_back(adLabel);
        adCount++;
    }
    setMinimumHeight(15 + 2 + ui->adBoxLayout->rowCount() * 32);
}

bool ability_frame::makeFromString(QString _inp)
{
    QStringList parts = _inp.split(':');
    if (parts.size() < 2)
        return false;
    int index = parts.at(0).right(1).toInt();
    setIndex(index);
    int lastBrac = parts.at(1).lastIndexOf(']');
    int firstBrac = parts.at(1).indexOf('[');
    QString name = parts.at(1).mid( lastBrac+1);
    ui->tokenFrame->setVisible(false);
    if (name.toLower().contains("(token)")){
        name.remove("(token)", Qt::CaseInsensitive);
        ui->tokenFrame->setVisible(true);
    }
    setName(name);
    QString adString = parts.at(1).mid(firstBrac, lastBrac - firstBrac + 1);

    std::vector<std::pair<int, int> > vals;
    if (adString.toLower().contains("free")){
        vals.push_back(std::pair<int, int>(0, 0));
    }
    else{
        QStringList adStrings = adString.split(']', Qt::SkipEmptyParts);

        for (auto& str : adStrings){
            int l_ad = str.mid(1,1).toInt();
            int r_ad = str.right(1).toInt();
            vals.push_back(std::pair<int, int>(l_ad, r_ad));
        }
    }
    setADValues(vals);
    if (parts.size() > 2)
        setDescr(parts.at(2));

    return true;
}

void ability_frame::ensureTextVisible()
{
    QString descrText = ui->Descr->text();
    QFontMetricsF metric(ui->Descr->font());
    QRectF widgetRect = ui->Descr->rect();
    QRectF rect = metric.boundingRect(ui->Descr->rect(), Qt::AlignLeft, descrText);
    double descrHeight = ui->Descr->height();
    double textHeight = rect.height();
    int linesWritten = rect.width() / widgetRect.width() + 1;
    if (textHeight > descrHeight){
        QFont curFont = ui->Descr->font();
        int curSize = curFont.pixelSize();
        curFont.setPixelSize(curSize - 1);
        ui->Descr->setFont(curFont);
        ui->Descr->setText(descrText);
    }
}
