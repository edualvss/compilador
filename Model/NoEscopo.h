#ifndef NOESCOPO_H
#define NOESCOPO_H

class NoEscopo {
private:
    int escopo;
    NoEscopo* pai;
public:
    NoEscopo() {
        this->escopo = -1;
        this->pai = NULL;
    }
    NoEscopo(int escopo, NoEscopo* pai) {
        this->escopo = escopo;
        this->pai = pai;
    }

    inline void setEscopo(int escopo) { this->escopo = escopo; }
    inline void setPai(NoEscopo* pai) { this->pai = pai; }

    inline int getEscopo() { return this->escopo; }
    inline NoEscopo* getPai() { return this->pai; }
};

#endif // NOESCOPO_H
