#include "Control/controle.h"

#include "View/mainwindow.h"

#include "Model/Lexico.h"
#include "Model/Semantico.h"
#include "Model/Sintatico.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

//#include <QDebug>

Controle::Controle(QObject *parent) :
    QObject(parent) {

    this->telaPrincipal = (MainWindow* ) parent;

}

void Controle::estabelecerConexoes() {

    connect(telaPrincipal,SIGNAL(analysisLexical(QString)),this,SLOT(executarAnaliseLexica(QString)));
    connect(telaPrincipal,SIGNAL(analysisSyntactic(QString)),this,SLOT(executarAnaliseSintatica(QString)));

}

void Controle::iniciarAplicacao() {
    if(!telaPrincipal) {
        telaPrincipal = new MainWindow();
    }

    this->estabelecerConexoes();

    this->telaPrincipal->show();
}

void Controle::executarAnaliseLexica(QString src) {
    Lexico* lexico = new Lexico();
    lexico->setInput(src.toStdString().c_str());
    try {
        this->telaPrincipal->clearConsole();
        this->telaPrincipal->printMessageConsole(tr("Análise Léxica"));
        Token* t = 0;
        while( (t = lexico->nextToken()) != 0 ) {

            this->telaPrincipal->printMessageConsole(tr("ID: %2\tPosição: %3\t\tLexema: %1").arg(QString::fromStdString(t->getLexeme()))
                                                     .arg(t->getId(),4).arg(t->getPosition(),4));
            delete t;
        }
        this->telaPrincipal->printMessageConsole(tr("Análise Léxica concluída com sucesso"));
    } catch(LexicalError &e) {
        int posicaoErro = e.getPosition();
        QString msg = tr("Erro Léxico: %1 na posição: %2").arg(QString::fromStdString(e.getMessage())).arg(posicaoErro);
        this->telaPrincipal->printMessageConsole(msg,true);
    }
    delete lexico;
}

void Controle::executarAnaliseSintatica(QString src) {

    Lexico* lexico = new Lexico(src.toStdString().c_str());
    Sintatico* sintatico = new Sintatico();
    Semantico* semantico = new Semantico();

    srcAnalisys = src;

    connect(semantico,SIGNAL(sendMessage(QString,bool,int)),this,SLOT(exibirMensagem(QString,bool,int)));

    try {
        this->telaPrincipal->clearConsole();
        telaPrincipal->printMessageConsole(tr("Análise Sintática & Semântica"));
        sintatico->parse(lexico,semantico);
        QString srcDeclaracoes = this->gerarAssemblyDeclaracoes(semantico->getListaVariaveis());
        QString srcPrograma = semantico->getCodigoFonte();
        QString codigoFonte = srcDeclaracoes+".text\n"+srcPrograma+"\tHLT";
        this->telaPrincipal->showAssembly(codigoFonte);
        this->telaPrincipal->setDataTable( semantico->getListaVariaveis() );
        telaPrincipal->printMessageConsole(tr("Análise Sintática & Semântica concluída com sucesso"));
    } catch( LexicalError &e ) {
        int posicaoErro = e.getPosition();
        QString msg = tr("Erro Léxico: %1 na posição: %2").arg(QString::fromStdString(e.getMessage())).arg(posicaoErro);
        this->telaPrincipal->printMessageConsole(msg,true);
    } catch( SyntaticError& e ) {
        int posicaoErro = e.getPosition();
        QString msg = tr("Erro Sintático: %1 na posição: %2").arg(QString::fromStdString(e.getMessage())).arg(posicaoErro);
        this->telaPrincipal->printMessageConsole(msg,true);
    } catch( SemanticError & e ) {
        int posicaoErro = e.getPosition();
        QString msg = tr("Erro Semântico: %1 na posição: %2").arg(QString::fromStdString(e.getMessage())).arg(posicaoErro);
        this->telaPrincipal->printMessageConsole(msg,true);
    }

    delete lexico;
    delete sintatico;
    delete semantico;

}

void Controle::exibirMensagem(QString msg, bool error, int position) {

    QString linhas = srcAnalisys.left(position);
    int numLinha = linhas.count('\n') + 1;

    QString exibir = tr("Posição: %1, Linha: %2, Mensagem: %3")
            .arg(position)
            .arg(numLinha)
            .arg(msg);
    this->telaPrincipal->printMessageConsole(exibir,error);
}

QString Controle::gerarAssemblyDeclaracoes(QList<Variavel *> *variaveis) {

    QString fonte = ".data\n";

    int size = variaveis->size();
    for(int i = 0; i < size; i++) {
        Variavel* v = variaveis->at(i);
        fonte += QString("\t%1_escopo%2 : 0\n").arg(v->getId()).arg(v->getEscopo()->getEscopo());
    }

    return fonte;

}
