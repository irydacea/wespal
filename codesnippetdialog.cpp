#include "codesnippetdialog.hpp"
#include "ui_codesnippetdialog.h"

#include "util.hpp"

#include <QClipboard>
#include <QFileDialog>
#include <QPushButton>
#include <QTextStream>

CodeSnippetDialog::CodeSnippetDialog(const QString& contents, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::CodeSnippetDialog)
{
	ui->setupUi(this);
	ui->teContents->setPlainText(contents);

	QPushButton* const copyButton = ui->buttonBox->addButton(tr("Co&py"), QDialogButtonBox::ApplyRole);

	copyButton->setDefault(true);
	connect(copyButton, SIGNAL(clicked()), this, SLOT(on_copyButton_clicked()));

	QPushButton* const saveButton = ui->buttonBox->button(QDialogButtonBox::Save);

	if(saveButton) {
		connect(saveButton, SIGNAL(clicked()), this, SLOT(on_saveButton_clicked()));
	}

	ui->successIcon->setPixmap(style()->standardIcon(QStyle::SP_DialogOkButton).pixmap(22));
	ui->boxClipboardMessage->setVisible(false);
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
	ui->boxClipboardMessage->setVisible(true);
}

void CodeSnippetDialog::on_saveButton_clicked()
{
	const QString& filePath = QFileDialog::getSaveFileName(this, tr("Save Color Range WML"));

	if(filePath.isNull()) {
		return;
	}

	QFile f(filePath);

	if(f.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
		QTextStream out(&f);
		out << ui->teContents->toPlainText();
		f.close();
	}

	if(f.error()) {
		JobUi::error(this, tr("The file could not be saved"), filePath);
	} else {
		JobUi::message(this, tr("The file was saved successfully."), filePath);
	}
}
