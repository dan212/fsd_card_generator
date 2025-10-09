#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QPainter>
#include <QProgressDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>
#include "unitcard.h"

#include <set>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setCardBGColor(const QColor &newCardBGColor);

    void setSystemBGColor(const QColor &newSystemBGColor);

    void setADBGColor(const QColor &newADBGColor);

private:
    Ui::MainWindow *ui;
    std::map<int, UnitCard*> m_generatedCards;
    std::set<QString> m_textProcessed;
    UnitCard* m_currentCard = nullptr;
    int generatedIndex = 0;

    QImage m_bgImage;

    QColor m_cardBGColor;
    QColor m_systemBGColor;
    QColor m_ADBGColor;



public slots:
    void generateCardsFromTextbox();
    void generateCardSheets();
    void removeCard();

    void onTableCellClick(int _row, int _column);
    void onCurrentCardDataEdited();
    void onCurrentCardTextChanged();

    void onAddCardStumpButtonClick();
    void onAddSystemStumpButtonClick();
    void onAddDamageStumpButtonClick();

    void onLoadFromFilesButtonClick();
    void onSetBgImageButtonClick();

    void onPickCardBackground();
    void onPickADBackground();
    void onPickSystemBackground();
};
#endif // MAINWINDOW_H
