#include "View/mdichild.h"

#include "View/highlighter.h"

#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QTextStream>
#include <QPainter>

MdiChild::MdiChild(bool assembly) {
    isAsm = assembly;
    if(isAsm) {
        this->setReadOnly(true);
    }

    // TextEdit
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;

    setLineWrapMode( NoWrap );

    QFont font;
    font.setFamily("Courier");
    font.setPointSize(12);
    font.setWordSpacing(2.5);
    QFontMetrics metrics(font);
    this->setTabStopDistance(2 * metrics.horizontalAdvance(' '));
    this->setFont(font);

    lighter = new HighLighter(this->document());

    // Code Editor
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

}

void MdiChild::newFile() {

    static int sequenceNumber = 1;

    isUntitled = true;
    if(isAsm) {
        curFile = tr("assembly.asm");
    } else {
        curFile = tr("main%1.eas").arg(sequenceNumber++);
    }
    setWindowTitle(curFile + "[*]");

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

bool MdiChild::loadFile(const QString &fileName) {

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("IDEAS"),
                             tr("Não foi possível ler o arquivo %1:\nErro: %2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    return true;
}

bool MdiChild::save() {

    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MdiChild::saveAs() {

    QString fileName;
    if(isAsm) {
        fileName = QFileDialog::getSaveFileName(this,tr("Salvar como"),
                                                curFile,tr("Código Assembly(*.asm)"));
    } else {
        fileName = QFileDialog::getSaveFileName(this,tr("Salvar como"),
                                               curFile,tr("Código IDE EAS(*.eas);;Código C(*.c)"));
    }

    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MdiChild::saveFile(const QString &fileName) {

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("IDEAS"),
                             tr("Não foi possível escrever no arquivo %1:\nErro: %2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out <<  toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString MdiChild::userFriendlyCurrentFile() {

    return strippedName(curFile);
}

void MdiChild::closeEvent(QCloseEvent *event) {

    if (maybeSave()) {
        event->accept();
        delete lighter;
    } else {
        event->ignore();
    }
}

void MdiChild::documentWasModified() {

    setWindowModified(document()->isModified());
}

bool MdiChild::maybeSave() {

    if (document()->isModified()) {
	QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("IDEAS"),
                     tr("'%1' foi alterado.\n"
                        "Deseja salvar suas alterações?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
		     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MdiChild::setCurrentFile(const QString &fileName) {

    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}

//////// Code editor ////////
int MdiChild::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}


void MdiChild::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void MdiChild::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void MdiChild::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void MdiChild::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::blue).lighter(190);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}



void MdiChild::lineNumberAreaPaintEvent(QPaintEvent *event) {
//![extraAreaPaintEvent_0]
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

//![extraAreaPaintEvent_0]

//![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
//![extraAreaPaintEvent_1]

//![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
//![extraAreaPaintEvent_2]
}



