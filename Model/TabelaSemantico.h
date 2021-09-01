#ifndef TABELASEMANTICO_H
#define TABELASEMANTICO_H

class TabelaSemantico {
public:
    enum Tipos {INT = 0,FLO, CHA, STR,BOO};
    enum Operacoes {SUM = 0,SUB,MUL,DIV,REL,MOD,POT,ROO,AND,OR_};
    enum Status {WAR = -3,OK_,ERR};

    // TIPO DE RETORNO DAS EXPRESSOES ENTRE TIPOS
    // 5 x 5 X 5  = TIPO X TIPO X OPER
    static int const expTable [5][5][10];

    // atribuicoes compativeis
    // 5 x 5 = TIPO X TIPO
    static int const atribTable [5][5];

    static int resultType (int TP1, int TP2, int OP) {
        if(TP1 < 0 || TP2 < 0 || OP < 0) {
            return ERR;
        }
        return (expTable[TP1][TP2][OP]);
    }

    static int atribType (int TP1, int TP2){
        if(TP1 < 0 || TP2 < 0) {
            return ERR;
        }
        return (atribTable[TP1][TP2]);
    }

//    TabelaSemantico();
};

int const TabelaSemantico::expTable[5][5][10] =
            {/*                INT                  */ /*                 FLOAT               */ /*                 CHAR                */ /*               STRING                */ /*                 BOOL                */
/*INT   */  {{INT,INT,INT,FLO,BOO,INT,INT,FLO,ERR,ERR},{FLO,FLO,FLO,FLO,BOO,ERR,FLO,FLO,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}},
/*FLOAT */  {{FLO,FLO,FLO,FLO,BOO,ERR,FLO,FLO,ERR,ERR},{FLO,FLO,FLO,FLO,BOO,ERR,FLO,FLO,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}},
/*CHAR  */  {{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}},
/*STRING*/  {{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}},
/*BOOL  */  {{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,BOO,BOO}}
            };
       /*     SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_ , SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_ , SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_ , SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_ , SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_     */

int const TabelaSemantico::atribTable[5][5]=
    {     /* INT FLO CHA STR BOO  */
    /*INT*/ {OK_,WAR,ERR,ERR,ERR},
    /*FLO*/ {OK_,OK_,ERR,ERR,ERR},
    /*CHA*/ {ERR,ERR,OK_,ERR,ERR},
    /*STR*/ {ERR,ERR,OK_,OK_,ERR},
    /*BOO*/ {ERR,ERR,ERR,ERR,OK_}
    };

#endif // TABELASEMANTICO_H
