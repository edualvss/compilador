#ifndef CONTROLE_H
#define CONTROLE_H

#include <QObject>

class MainWindow;
class Variavel;

class Controle : public QObject {
    Q_OBJECT

private:
    MainWindow* telaPrincipal;
    QString srcAnalisys;

    void estabelecerConexoes();

    QString gerarAssemblyDeclaracoes(QList<Variavel*>* variaveis);

public:
    explicit Controle(QObject *parent = 0);
    
    void iniciarAplicacao();
    
private slots:

    void executarAnaliseLexica(QString src);
    void executarAnaliseSintatica(QString src);

    void exibirMensagem(QString msg, bool error,int position);
};

#endif // CONTROLE_H
