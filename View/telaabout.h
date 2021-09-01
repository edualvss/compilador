#ifndef TELAABOUT_H
#define TELAABOUT_H

#include <QDialog>

namespace Ui {
class TelaAbout;
}

class TelaAbout : public QDialog
{
    Q_OBJECT
    
public:
    explicit TelaAbout(QWidget *parent = 0);
    ~TelaAbout();
    
private:
    void adicionarAutores();
    QList<QStringList> autores;
    Ui::TelaAbout *ui;
};

#endif // TELAABOUT_H
