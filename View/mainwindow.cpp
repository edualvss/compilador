#include "View/mainwindow.h"
#include "View/mdichild.h"
#include "View/telaabout.h"

#include "Model/Variavel.h"

#include <QMdiArea>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSplitter>
#include <QStatusBar>
#include <QHeaderView>
#include <QSignalMapper>
#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QApplication>
#include <QSettings>

MainWindow::MainWindow() {
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QGroupBox* groupConsole = new QGroupBox(tr("Console"),this);
    groupConsole->setStatusTip(tr("Console de mensagens"));
    QHBoxLayout* layout = new QHBoxLayout(groupConsole);
    groupConsole->setLayout(layout);
    console = new QPlainTextEdit(groupConsole);
    console->setReadOnly(true);
    layout->addWidget(console);

    QGroupBox* groupTable = new QGroupBox(tr("Tabela de Símbolos"),this);
    groupTable->setStatusTip(tr("Tabela de símbolos"));
    QHBoxLayout* l = new QHBoxLayout(groupTable);
    groupTable->setLayout(l);
    tabela = new QTableWidget(groupTable);
    tabela->setColumnCount(5);
    QStringList cabecalho = QStringList() << "Tipo" << "ID" << "Escopo" << "Inicializada" << "Utilizada";
    tabela->setHorizontalHeaderLabels(cabecalho);
    tabela->horizontalHeader()->setVisible(true);
    tabela->verticalHeader()->setVisible(false);
    l->addWidget(tabela);

    QSplitter* tableSplitter = new QSplitter(Qt::Horizontal,this);
    tableSplitter->addWidget(mdiArea);
    tableSplitter->addWidget(groupTable);
    QSplitter* mainSplitter = new QSplitter(Qt::Vertical,this);
    mainSplitter->addWidget(tableSplitter);
    mainSplitter->addWidget(groupConsole);

    setCentralWidget(mainSplitter);

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mappedObject(QObject*)),
            this, SLOT(setActiveSubWindow(QObject*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("IDEAS"));
    setUnifiedTitleAndToolBarOnMac(true);

    setWindowIcon( QIcon(":/icons/about.png") );
}

void MainWindow::clearConsole() {

    console->clear();

}

void MainWindow::printMessageConsole(QString msg, bool error) {
    if(error) {
        console->appendHtml(QString("<font color=red>%1</font>").arg(msg));
    } else {
        console->appendHtml(msg);
    }

}

void MainWindow::setDataTable(QList<Variavel *> *lista) {

    int numLinhas = lista->size();
//    this->tabela->clear();
    this->tabela->setRowCount(numLinhas);

    for(int i = 0; i < numLinhas; i++) {
        Variavel* v = lista->at(i);
        QTableWidgetItem* tp = new QTableWidgetItem();
        QTableWidgetItem* id = new QTableWidgetItem();
        QTableWidgetItem* escopo = new QTableWidgetItem();
        QTableWidgetItem* inicializada = new QTableWidgetItem();
        QTableWidgetItem* utilizada = new QTableWidgetItem();

        tp->setFlags( tp->flags() ^ Qt::ItemIsEditable );
        id->setFlags( id->flags() ^ Qt::ItemIsEditable);
        escopo->setFlags( escopo->flags() ^ Qt::ItemIsEditable );
        inicializada->setFlags( inicializada->flags() ^ Qt::ItemIsEditable );
        utilizada->setFlags( utilizada->flags() ^ Qt::ItemIsEditable );

        tp->setTextAlignment(Qt::AlignCenter);
        id->setTextAlignment(Qt::AlignCenter);
        escopo->setTextAlignment(Qt::AlignCenter);
        inicializada->setTextAlignment(Qt::AlignCenter);
        utilizada->setTextAlignment(Qt::AlignCenter);

        QString tipo;
        switch(v->getTipo()) {
            case TipoVariavel::Caracter:
                tipo = "char";
                break;
            case TipoVariavel::Desconhecido:
                tipo = "Desconhecido";
                break;
            case TipoVariavel::Inteiro:
                tipo = "int";
                break;
            case TipoVariavel::Real:
                tipo = "float";
                break;
            case TipoVariavel::String:
                tipo = "char*";
                break;
            case TipoVariavel::Bool:
                tipo = "bool";
                break;
        }

        bool usada = v->isUsada();

        QString ini = v->isInicializada() ? tr("Sim"):tr("Não");
        QString use = usada ? tr("Sim"):tr("Não");
        QString esc = QString::number(v->getEscopo()->getEscopo());
        QString idVar = v->getId();

        if(!usada) {
            this->printMessageConsole(tr("<font color=gray>Variável \"%1\" no escopo \"%2\" não utilizada!</font>").arg(idVar).arg(esc));
        }

        tp->setText(tipo);
        id->setText(idVar);
        escopo->setText(esc);
        inicializada->setText( ini );
        utilizada->setText( use );

        tabela->setItem(i,0,tp);
        tabela->setItem(i,1,id);
        tabela->setItem(i,2,escopo);
        tabela->setItem(i,3,inicializada);
        tabela->setItem(i,4,utilizada);

        delete v;
    }

    delete lista;

}

void MainWindow::closeEvent(QCloseEvent *event) {

    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile() {
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();

}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                            tr("Abrir código fonte"), "",
                                            tr("Código IDE EAS(*.eas);;Código C(*.c)"));
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("Arquivo carregado"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
}

void MainWindow::save() {
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("Arquivo salvo"), 2000);
}

void MainWindow::saveAs() {
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("Arquivo salvo"), 2000);
}

void MainWindow::cut() {
    if (activeMdiChild())
        activeMdiChild()->cut();
}

void MainWindow::copy() {
    if (activeMdiChild())
        activeMdiChild()->copy();
}

void MainWindow::paste() {
    if (activeMdiChild())
        activeMdiChild()->paste();
}

void MainWindow::about() {

    TelaAbout* about = new TelaAbout(this);

    about->exec();

    delete about;

}

void MainWindow::updateMenus() {
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    lexicalAct->setEnabled(hasMdiChild);
    syntacticAct->setEnabled(hasMdiChild);

    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu() {
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MdiChild *MainWindow::createMdiChild(bool assembly) {
    MdiChild *child = new MdiChild(assembly);
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    return child;
}

void MainWindow::createActions() {
    newAct = new QAction(QIcon(":/icons/icon_new.png"), tr("&Novo"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Criar um novo arquivo"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/icons/icon_open.png"), tr("&Abrir..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Abrir um arquivo existente"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/icons/icon_save.png"), tr("&Salvar"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Salvar documento no disco"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/icons/icon_saveAs.png"),tr("Salvar Como..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Salvar documento com novo nome"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

//! [0]
    exitAct = new QAction(QIcon(":/icons/icon_exit.png"),tr("Sair"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Sair da aplicação"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
//! [0]

    cutAct = new QAction(QIcon(":/icons/cut.png"), tr("Recor&tar"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cortar o conteúdo da seleção atual e colocar na área de transferência"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/icons/copy.png"), tr("&Copiar"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copiar o conteúdo da seleção atual e colocar na área de transferência"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/icons/paste.png"), tr("Co&lar"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Colar o conteúdo da área de transferência no local da seleção"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    closeAct = new QAction(tr("&Fechar"), this);
    closeAct->setStatusTip(tr("Fechar a janela atual"));
    connect(closeAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Fechar &Todos"), this);
    closeAllAct->setStatusTip(tr("Fechar todas as janelas"));
    connect(closeAllAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Enquadrar"), this);
    tileAct->setStatusTip(tr("Enquadrar as janelas no espaço disponível"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("Cascata"), this);
    cascadeAct->setStatusTip(tr("Deixar as janelas em cascata no espaço disponível"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Pró&ximo"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Mover o foco para a próxima janela"));
    connect(nextAct, SIGNAL(triggered()),
            mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Ante&rior"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Mover o foco para a janela anterior"));
    connect(previousAct, SIGNAL(triggered()),
            mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    lexicalAct = new QAction(QIcon(":/icons/icon_lexico.png"),tr("&Léxico"),this );
    lexicalAct->setShortcut( Qt::Key_F8 );
    lexicalAct->setStatusTip(tr("Realizar a análise léxica da janela em foco"));
    connect(lexicalAct,SIGNAL(triggered()),this,SLOT(lexicalAnalysis()));

    syntacticAct = new QAction(QIcon(":/icons/icon_sintatico.png"),tr("&Sintático e Semântico"),this);
    syntacticAct->setShortcut( Qt::Key_F9 );
    syntacticAct->setStatusTip(tr("Realizar a análise sintática & semântica da janela em foco"));
    connect(syntacticAct,SIGNAL(triggered()),this,SLOT(syntacticAnalysis()));

    aboutAct = new QAction(QIcon(":/icons/about.png"),tr("&Sobre"), this);
    aboutAct->setStatusTip(tr("Sobre a aplicação"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(":/icons/icon_qt.png"),tr("Sobre &Qt"), this);
    aboutQtAct->setStatusTip(tr("Sobre o Qt, framework utilizado no desenvolvimento"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&Arquivo"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    QAction *action = fileMenu->addAction(tr("Trocar direção do layout"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Editar"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    windowMenu = menuBar()->addMenu(tr("&Janela"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    analysisMenu = menuBar()->addMenu(tr("Aná&lise"));
    analysisMenu->addAction(lexicalAct);
    analysisMenu->addAction(syntacticAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("Aj&uda"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars() {
    fileToolBar = addToolBar(tr("Arquivo"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Editar"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    analysisToolBar = addToolBar(tr("Análise"));
    analysisToolBar->addAction(lexicalAct);
    analysisToolBar->addAction(syntacticAct);
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Pronto"));
}

void MainWindow::readSettings() {
    QSettings settings("EAS", "IDEAS");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings() {
    QSettings settings("EAS", "IDEAS");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

MdiChild *MainWindow::activeMdiChild() {
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) {
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::switchLayoutDirection() {
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::setActiveSubWindow(QObject *window) {

    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::lexicalAnalysis() {

    MdiChild* mdi = activeMdiChild();
    if(mdi != 0) {
        statusBar()->showMessage(tr("Análise léxica em execução"), 2000);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        emit analysisLexical( mdi->toPlainText() );
        QApplication::restoreOverrideCursor();
    }

}

void MainWindow::syntacticAnalysis() {
    MdiChild* mdi = activeMdiChild();
    if(mdi != 0) {
        statusBar()->showMessage(tr("Análise sintática em execução"), 2000);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        emit analysisSyntactic( mdi->toPlainText() );
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::showAssembly(QString src) {

    MdiChild *child = createMdiChild(true);
    child->setPlainText(src);
    child->newFile();
    child->show();


}
