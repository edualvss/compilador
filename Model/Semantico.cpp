#include "Semantico.h"
#include "Constants.h"

#include "Model/TabelaSemantico.h"


void Semantico::executeAction(int action, const Token *token) noexcept(false) {

    QString lexema = QString::fromStdString(token->getLexeme());

    switch( action ) {
        case 1:{ // ID de uma variável
            Variavel* var = this->getVarOnList(lexema);
            if(tipoAtual != TipoVariavel::Desconhecido) { // É declaração
                if(var == NULL) { // Não declarada ainda com este id
                    var = new Variavel();
                    var->setEscopo(this->escopoAtual);
                    var->setId(lexema);
                    var->setTipo(tipoAtual);
                    lista->append(var);
                } else {
                    var = this->getVarOnList(lexema,escopoAtual->getEscopo());
                    if(var != NULL) {
                        if(!pilhaTokens->isEmpty()) {
                            break;
                        }
                        emit sendMessage(tr("Já foi declarada uma variável \"%1\" neste escopo!").arg(lexema),true,token->getPosition());
                    } else {
                        var = new Variavel();
                        var->setEscopo(escopoAtual);
                        var->setId(lexema);
                        var->setTipo(tipoAtual);
                        lista->append(var);
                    }
                }
            }
            break;
        }
        case 2:{ // Tipo de dado na declaração de variável
            if(lexema.compare("int") == 0) {
                tipoAtual = TipoVariavel::Inteiro;
            } else if(lexema.compare("float") == 0) {
                tipoAtual = TipoVariavel::Real;
            } else if(lexema.compare("char") == 0) {
                tipoAtual = TipoVariavel::Caracter;
            } else if(lexema.compare("bool") == 0){
                tipoAtual = TipoVariavel::Bool;
            } else {
                tipoAtual = TipoVariavel::String;
            }
            break;
        }
        case 3: // Fim da declaração de variável, lexema == ";"
            tipoAtual = TipoVariavel::Desconhecido;
            break;
        case 4:{ // Início de escopo
            NoEscopo* novoEscopo = new NoEscopo();
            novoEscopo->setEscopo(++numeradorEscopos);
            novoEscopo->setPai(pilhaEscopo->top());
            this->escopoAtual = novoEscopo;
            pilhaEscopo->push(novoEscopo);
            break;
        }
        case 5: // Fim de escopo
            pilhaEscopo->pop();
            escopoAtual = pilhaEscopo->top();
            break;
        case 6:{ // Atribuição com leitura
            NoEscopo* escopo = escopoAtual;
            bool encontrada = false;
            int tipo = TabelaSemantico::ERR;
            do {
                Variavel* var = this->getVarOnList(lexema,escopo->getEscopo());
                if(var != NULL) {
                    if(!var->isInicializada()) {
                        emit sendMessage(tr("Variável \"%1\" não foi inicializada").arg(lexema),false,token->getPosition());
                    }
                    var->setUsada(true);
                    var->setInicializada(true);
                    encontrada = true;
                    tipo = var->getTipo();
                    this->nomeIdAtribuicao = QString("%1_escopo%2").arg(lexema).arg(var->getEscopo()->getEscopo());
                    break;
                } else {
                    escopo = escopo->getPai();
                }
            } while(escopo != NULL);
            if(!encontrada) {
                emit sendMessage(tr("Variável \"%1\" não declarada").arg(lexema),true,token->getPosition());
            }
            this->pilhaExp->push(tipo);
            break;
        }
        case 7:{ // Escrita na variável através de "scanf"
            NoEscopo* escopo = escopoAtual;
            bool encontrada = false;
            do {
                Variavel* var = this->getVarOnList(lexema,escopo->getEscopo());
                if(var != NULL) {
                    var->setInicializada(true);
                    encontrada = true;
                    gerarCod("LD","$in_port");
                    gerarCod("STO",QString("%1_escopo%2").arg(lexema).arg(var->getEscopo()->getEscopo()));
                    break;
                } else {
                    escopo = escopo->getPai();
                }
            } while (escopo != NULL);
            if(!encontrada) {
                emit sendMessage(tr("Variável \"%1\" não declarada").arg(lexema),true,token->getPosition());
            }
            break;
        }
        case 8:{ // Variavel que recebe a atribuicao direta
            pilhaTokens->push(lexema);
            break;
        }
        case 9: { // Fim <EXP>
            QString id = pilhaTokens->pop();
            NoEscopo* escopo = escopoAtual;
            bool encontrada = false;
            do {
                Variavel* var = this->getVarOnList(id,escopo->getEscopo());
                if(var != NULL) {
                    var->setInicializada(true);
                    encontrada = true;
                    break;
                } else {
                    escopo = escopo->getPai();
                }
            } while (escopo != NULL);
            if(!encontrada) {
                emit sendMessage(tr("Variável \"%1\" não declarada").arg(id),true,token->getPosition());
            }
            break;
        }
        case 10: {// <VARIAVEL>
            NoEscopo* escopo = escopoAtual;
            bool encontrada = false;
            int tipo = TabelaSemantico::ERR;
            do {
                Variavel* var = this->getVarOnList(lexema,escopo->getEscopo());
                if(var != NULL) {
                    if(!var->isInicializada()) {
                        emit sendMessage(tr("Variável \"%1\" não foi inicializada").arg(lexema),false,token->getPosition());
                    }
                    var->setUsada(true);
                    var->setInicializada(true);
                    encontrada = true;
                    tipo = var->getTipo();
                    break;
                } else {
                    escopo = escopo->getPai();
                }
            } while(escopo != NULL);
            if(!encontrada) {
                emit sendMessage(tr("Variável \"%1\" não declarada").arg(lexema),true,token->getPosition());
            }
            this->pilhaExp->push(tipo);
            break;
        }
        case 11: // numberInt
            this->pilhaExp->push(TabelaSemantico::INT);
            break;
        case 12: // numberFloat
            this->pilhaExp->push(TabelaSemantico::FLO);
            break;
        case 13: // character
            this->pilhaExp->push(TabelaSemantico::CHA);
            break;
        case 14: // stringText
            this->pilhaExp->push(TabelaSemantico::STR);
            break;
        case 15: // true | false
            this->pilhaExp->push(TabelaSemantico::BOO);
            break;
        case 16: { // Final operação || , or
            int op2 = this->pilhaExp->pop();
            int op1 = this->pilhaExp->pop();
            int res = TabelaSemantico::resultType(op1,op2,TabelaSemantico::OR_);
            if(res == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(res == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }

            this->pilhaExp->push(res);
            break;
        }
        case 17:{ // Final operação && , and
            int op2 = this->pilhaExp->pop();
            int op1 = this->pilhaExp->pop();
            int res = TabelaSemantico::resultType(op1,op2,TabelaSemantico::AND);
            if(res == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(res == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }

            this->pilhaExp->push(res);
            break;
        }
        case 18:{ // Final operação ! , not
            int op1 = this->pilhaExp->pop();
            int res = op1 == TabelaSemantico::BOO ? TabelaSemantico::BOO : TabelaSemantico::STR;
            if(res == TabelaSemantico::STR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            }
            this->pilhaExp->push(res);
            break;
        }
        case 19:{ // Final operação <OPERADOR_RELACIONAL>
            int op2 = this->pilhaExp->pop();
            int op1 = this->pilhaExp->pop();
            int res = TabelaSemantico::resultType(op1,op2,TabelaSemantico::REL);
            if(res == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(res == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }

            this->pilhaExp->push(res);

            gerarCod("STO",QString::number(TEMP_DIR));
            gerarCod("LD",QString::number(TEMP_ESQ));
            gerarCod("SUB",QString::number(TEMP_DIR));

            break;
        }
        case 20:  // Final operação <ARIT2>
        case 21:  // Final operação <ARIT1>
        case 22:{ // Final operação <ARIT0>
            int op2 = this->pilhaExp->pop();
            int op1 = this->pilhaExp->pop();
            int res = TabelaSemantico::resultType(op1,op2,operacao);

//            qDebug() << QString("Op1: %1, Op2: %2, Operacao: %4, res: %3").arg(op1).arg(op2).arg(res).arg(operacao);
            if(res == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(res == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }

            this->pilhaExp->push(res);
            break;
        }
        case 23: { // Operação mudança de sinal -
            int op1 = this->pilhaExp->pop();
            int res = (op1 == TabelaSemantico::INT || op1 == TabelaSemantico::FLO) ? op1 : TabelaSemantico::ERR;
            this->pilhaExp->push(res);
            break;
        }

        case 25:{ // Variável que recebe a atribuição | Buscar o tipo para empilhar e comparar na expressão
            NoEscopo* escopo = escopoAtual;
            int tipo = TabelaSemantico::ERR;
            do {
                Variavel* var = this->getVarOnList(lexema,escopo->getEscopo());
                if(var != NULL) {
                    tipo = var->getTipo();
                    this->nomeIdAtribuicao = QString("%1_escopo%2").arg(lexema).arg(var->getEscopo()->getEscopo());
                    break;
                } else {
                    escopo = escopo->getPai();
                }
            } while(escopo != NULL);
//            qDebug() << QString("Tipo da variavel que recebe atribuicao: %1").arg(tipo);
            this->pilhaExp->push(tipo);
            break;
        }

        case 26: { // Final de expressão em atribuição direta
            int tpExp = this->pilhaExp->pop();
            int tpVar = this->pilhaExp->pop();
            int atribRes = TabelaSemantico::atribType(tpVar,tpExp);
//            qDebug() << tr("Tipo exp: %1, tipo var: %2, atribRes: %3").arg(tpExp).arg(tpVar).arg(atribRes);
            if(atribRes == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na atribuição"),true,token->getPosition());
            } else if(atribRes == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na atribuição"),false,token->getPosition());
            }
            break;
        }

        case 27: // Potência ^
            this->operacao = TabelaSemantico::POT;
            break;
        case 28: // Raiz ::
            this->operacao = TabelaSemantico::ROO;
            break;
        case 29: // Multiplicação *
            this->operacao = TabelaSemantico::MUL;
            break;
        case 30: // Divisão /
            this->operacao = TabelaSemantico::DIV;
            break;
        case 31: // Módulo %
            this->operacao = TabelaSemantico::MOD;
            break;
        case 32: // Soma +
            this->operacao = TabelaSemantico::SUM;
            break;
        case 33: // Subtração -
            this->operacao = TabelaSemantico::SUB;
            break;

        case 34:{ // Final expressão +=
            int opExp = pilhaExp->pop();
            int opVar = pilhaExp->pop();
            int resOp = TabelaSemantico::resultType(opVar,opExp,TabelaSemantico::SUM);
            if(resOp == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(resOp == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }
            int atribRes = TabelaSemantico::atribType(opVar,resOp);
            if(atribRes == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na atribuição"),true,token->getPosition());
            } else if(atribRes == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na atribuição"),false,token->getPosition());
            }

            gerarCod("ADD",nomeIdAtribuicao);

            break;
        }
        case 35:{ // Final expressão -=
            int opExp = pilhaExp->pop();
            int opVar = pilhaExp->pop();
            int resOp = TabelaSemantico::resultType(opVar,opExp,TabelaSemantico::SUB);
            if(resOp == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(resOp == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }

            int atribRes = TabelaSemantico::atribType(opVar,resOp);
            if(atribRes == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na atribuição"),true,token->getPosition());
            } else if(atribRes == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na atribuição"),false,token->getPosition());
            }

            gerarCod("SUB",nomeIdAtribuicao);

            break;
        }
        case 36:{ // Final expressão *=
            int opExp = pilhaExp->pop();
            int opVar = pilhaExp->pop();
            int resOp = TabelaSemantico::resultType(opVar,opExp,TabelaSemantico::MUL);
            if(resOp == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(resOp == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }

            int atribRes = TabelaSemantico::atribType(opVar,resOp);
            if(atribRes == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na atribuição"),true,token->getPosition());
            } else if(atribRes == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na atribuição"),false,token->getPosition());
            }
            break;
        }
        case 37:{ // Final expressão /=
            int opExp = pilhaExp->pop();
            int opVar = pilhaExp->pop();
            int resOp = TabelaSemantico::resultType(opVar,opExp,TabelaSemantico::DIV);
            if(resOp == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(resOp == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }

            int atribRes = TabelaSemantico::atribType(opVar,resOp);
            if(atribRes == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na atribuição"),true,token->getPosition());
            } else if(atribRes == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na atribuição"),false,token->getPosition());
            }
            break;
        }
        case 38:{ // Final expressão %=
            int opExp = pilhaExp->pop();
            int opVar = pilhaExp->pop();
            int resOp = TabelaSemantico::resultType(opVar,opExp,TabelaSemantico::MOD);
            if(resOp == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na expressão"),true,token->getPosition());
            } else if(resOp == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na expressão"),false,token->getPosition());
            }

            int atribRes = TabelaSemantico::atribType(opVar,resOp);
            if(atribRes == TabelaSemantico::ERR) {
                emit sendMessage(tr("Erro: Tipos incompatíveis na atribuição"),true,token->getPosition());
            } else if(atribRes == TabelaSemantico::WAR) {
                emit sendMessage(tr("Warning: Diferença entre tipos na atribuição"),false,token->getPosition());
            }
            break;
        }

        case 39: // Final de <EXP> em atribuição
            pilhaExp->clear();            
            this->gerarCod("STO",nomeIdAtribuicao);
            break;

        case 40:{ // Final de <EXP> em condição lógica (desvios, laços)
            int resOp = pilhaExp->pop();
            if(resOp != TabelaSemantico::BOO) {
                emit sendMessage(tr("Somente é aceito tipo lógico nas expressões de laços e desvios"),true,token->getPosition());
            }
            pilhaExp->clear();
            break;
        }
        case 41:{ // <OPERADOR_ARIT2> = '+' ou '-' para geração de código
            flagExp = true;
            operadorAritmetico = lexema.at(0).toLatin1();
            break;
        }
        case 42:{ // numberInt
            if(!flagExp) {
                gerarCod("LDI",lexema);
            } else {
                if(operadorAritmetico == '+') {
                    gerarCod("ADDI",lexema);
                } else if(operadorAritmetico == '-') {
                    gerarCod("SUBI",lexema);
                }
                flagExp = false;
            }
            break;
        }
        case 43:{ // <VARIAVEL>
            NoEscopo* escopo = escopoAtual;
            QString varId;
            do {
                Variavel* var = this->getVarOnList(lexema,escopo->getEscopo());
                if(var != NULL) {
                    varId = QString("%1_escopo%2").arg(lexema).arg(var->getEscopo()->getEscopo());
                    break;
                } else {
                    escopo = escopo->getPai();
                }
            } while(escopo != NULL);

            if(!flagExp) {
                gerarCod("LD",varId);
            } else {
                if(operadorAritmetico == '+') {
                    gerarCod("ADD", varId);
                } else if(operadorAritmetico == '-') {
                    gerarCod("SUB",varId);
                }
                flagExp = false;
            }
            break;
        }
        case 44:{ // Após <SAIDAS> no <PRINT>
            gerarCod("STO","$out_port");
            break;
        }
        case 45: { // <OPERADOR_RELACIONAL>
            operadorRelacional = lexema;
            gerarCod("STO",QString::number(TEMP_ESQ));
            break;
        }
        case 46:{ // Início "if" -> após expressão
            this->numeradorRotulos++;
            pilhaRotulos->push(numeradorRotulos);
            if(operadorRelacional == ">") {
                gerarCod("BLE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "<") {
                gerarCod("BGE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "<=") {
                gerarCod("BGT",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == ">=") {
                gerarCod("BLT",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "==") {
                gerarCod("BNE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "!=") {
                gerarCod("BEQ",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            break;
        }
        case 47: { // Fim do "if"
            int rotulo = pilhaRotulos->pop();
            gerarRot(TEXTO_ROTULO,rotulo);
            break;
        }
        case 48: { // Início "else"
            this->numeradorRotulos++;
            int rot =  pilhaRotulos->pop();
            gerarCod("JMP",TEXTO_ROTULO+QString::number(numeradorRotulos));
            pilhaRotulos->push(numeradorRotulos);
            gerarRot(TEXTO_ROTULO,rot);
            break;
        }
        case 49: { // Início laço "while" antes da expressão
            this->numeradorRotulos++;
            pilhaRotulos->push(numeradorRotulos);
            gerarRot(TEXTO_ROTULO,numeradorRotulos);
            break;
        }
        case 50: { // Início laço "while" depois da expressão
            this->numeradorRotulos++;
            pilhaRotulos->push(numeradorRotulos);
            if(operadorRelacional == ">") {
                gerarCod("BLE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "<") {
                gerarCod("BGE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "<=") {
                gerarCod("BGT",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == ">=") {
                gerarCod("BLT",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "==") {
                gerarCod("BNE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "!=") {
                gerarCod("BEQ",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            break;
        }
        case 51:{ // Fim laço "while"
            int rot2 = pilhaRotulos->pop();
            int rot = pilhaRotulos->pop();
            gerarCod("JMP",TEXTO_ROTULO+QString::number(rot));
            gerarRot(TEXTO_ROTULO,rot2);
            break;
        }
        case 52: { // Início laço "do .. while"
            this->numeradorRotulos++;
            pilhaRotulos->push(numeradorRotulos);
            gerarRot(TEXTO_ROTULO,numeradorRotulos);
            break;
        }
        case 53: { // Fim laço "do .. while" depois da expressão
            int rotulo = pilhaRotulos->pop();
            if(operadorRelacional == "<=") {
                gerarCod("BLE",TEXTO_ROTULO+QString::number(rotulo));
            }
            if(operadorRelacional == ">=") {
                gerarCod("BGE",TEXTO_ROTULO+QString::number(rotulo));
            }
            if(operadorRelacional == ">") {
                gerarCod("BGT",TEXTO_ROTULO+QString::number(rotulo));
            }
            if(operadorRelacional == "<") {
                gerarCod("BLT",TEXTO_ROTULO+QString::number(rotulo));
            }
            if(operadorRelacional == "!=") {
                gerarCod("BNE",TEXTO_ROTULO+QString::number(rotulo));
            }
            if(operadorRelacional == "==") {
                gerarCod("BEQ",TEXTO_ROTULO+QString::number(rotulo));
            }
            break;
        }
        case 54: { // Final operação incremento. OBS: antes do STO no assembly
            gerarCod("LD",nomeIdAtribuicao);
            gerarCod("ADDI","1");
            break;
        }
        case 55: { // Final operação decremento. OBS: antes do STO no assembly
            gerarCod("LD",nomeIdAtribuicao);
            gerarCod("SUBI","1");
            break;
        }
        case 56: { // Início laço "for" antes da expressão | Considera-se que a parte da atribuição inicial já está tratada pelas <EXP>
            this->numeradorRotulos++;
            pilhaRotulos->push(numeradorRotulos);
            gerarRot(TEXTO_ROTULO,numeradorRotulos);
            break;
        }
        case 57: {// Início laço "for" depois da expressão
            this->numeradorRotulos++;
            pilhaRotulos->push(numeradorRotulos);
            if(operadorRelacional == ">") {
                gerarCod("BLE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "<") {
                gerarCod("BGE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "<=") {
                gerarCod("BGT",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == ">=") {
                gerarCod("BLT",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "==") {
                gerarCod("BNE",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            if(operadorRelacional == "!=") {
                gerarCod("BEQ",TEXTO_ROTULO+QString::number(numeradorRotulos));
            }
            break;
        }
        case 58: { // Início laço "for" antes das atribuições finais
            codigoTemporario = codigoFonte;
            codigoFonte = "";
            break;
        }
        case 59: { // Início laço "for" depois das atribuições finais
            pilhaAtribuicoesFinalFor->push(codigoFonte);
            codigoFonte = codigoTemporario;
            break;
        }
        case 60: { // Fim laço "for"
            codigoTemporario = pilhaAtribuicoesFinalFor->pop();
            codigoFonte += codigoTemporario;
            int rot2 = pilhaRotulos->pop();
            int rot = pilhaRotulos->pop();
            gerarCod("JMP",TEXTO_ROTULO+QString::number(rot));
            gerarRot(TEXTO_ROTULO,rot2);
            break;
        }
    }

}

Variavel* Semantico::getVarOnList(QString id) {

    for(int i = 0; i < lista->size(); i++) {
        Variavel* v = lista->at(i);
        if(v->getId() == id ) {
            return v;
        }
    }
    return NULL;
}

Variavel* Semantico::getVarOnList(QString id, int escopo) {
    for(int i = 0; i < lista->size(); i++) {
        Variavel* v = lista->at(i);
        if(v->getId() == id && v->getEscopo()->getEscopo() == escopo) {
            return v;
        }
    }
    return NULL;
}

void Semantico::gerarCod(QString mnemonico, QString value) {

    codigoFonte += "\t"+mnemonico+"\t"+value+"\n";

}

void Semantico::gerarRot(QString texto, int numeroRotulo) {

    codigoFonte += texto+QString::number(numeroRotulo)+":\n";

}
