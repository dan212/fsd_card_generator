#ifndef UNITCARD_H
#define UNITCARD_H

#include <QFrame>
#include <QString>
#include <QRegularExpression>
#include <QPalette>
#include <QColor>

#include "ability_frame.h"

namespace Ui {
class UnitCard;
}

class UnitCard : public QFrame
{
    Q_OBJECT

public:
    explicit UnitCard(QWidget *parent = nullptr);
    ~UnitCard();

private:
    Ui::UnitCard *ui;
    std::vector<ability_frame*> m_abilities;
    std::vector<QLabel*> m_damageValueLabels;

    const QRegularExpression splitters = QRegularExpression("[,|:|;|-|–]");

    QString cardText;
    QString name;
    int index = 0;
    int cardCount = 1;
    bool updateNeeded = false;

    QLabel *bg_image = nullptr;

    QColor m_systemBGColor;
    QColor m_ADBGColor;
public:
    void parceFromText(QString _data);
    void setIndex(int _index) {index = _index;};

    int getIndex() {return index;};
    QString getText() {return cardText;};
    QString getName() {return name;};
    int getCount() {return cardCount;};
    void setCount(int _count);

    void addSystemStump();
    void addDamageStump();

    void setBackgroundImage(QImage& _bg);

    void setCardBackground(const QColor& _color);
    void setAdBackground(const QColor& _color);
    void setSystemBackground(const QColor& _color);
signals:
    void requestUpdateImage();
};

#endif // UNITCARD_H
