#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "Token.h"
#include "SemanticError.h"

#include "Model/Variavel.h"

#include <QList>
#include <QStack>
#include <QObject>

class Semantico : public QObject {
    Q_OBJECT
private:
    // Criação tabela de símbolos
    int numeradorEscopos;
    NoEscopo* escopoAtual;
    TipoVariavel::Tipo tipoAtual;
    QList<Variavel *>* lista;
    QStack<NoEscopo *> *pilhaEscopo;
    QStack<QString>* pilhaTokens;
    QStack<int>* pilhaExp;
    int operacao;

    Variavel* getVarOnList(QString id);
    Variavel* getVarOnList(QString id, int escopo);

    // Geração código
    bool flagExp;
    char operadorAritmetico;
    QString nomeIdAtribuicao;

    QString operadorRelacional;
    const int TEMP_ESQ;
    const int TEMP_DIR;
    const QString TEXTO_ROTULO;

    int numeradorRotulos;
    QStack<int>* pilhaRotulos;

    void gerarCod(QString mnemonico,QString value);
    void gerarRot(QString texto,int numeroRotulo);

    QString codigoFonte;

    QString codigoTemporario;
    QStack<QString> *pilhaAtribuicoesFinalFor;

public:
    Semantico() : TEMP_ESQ(1000),TEMP_DIR(1001),TEXTO_ROTULO("ROT") {
        tipoAtual = TipoVariavel::Desconhecido;
        lista = new QList<Variavel *>();
        pilhaEscopo = new QStack<NoEscopo *>();
        numeradorEscopos = 0;
        escopoAtual = new NoEscopo(0,NULL);
        pilhaEscopo->push(escopoAtual);
        pilhaTokens = new QStack<QString>();
        pilhaExp = new QStack<int>();

        flagExp = false;

        numeradorRotulos = 0;
        pilhaRotulos = new QStack<int>();
        pilhaAtribuicoesFinalFor = new QStack<QString>();
    }

    ~Semantico() {
        delete pilhaEscopo;
        delete pilhaTokens;
        delete pilhaExp;
    }

    inline QList<Variavel *>* getListaVariaveis() { return this->lista; }

    void executeAction(int action, const Token *token) noexcept(false);

    inline QString getCodigoFonte(){ return this->codigoFonte; }
signals:
    void sendMessage(QString message,bool error,int position);
};

#endif
