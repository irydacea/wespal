//
// codename Morning Star
//
// Copyright (C) 2008, 2009, 2010 by Ignacio R. Morelle <shadowm@wesnoth.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "defs.hpp"
#include "mainwindow.hpp"
#include "rc_qt4.hpp"
#include "ui_mainwindow.h"
#include "version.hpp"

#include <QDesktopServices> // viva la integration!
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow),
	img_path_(),
	source_path_(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation)),
	img_original_(),
	img_transview_()
{
    ui->setupUi(this);

	ui->previewOriginal->setBackgroundRole(QPalette::Base);

	ui->cbxKeyPal->addItem(tr("TC Magenta"));
	ui->cbxKeyPal->addItem(tr("TC Flag"));
	ui->cbxKeyPal->addItem(tr("TC Ellipse"));

	ui->cbxNewPal->addItem(tr("TC Magenta"));
	ui->cbxNewPal->addItem(tr("TC Flag"));
	ui->cbxNewPal->addItem(tr("TC Ellipse"));

	ui->listRanges->addItems(mos_color_range_names);

	for(int n = 0; n < ui->listRanges->count(); ++n) {
		QListWidgetItem *i = ui->listRanges->item(n);
		i->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		i->setCheckState(Qt::Checked);

		// FIXME: why am I doing this just to capitalize the names?...
		i->setText(
			i->text().toUpper().left(1) +
			i->text().right(i->text().length() - 1)
		);

		// Reset selection to #1
		ui->listRanges->setItemSelected(i, n == 0);
	}

	ui->radRc->setChecked(true);
	ui->staFunctionOpts->setCurrentIndex(0);
	toggle_page2(false);
	toggle_page1(true);

	//this->do_open();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_radRc_clicked()
{
	ui->staFunctionOpts->setCurrentIndex(0);
	toggle_page1(true);
	toggle_page2(false);
}

void MainWindow::on_radPal_clicked()
{
	ui->staFunctionOpts->setCurrentIndex(1);
	toggle_page1(false);
	toggle_page2(true);
}

void MainWindow::toggle_page1(bool newstate)
{
	ui->listRanges->setEnabled(newstate);
}

void MainWindow::toggle_page2(bool newstate)
{
	ui->cbxNewPal->setEnabled(newstate);
	ui->lblNewPal->setEnabled(newstate);
}

void MainWindow::on_actionAbout_Morning_Star_triggered()
{
	do_about();
}

void MainWindow::on_buttonBox_clicked(QAbstractButton* button)
{
	QDialogButtonBox::StandardButton btype = ui->buttonBox->standardButton(button);

	switch(btype) {
	case QDialogButtonBox::Save:
		do_save();
		break;
	case QDialogButtonBox::Close:
		do_close();
		break;
	default:
		;
	}
}

void MainWindow::on_action_Open_triggered()
{
	do_open();
}

void MainWindow::on_action_Quit_triggered()
{
	do_close();
}

void MainWindow::do_open()
{
	this->setEnabled(false);

	QString path_temp = QFileDialog::getOpenFileName(
		this,
		tr("Choose source image"),
		source_path_,
		tr("PNG image (*.png);;All files (*)")
	);

	if(path_temp.isNull()) {
		if(img_original_.isNull()) {
			// it's null if we've just setup the window
			do_close();
		}
	}
	else {
		QImage img_temp(path_temp);
		if(img_temp.isNull()) {
			QMessageBox::information(
					this, tr("Morning Star"), tr("Could not load %1.").arg(path_temp)
					);
			return;
		}

		img_path_ = path_temp;
		// We want to work on actual ARGB data
		img_original_ = img_temp.convertToFormat(QImage::Format_ARGB32);

		// Refresh UI
		this->setWindowTitle(QString(img_path_ + " - ") + tr("Morning Star"));
		ui->previewOriginal->setPixmap(QPixmap::fromImage(img_original_));
		refresh_previews();
	}

	this->setEnabled(true);
}

void MainWindow::refresh_previews()
{
	rc_map cvt_map = recolor_range(mos_color_range_from_id(ui->listRanges->currentIndex().row() + 1), mos_pal_magenta);
	rc_image(img_original_, img_transview_, cvt_map);

	ui->previewRc->setPixmap(QPixmap::fromImage(img_transview_));
}

void MainWindow::do_save()
{

}

void MainWindow::do_close()
{
	this->close();
	QCoreApplication::quit();
}

void MainWindow::do_about()
{
	QMessageBox msg;

	QString text =
		tr("<b>Codename <i>Morning Star</i> %1</b>").arg(mos_version) + "<br><br>";

	text += tr("Built with Qt %1.").arg(QT_VERSION_STR) + "<br>";
	text += tr("Copyright &copy; 2008, 2009, 2010 by Ignacio R. Morelle.") + "<br>";
	text += "&lt;shadowm@wesnoth.org&gt;<br><br>";

	text += tr(
		"This program is free software; you can redistribute it and/or modify<br>"
		"it under the terms of the GNU General Public License as published by<br>"
		"the Free Software Foundation; either version 2 of the License, or<br>"
		"(at your option) any later version.<br>"
		"<br>"
		"This program is distributed in the hope that it will be useful, but<br>"
		"<b>WITHOUT ANY WARRANTY</b>; without even the implied warranty of<br>"
		"<b>MERCHANTABILITY</b> or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.<br>"
		"See the GNU General Public License for more details.<br>"
	);

	msg.setText(text);
	msg.exec();
}

void MainWindow::on_listRanges_itemClicked(QListWidgetItem* /*item*/)
{
	refresh_previews();
}
