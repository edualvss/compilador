#include "View/telaabout.h"
#include "ui_telaabout.h"

TelaAbout::TelaAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TelaAbout) {
    ui->setupUi(this);

    this->adicionarAutores();

    delete ui->widgetAuthors->layout();
    QVBoxLayout* layout = new QVBoxLayout(ui->widgetAuthors);
    ui->widgetAuthors->setLayout(layout);

    for(int i = 0; i < autores.size(); i++) {
        QStringList autor = autores.at(i);
        QLabel* lab = new QLabel(ui->scrollAuthors);
        lab->setWordWrap(true);
        lab->setText("");
        for(int x = 0; x < autor.size(); x++) {
            lab->setText( lab->text()+"<br />"+autor.at(x));
        }
        layout->addWidget(lab);
    }
}

void TelaAbout::adicionarAutores() {

    QStringList autor1;
    autor1 << tr("<b>Dono e Desenvolvedor</b>") << "Eduardo Alves da Silva" << "eduardoalves@edu.univali.br";

    autores.insert(0,autor1);

}

TelaAbout::~TelaAbout() {
    delete ui;
}
