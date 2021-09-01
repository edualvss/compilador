#ifndef MDICHILD_H
#define MDICHILD_H

#include <QPlainTextEdit>


QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class HighLighter;
class LineNumberArea;

class MdiChild : public QPlainTextEdit {
    Q_OBJECT

public:
    MdiChild(bool assembly = false);

    // TextEdit
    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

    // Code Editor
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    // TextEdit
    void closeEvent(QCloseEvent *event);
    // Code Editor
    void resizeEvent(QResizeEvent *event);

private slots:
    // TextEdit
    void documentWasModified();

    // Code Editor
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    // TextEdit
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    HighLighter* lighter;
    QString curFile;
    bool isUntitled;

    // Code Editor
    QWidget *lineNumberArea;

    bool isAsm;

};


class LineNumberArea : public QWidget {
public:
    LineNumberArea(MdiChild *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    MdiChild *codeEditor;
};

#endif
