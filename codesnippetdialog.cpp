#include "codesnippetdialog.hpp"
#include "ui_codesnippetdialog.h"

#include <QClipboard>
#include <QPushButton>

CodeSnippetDialog::CodeSnippetDialog(const QString& contents, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::CodeSnippetDialog)
{
	ui->setupUi(this);
	ui->teContents->setPlainText(contents);

	QPushButton* const copyButton = ui->buttonBox->addButton(tr("Co&py"), QDialogButtonBox::ApplyRole);

	copyButton->setDefault(true);
	connect(copyButton, SIGNAL(clicked()), this, SLOT(on_copyButton_clicked()));
}

CodeSnippetDialog::~CodeSnippetDialog()
{
	delete ui;
}

void CodeSnippetDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void CodeSnippetDialog::on_copyButton_clicked()
{
	QApplication::clipboard()->setText(ui->teContents->toPlainText());
}
