#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Variavel;
class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QPlainTextEdit;
class QTableWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

    void setDataTable(QList<Variavel* >* lista);
    void printMessageConsole(QString msg,bool error = false);
    void clearConsole();

    void showAssembly(QString src);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void cut();
    void copy();
    void paste();
    void about();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild(bool assembly = false);
    void switchLayoutDirection();
    void setActiveSubWindow(QObject *window);

    void lexicalAnalysis();
    void syntacticAnalysis();

signals:
    void analysisLexical(QString src);
    void analysisSyntactic(QString src);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);

    QMdiArea *mdiArea;
    QPlainTextEdit* console;
    QTableWidget* tabela;
    QSignalMapper *windowMapper;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QMenu *analysisMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *analysisToolBar;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;

    QAction *lexicalAct;
    QAction *syntacticAct;

    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
