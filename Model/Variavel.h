#ifndef VARIAVEL_H
#define VARIAVEL_H

#include <QString>
#include "Model/NoEscopo.h"

namespace TipoVariavel {
    enum Tipo{Desconhecido=-1,Inteiro,Real,Caracter,String,Bool};
}
class Variavel {
private:

    TipoVariavel::Tipo tipo;
    QString id;
    bool usada;
    bool inicializada;
    NoEscopo* escopo;

public:
    Variavel(){
        this->tipo = TipoVariavel::Desconhecido;
        this->id = QString();
        this->usada = false;
        this->inicializada = false;
        this->escopo = NULL;
    }
    Variavel(TipoVariavel::Tipo tp, char* id, bool usada,bool inicializada, NoEscopo* escopo){

        this->tipo = tp;
        this->id = id;
        this->usada = usada;
        this->inicializada = inicializada;
        this->escopo = escopo;

    }

    inline void setTipo(TipoVariavel::Tipo tp) {this->tipo = tp;}
    inline void setId(QString id) {this->id = id;}
    inline void setUsada(bool usada) {this->usada = usada;}
    inline void setInicializada(bool inicializada) {this->inicializada = inicializada;}
    inline void setEscopo(NoEscopo* escopo) {this->escopo = escopo;}

    inline TipoVariavel::Tipo getTipo() {return this->tipo;}
    inline QString getId() {return this->id;}
    inline bool isUsada() {return this->usada;}
    inline bool isInicializada() {return this->inicializada;}
    inline NoEscopo* getEscopo() {return this->escopo;}

};

#endif // VARIAVEL_H
