#ifndef ABILITY_FRAME_H
#define ABILITY_FRAME_H

#include <QFrame>
#include <QLabel>
#include <QRegularExpression>
#include <QFontMetricsF>

namespace Ui {
class ability_frame;
}

class ability_frame : public QFrame
{
    Q_OBJECT

public:
    explicit ability_frame(QWidget *parent = nullptr);
    ~ability_frame();

    void setIndex(int _count);
    void setName(QString _name);
    void setDescr(QString _descr);
    void setADValues(std::vector<std::pair<int,int>> _ads);

    bool makeFromString(QString _inp);
    void ensureTextVisible();
private:
    Ui::ability_frame *ui;
    std::vector<QLabel*> m_adLabels;

    const QRegularExpression splitters = QRegularExpression("[,|:|;|-|–]");
};

#endif // ABILITY_FRAME_H
