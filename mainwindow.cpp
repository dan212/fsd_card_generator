#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->cardsLoaded->setColumnCount(3);
    ui->cardsLoaded->setHorizontalHeaderLabels({"Id", "Name", "Count"});


    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::generateCardsFromTextbox);
    connect(ui->generateCardsButton, &QPushButton::clicked, this, &MainWindow::generateCardSheets);
    connect(ui->removeSelectedCardButton, &QPushButton::clicked, this, &MainWindow::removeCard);
    connect(ui->loadFromPngButton, &QPushButton::clicked, this, &MainWindow::onLoadFromFilesButtonClick);
    connect(ui->addDamageButton, &QPushButton::clicked, this, &MainWindow::onAddDamageStumpButtonClick);
    connect(ui->addSystemButton, &QPushButton::clicked, this, &MainWindow::onAddSystemStumpButtonClick);
    connect(ui->templateButton, &QPushButton::clicked, this, &MainWindow::onAddCardStumpButtonClick);
    connect(ui->setBgImageButton, &QPushButton::clicked, this, &MainWindow::onSetBgImageButtonClick);

    connect(ui->pickADBackground, &QPushButton::clicked, this, &MainWindow::onPickADBackground);
    connect(ui->pickCardBackground, &QPushButton::clicked, this, &MainWindow::onPickCardBackground);
    connect(ui->pickSystemBackground, &QPushButton::clicked, this, &MainWindow::onPickSystemBackground);

    connect(ui->cardsLoaded, &QTableWidget::cellClicked, this, &MainWindow::onTableCellClick);

    connect(ui->currentCardData, &QPlainTextEdit::textChanged,this, &MainWindow::onCurrentCardDataEdited);

    connect(ui->bgAlphaSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](){
        setCardBGColor(m_cardBGColor);
        setSystemBGColor(m_systemBGColor);
        setADBGColor(m_ADBGColor);
    });

    m_cardBGColor = QColor::fromRgb(255,255,255,140);
    m_systemBGColor = QColor::fromRgb(255,255,255,100);
    m_ADBGColor = QColor::fromRgb(255,189,91,140);

    QPixmap adPixmap = QPixmap(ui->adBgLabel->size());
    adPixmap.fill(m_ADBGColor);
    ui->adBgLabel->setPixmap(adPixmap);

    QPixmap cardPixmap = QPixmap(ui->cardBgLabel->size());
    cardPixmap.fill(m_cardBGColor);
    ui->cardBgLabel->setPixmap(cardPixmap);

    QPixmap systemPixmap = QPixmap(ui->systemBgLabel->size());
    systemPixmap.fill(m_systemBGColor);
    ui->systemBgLabel->setPixmap(systemPixmap);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCardBGColor(const QColor &newCardBGColor)
{
    m_cardBGColor = newCardBGColor;
    m_cardBGColor.setAlpha(ui->bgAlphaSpinBox->value());
    for (auto& card : m_generatedCards){
        card.second->setCardBackground(m_cardBGColor);
    }
    onCurrentCardTextChanged();

    QPixmap cardPixmap = QPixmap(ui->cardBgLabel->size());
    cardPixmap.fill(m_cardBGColor);
    ui->cardBgLabel->setPixmap(cardPixmap);

}

void MainWindow::setSystemBGColor(const QColor &newSystemBGColor)
{
    m_systemBGColor = newSystemBGColor;
    m_systemBGColor.setAlpha(ui->bgAlphaSpinBox->value());
    for (auto& card : m_generatedCards){
        card.second->setSystemBackground(m_systemBGColor);
    }
    onCurrentCardTextChanged();

    QPixmap systemPixmap = QPixmap(ui->systemBgLabel->size());
    systemPixmap.fill(m_systemBGColor);
    ui->systemBgLabel->setPixmap(systemPixmap);
}

void MainWindow::setADBGColor(const QColor &newADBGColor)
{
    m_ADBGColor = newADBGColor;
    m_ADBGColor.setAlpha(ui->bgAlphaSpinBox->value());
    for (auto& card : m_generatedCards){
        card.second->setAdBackground(m_ADBGColor);
    }
    onCurrentCardTextChanged();

    QPixmap adPixmap = QPixmap(ui->adBgLabel->size());
    adPixmap.fill(m_ADBGColor);
    ui->adBgLabel->setPixmap(adPixmap);
}

void MainWindow::generateCardsFromTextbox()
{
    QString inpData = ui->plainTextEdit->toPlainText();

    QStringList individualData = inpData.split("___");

    QProgressDialog* pr_d = new QProgressDialog("Parcing data...", "Cancel", 0 , individualData.size(), this);
    pr_d->setWindowModality(Qt::WindowModal);

    int progress = 0;
    for (auto& strData : individualData){
        if (strData.simplified().size() == 0){
            progress++;
            pr_d->setValue(progress);
            continue;
        }

        if (strData.at(0) == '\n')
            strData = strData.mid(1);

        if (m_textProcessed.find(strData) != m_textProcessed.end()){
            progress++;
            pr_d->setValue(progress);
            continue;
        }

        UnitCard* card = new UnitCard(this);
        card->parceFromText(strData);
        card->setBackgroundImage(m_bgImage);
        card->setCardBackground(m_cardBGColor);
        card->setAdBackground(m_ADBGColor);
        card->setSystemBackground(m_systemBGColor);

        m_textProcessed.insert(strData);

        connect(card, &UnitCard::requestUpdateImage, this, &MainWindow::onCurrentCardTextChanged);

        //ui->cardsLayout->addWidget(card, 1,1);
        int index = ++generatedIndex;
        card->setIndex(index);
        m_generatedCards[index] = card;

        ui->cardsLoaded->setRowCount(m_generatedCards.size());

        QLabel *indexLabel = new QLabel(QString::number(index),ui->cardsLoaded );
        indexLabel->setProperty("index", index);
        ui->cardsLoaded->setCellWidget(m_generatedCards.size() - 1, 0, indexLabel);

        QLabel *nameLabel = new QLabel(card->getName(),ui->cardsLoaded );
        ui->cardsLoaded->setCellWidget(m_generatedCards.size() - 1, 1, nameLabel);

        QSpinBox *countSpinBox = new QSpinBox(ui->cardsLoaded );
        countSpinBox->setValue(card->getCount());
        ui->cardsLoaded->setCellWidget(m_generatedCards.size() - 1, 2, countSpinBox);
        progress++;
        pr_d->setValue(progress);

        if (pr_d->wasCanceled())
            break;
        //card->grab().save("test.png");
    }
    //pr_d->deleteLater();
}

void MainWindow::generateCardSheets()
{
    QString saveFolderDir = QFileDialog::getExistingDirectory(this, "Select Directory to save");

    if (saveFolderDir.size() == 0)
        return;

    QDir saveDir(saveFolderDir);
    bool createdSubDirsOk = false;
    createdSubDirsOk = saveDir.mkdir("single") || saveDir.exists("single");
    createdSubDirsOk &= saveDir.mkdir("combined") || saveDir.exists("combined");

    if (!createdSubDirsOk){
        QMessageBox msgBox;
        msgBox.setText("Can not create 'single' and 'combined' sub-folders.");
        msgBox.exec();
        return;
    }

    QString saveDirSingle = saveFolderDir + "/single";
    QString saveDirCombined = saveFolderDir + "/combined";

    int totalCardCount = 0;
    for (const auto& card : m_generatedCards){
        totalCardCount += card.second->getCount();
    }
    QProgressDialog* pr_d = new QProgressDialog("Generating cards...", "Cancel", 0 , totalCardCount, this);
    pr_d->setWindowModality(Qt::WindowModal);


    if (!m_bgImage.isNull()){ //Save background image with some card design data
        m_bgImage.setText("System_BG", QVariant(m_systemBGColor).toString());
        m_bgImage.setText("Card_BG", QVariant(m_cardBGColor).toString());
        m_bgImage.setText("AD_BG", QVariant(m_ADBGColor).toString());
        m_bgImage.setText("Alpha_bg", QVariant(ui->bgAlphaSpinBox->value()).toString());

        m_bgImage.save(QString("%1/background.png").arg(saveDirSingle));
    }

    totalCardCount = 0;
    QImage currentPage(318 * 3, 441 * 3, QImage::Format::Format_RGBA8888);
    QPainter localPainter(&currentPage);
    localPainter.fillRect(0, 0, 318 * 3, 441 * 3, Qt::GlobalColor::white);
    int currentPageCount = 0;
    int currentCardCount = 0;
    QString cardText;
    for (int i = 0; i < ui->cardsLoaded->rowCount(); i++){
        int index = ui->cardsLoaded->cellWidget(i, 0)->property("index").toInt();
        int count = dynamic_cast<QSpinBox*>( ui->cardsLoaded->cellWidget(i, 2))->value();
        m_generatedCards.at(index)->setCount(count);
        for (int j = 0; j < count; j++){
            if (m_generatedCards.find(index) != m_generatedCards.end()){
                QImage currentCard = m_generatedCards.at(index)->grab().toImage();
                localPainter.drawImage(QPoint(318 * (currentCardCount % 3), 441 * (currentCardCount / 3)), currentCard);
                cardText += m_generatedCards.at(index)->getText();
                cardText += "\n___\n";

                currentCard.setText("cardText", m_generatedCards.at(index)->getText() + "\n___\n");
                currentCard.save(QString("%1/%2.png").arg(saveDirSingle).arg(m_generatedCards.at(index)->getName()));
            }
            currentCardCount++;
            totalCardCount++;
            pr_d->setValue(totalCardCount);
            if (currentCardCount == 9){
                currentPage.setText("cardText", cardText);
                currentPage.save(QString("%1/page_%2.png").arg(saveDirCombined).arg(currentPageCount));
                currentPageCount++;

                //currentPage = QImage(318 * 3, 441 * 3, QImage::Format::Format_RGBA8888);
                localPainter.fillRect(0, 0, 318 * 3, 441 * 3, Qt::GlobalColor::white);
                currentCardCount = 0;
                cardText.clear();
            }
        }
    }

    if (currentCardCount > 0){
        currentPage.setText("cardText", cardText);
        cardText.clear();
        currentPage.save(QString("%1/page_%2.png").arg(saveDirCombined).arg(currentPageCount));
    }
}

void MainWindow::removeCard()
{
    auto list = ui->cardsLoaded->selectedItems().size();
    int row = ui->cardsLoaded->currentRow();
    if (row > -1){
        std::set<int> removeIndex;
        std::set<int> removeRows;
        int index = ui->cardsLoaded->cellWidget(row, 0)->property("index").toInt();
        removeIndex.insert(index);
        removeRows.insert(row);

        if (removeRows.size() > 0) {
            std::vector<int> sortedRows;
            for (const int row : removeRows)
                sortedRows.push_back(row);

            std::sort(sortedRows.begin(), sortedRows.end());

            for (int i = sortedRows.size() - 1; i >= 0; i--){
                ui->cardsLoaded->removeRow(sortedRows.at(i));
            }
        }

        if (removeIndex.size() > 0) {
            for (const int ind : removeIndex)
                m_generatedCards.erase(ind);
        }

        ui->cardDisplayLabel->clear();
        ui->currentCardData->clear();
        m_currentCard = nullptr;
    }
}

void MainWindow::onTableCellClick(int _row, int _column)
{
    if (ui->cardsLoaded->cellWidget(_row, 0) != nullptr){
        int index = ui->cardsLoaded->cellWidget(_row, 0)->property("index").toInt();

        if (m_generatedCards.find(index) != m_generatedCards.end()){
            m_currentCard = m_generatedCards.at(index);
            //m_currentCard->setVisible(true);
            ui->cardDisplayLabel->setPixmap(m_currentCard->grab());
            ui->currentCardData->setPlainText(m_currentCard->getText());
        }
    }
}

void MainWindow::onCurrentCardDataEdited()
{
    QString curData = ui->currentCardData->toPlainText();
    if (curData.size() > 0 && m_currentCard != nullptr){
        m_currentCard->parceFromText(curData);

        ui->cardDisplayLabel->clear();
        ui->cardDisplayLabel->setPixmap(m_currentCard->grab());

        QLabel* nameLabel = dynamic_cast<QLabel*>(ui->cardsLoaded->cellWidget(ui->cardsLoaded->currentRow(), 1));
        if (nameLabel)
            nameLabel->setText(m_currentCard->getName());

        //ui->currentCardData->setPlainText(m_currentCard->getText());
    }
}

void MainWindow::onCurrentCardTextChanged()
{
    if (m_currentCard){
        ui->currentCardData->setPlainText(m_currentCard->getText());
        onCurrentCardDataEdited();
    }
}

void MainWindow::onAddCardStumpButtonClick()
{
    QString strData = "Template Unit - 0 pts\n";
    strData += "Infantry – X Bases\n";
    strData += "Cmd -: Def 3+: Save 1d8: Move 2DU\n";
    strData += "Tags:\n";
    UnitCard* card = new UnitCard(this);
    card->parceFromText(strData);
    card->setBackgroundImage(m_bgImage);
    card->setCardBackground(m_cardBGColor);
    card->setAdBackground(m_ADBGColor);
    card->setSystemBackground(m_systemBGColor);

    connect(card, &UnitCard::requestUpdateImage, this, &MainWindow::onCurrentCardTextChanged);

    //ui->cardsLayout->addWidget(card, 1,1);
    int index = ++generatedIndex;
    card->setIndex(index);
    m_generatedCards[index] = card;

    ui->cardsLoaded->setRowCount(m_generatedCards.size());

    QLabel *indexLabel = new QLabel(QString::number(index),ui->cardsLoaded );
    indexLabel->setProperty("index", index);
    ui->cardsLoaded->setCellWidget(m_generatedCards.size() - 1, 0, indexLabel);

    QLabel *nameLabel = new QLabel(card->getName(),ui->cardsLoaded );
    ui->cardsLoaded->setCellWidget(m_generatedCards.size() - 1, 1, nameLabel);

    QSpinBox *countSpinBox = new QSpinBox(ui->cardsLoaded );
    countSpinBox->setValue(1);
    ui->cardsLoaded->setCellWidget(m_generatedCards.size() - 1, 2, countSpinBox);

    onTableCellClick(m_generatedCards.size(), 0);
}

void MainWindow::onAddSystemStumpButtonClick()
{
    if (m_currentCard)
        m_currentCard->addSystemStump();
}

void MainWindow::onAddDamageStumpButtonClick()
{
    if (m_currentCard)
        m_currentCard->addDamageStump();
}

void MainWindow::onLoadFromFilesButtonClick()
{
    QList<QString> files = QFileDialog::getOpenFileNames(this, "Select cards/sheets to load", "", "PNG images (*.png)");
    QString totalText;
    if (files.size() > 0){
        int curProgres = 0;
        QProgressDialog* pr_d = new QProgressDialog("Generating cards...", "Cancel", 0 , files.size(), this);
        pr_d->setWindowModality(Qt::WindowModal);
        ui->cardsLoaded->clear();
        for (auto& card : m_generatedCards)
            card.second->deleteLater();
        m_generatedCards.clear();

        for (const auto& file_p : files){
            if (file_p.endsWith("background.png")){
                m_bgImage = QImage(file_p);
                m_systemBGColor = QVariant(m_bgImage.text("System_BG")).value<QColor>();
                m_cardBGColor = QVariant(m_bgImage.text("Card_BG")).value<QColor>();
                m_ADBGColor = QVariant(m_bgImage.text("AD_BG")).value<QColor>();
                ui->bgAlphaSpinBox->setValue(m_bgImage.text("Alpha_bg").toInt());
                curProgres++;
                pr_d->setValue(curProgres);
            }
            else{
                QImage sheet = QImage(file_p);
                if (!sheet.isNull()){
                    QString text = sheet.text("cardText");
                    totalText.append(text);
                }
                curProgres++;
                pr_d->setValue(curProgres);
            }
        }
        ui->plainTextEdit->setPlainText(totalText);
        generateCardsFromTextbox();
    }
}

void MainWindow::onSetBgImageButtonClick()
{
    QString imgPath = QFileDialog::getOpenFileName(this, "Select background image", "", "PNG images (*.png);;JPG images (*.jpg);;JPEG images (*.jpeg);;BMP images (*.bmp)");

    if (imgPath.size() > 0){
        m_bgImage = QImage(imgPath);
        for (auto& card : m_generatedCards){
            card.second->setBackgroundImage(m_bgImage);
        }
        onCurrentCardTextChanged();
    }
}

void MainWindow::onPickCardBackground()
{
    QColor newColor = QColorDialog::getColor(Qt::white, this, "Card background color");
    if (newColor.isValid()){
        setCardBGColor(newColor);
    }
}

void MainWindow::onPickADBackground()
{
    QColor newColor = QColorDialog::getColor(Qt::white, this, "Card background color");
    if (newColor.isValid()){
        setADBGColor(newColor);
    }
}

void MainWindow::onPickSystemBackground()
{
    QColor newColor = QColorDialog::getColor(Qt::white, this, "Card background color");
    if (newColor.isValid()){
        setSystemBGColor(newColor);
    }
}
