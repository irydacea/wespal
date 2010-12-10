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
#include "customranges.hpp"
#include "mainwindow.hpp"
#include "rc_qt4.hpp"
#include "ui_mainwindow.h"
#include "version.hpp"

#include <QDesktopServices> // viva la integration!
#include <QFileDialog>
#include <QIcon>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QUrl>
#include <QWhatsThis>

namespace {
	struct no_initial_file {};
	struct canceled_job    {};

	QString capitalize(const QString& str)
	{
		if(str.isEmpty())
			return str;

		QString ret = str.toUpper().left(1);
		ret += str.right(str.length() - 1);

		return ret;
	}
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow),
	img_path_(),
	img_original_(),
	img_transview_(),
	zoom_(1.0f),
	ignore_drops_(false),
	drag_use_rc_(false),
	drag_start_(false),
	drag_start_pos_()
{
    ui->setupUi(this);

	initialize_specs();
	update_ui_from_specs();

	QAction* const act_whatsthis = QWhatsThis::createAction(this);
	ui->menu_Help->insertAction(ui->actionAbout_Morning_Star, act_whatsthis);
	ui->menu_Help->insertSeparator(ui->actionAbout_Morning_Star);

	QPushButton* const save = ui->buttonBox->button(QDialogButtonBox::Save);
	QPushButton* const close = ui->buttonBox->button(QDialogButtonBox::Close);

	save->setWhatsThis(tr("Saves the current recolor job."));
	close->setWhatsThis(tr("Abandons the current job and exits."));

	ui->action_Open->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton, 0, dynamic_cast<QWidget*>(ui->action_Open)));
	ui->action_Save->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton, 0, dynamic_cast<QWidget*>(ui->action_Save)));
	ui->action_Quit->setIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton, 0, dynamic_cast<QWidget*>(ui->action_Quit)));

	ui->radRc->setChecked(true);
	ui->staFunctionOpts->setCurrentIndex(0);
	toggle_page2(false);
	toggle_page1(true);

	ui->cbxZoomFactor->addItems(
		QStringList()
			<< "50%"
			<< "100%"
			<< "200%"
			<< "400%"
			<< "800%"
	);

	ui->cbxZoomFactor->setCurrentIndex(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize_specs()
{
	color_ranges_.clear();
	palettes_.clear();

	/* The main 9 color ranges. */

	for(int i = 1; i < 10; ++i) {
		QString const& id = mos_color_range_id_to_name(i);
		color_ranges_.push_back(range_spec(
			mos_color_range_from_id(i),
			id,
			capitalize(id)
		));
	}

	/* User defined ranges. */

	color_ranges_ << user_ranges_;

	/* The main 3 palettes. */

	palettes_
		<< pal_spec(mos_pal_magenta, "magenta", tr("Magenta TC"))
		<< pal_spec(mos_pal_flag_green, "flag_green", tr("Green flag TC"))
		<< pal_spec(mos_pal_ellipse_red, "ellipse_red", tr("Red ellipse TC"));

	/* User defined palettes. */

	palettes_ << user_palettes_;
}

void MainWindow::update_ui_from_specs()
{
	QComboBox& ckeys = *(ui->cbxKeyPal);
	QComboBox& cpals = *(ui->cbxNewPal);
	QListWidget& cranges = *(ui->listRanges);

	ckeys.clear();
	cpals.clear();

	foreach(const pal_spec& p, palettes_) {
		cpals.addItem(p.name);
		ckeys.addItem(p.name);
	}

	ckeys.setCurrentIndex(0);
	cpals.setCurrentIndex(0);

	ui->listRanges->clear();
	ui->listRanges->addItems(mos_color_range_names);
	ui->listRanges->setCurrentRow(0);

	for(int n = 0; n < ui->listRanges->count(); ++n) {
		QListWidgetItem *i = cranges.item(n);
		i->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		i->setCheckState(Qt::Checked);
		i->setText(capitalize(i->text()));
		// Reset selection to #1
		cranges.setItemSelected(i, n == 0);
	}

	cranges.setCurrentRow(0);
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

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton) {
		this->drag_start_pos_ = event->pos();
		this->drag_use_rc_ = ui->previewRc->geometry().contains(event->pos());
		this->drag_start_ = drag_use_rc_ || ui->previewOriginal->geometry().contains(event->pos());
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(drag_start_ && (event->buttons() & Qt::LeftButton) && (event->pos() - drag_start_pos_).manhattanLength() >= QApplication::startDragDistance()) {

		QDrag *d = new QDrag(this);
		QMimeData *m = new QMimeData();

		if(drag_use_rc_)
			m->setImageData(this->img_transview_);
		else
			m->setImageData(this->img_original_);

		d->setMimeData(m);

		ignore_drops_ = true;
		d->exec(Qt::CopyAction);
		ignore_drops_ = drag_start_ = false;
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
	if(e->mimeData()->hasImage() || e->mimeData()->hasUrls())
		e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e)
{
	if(ignore_drops_)
		return;

	QImage newimg;
	QString newpath = "";

	e->acceptProposedAction();

	if(e->mimeData()->hasImage()) {
		newimg = qvariant_cast<QImage>(e->mimeData()->imageData());
	}
	else if(e->mimeData()->hasUrls()) {
		newpath = e->mimeData()->urls().front().path();
		newimg.load(newpath);
	}

	if(newimg.isNull()) {
		return;
	}

	img_original_ = newimg.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	if(newpath.isEmpty() != true) {
		img_path_ = newpath;
		this->setWindowTitle(tr("Wesnoth RCX") + " - " + img_path_);
	}
	else {
		this->setWindowTitle(tr("Wesnoth RCX") + " - " + tr("Dropped file"));
	}

	refresh_previews();
}

void MainWindow::on_radRc_clicked()
{
	ui->staFunctionOpts->setCurrentIndex(0);
	toggle_page1(true);
	toggle_page2(false);
	refresh_previews();
}

void MainWindow::on_radPal_clicked()
{
	ui->staFunctionOpts->setCurrentIndex(1);
	toggle_page1(false);
	toggle_page2(true);
	refresh_previews();
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

bool MainWindow::initial_open(const QString &initial_file)
{
	try {
		this->do_open(initial_file);
	}
	catch(no_initial_file const&) {
		return false;
	}

	return true;
}

QString MainWindow::supported_file_patterns() const
{
	QString ret;
	QList<QByteArray> supported_fmts = QImageReader::supportedImageFormats();
	bool have_xcf = false;
	bool have_psd = false;

	foreach(QByteArray str, supported_fmts) {
		QString qstr(str);
		if(qstr == "XCF") {
			have_xcf = true;
		}
		else if(qstr == "PSD") {
			have_psd = true;
		}
	}

	ret += tr("All Supported Files") + " (*.png *.bmp";
	if(have_xcf) { ret += " *.xcf"; }
	if(have_psd) { ret += " *.psd"; }

	ret += ");;" + tr("PNG image") + " (*.png);;" + tr("Windows BMP image");
	ret += " (*.bmp);;";

	if(have_xcf)
		ret += tr("GIMP image") + " (*.xcf);;";

	if(have_psd)
		ret += tr("Photoshop image") + " (*.psd);;";

	ret += tr("All files") + " (*)";

	return ret;
}

void MainWindow::do_open(const QString &initial_file)
{
	QString path_temp;
	QString start_dir;

	if(initial_file.isNull() || initial_file.isEmpty()) {
		if(img_path_.isEmpty())
			start_dir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
		else
			start_dir = QFileInfo(img_path_).absolutePath();

		path_temp = QFileDialog::getOpenFileName(
			this,
			tr("Choose source image"),
			start_dir,
			supported_file_patterns()
		);
	}
	else {
		path_temp = initial_file;
	}

	if(path_temp.isNull() && img_original_.isNull()) {
		// it's null if we've just setup the window
		throw no_initial_file();
	}

	QImage img_temp(path_temp);
	if(img_temp.isNull()) {
		if(path_temp.isNull() != true) {
			QMessageBox::information(
				this, tr("Wesnoth RCX"), tr("Could not load %1.").arg(path_temp)
			);
		}
		if(img_original_.isNull()) {
			throw no_initial_file();
		}
		return;
	}

	img_path_ = path_temp;
	// We want to work on actual ARGB data
	img_original_ = img_temp.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	this->setWindowTitle(tr("Wesnoth RCX") + QString(" - " + img_path_));
	refresh_previews();
}

void MainWindow::refresh_previews()
{
	if(this->img_original_.isNull())
		return;

	rc_map cvt_map;
	QList<QRgb> const *key_pal = current_pal_data();

	if(ui->staFunctionOpts->currentIndex()) {
		QList<QRgb> const *target_pal = current_pal_data(true);
		cvt_map = recolor_palettes(*key_pal, *target_pal);
	}
	else {
		cvt_map = recolor_range(
			color_ranges_.at(ui->listRanges->currentIndex().row()).def, *key_pal);
	}

	rc_image(img_original_, img_transview_, cvt_map);

	const int sw = this->img_original_.width() * zoom_;
	const int sh = this->img_original_.height() * zoom_;

	ui->previewOriginal->setPixmap(QPixmap::fromImage(img_original_).scaled(sw, sh));
	ui->previewRc->setPixmap(QPixmap::fromImage(img_transview_).scaled(sw, sh));
}

void MainWindow::do_save()
{
	QString base = QFileDialog::getExistingDirectory(
		this,
		tr("Choose an output directory"),
		QFileInfo(img_path_).absolutePath(),
		QFileDialog::ShowDirsOnly
	);

	if(base.isNull()) {
		return;
	}

	QStringList succeeded;

	try {
		if(ui->staFunctionOpts->currentIndex()) {
			succeeded = do_save_single_recolor(base);
		}
		else {
			succeeded = do_save_color_ranges(base);
		}

		QMessageBox::information(
				this, tr("Wesnoth RCX"), tr("Job successfully saved. The following files have been generated:<br><br>%1").arg(succeeded.join("<br>"))
				);
	} catch(const canceled_job&) {
		;
	} catch(const QStringList& failed) {
		QMessageBox::information(
				this, tr("Wesnoth RCX"), tr("The following files could not be saved correctly:<br><br>%1").arg(failed.join("<br>")));
	}
}

void MainWindow::do_close()
{
	this->close();
}

void MainWindow::do_about()
{
	QMessageBox msg;

	QString text =
		tr("<b>Wesnoth RCX %1</b>").arg(mos_version) + "<br><br>";

	text += tr("Built with Qt %1.").arg(QT_VERSION_STR) + "<br>";
	text += tr("Copyright &copy; 2008, 2009, 2010 by Ignacio R. Morelle.") + "<br>";
	text += "&lt;shadowm@wesnoth.org&gt;<br><br>";

	text += tr(
		"This program is free software; you can redistribute it and/or modify "
		"it under the terms of the GNU General Public License as published by "
		"the Free Software Foundation; either version 2 of the License, or "
		"(at your option) any later version.<br>"
		"<br>"
		"This program is distributed in the hope that it will be useful, but "
		"<b>WITHOUT ANY WARRANTY</b>; without even the implied warranty of "
		"<b>MERCHANTABILITY</b> or <b>FITNESS FOR A PARTICULAR PURPOSE</b>. "
		"See the GNU General Public License for more details."
	);

	msg.setText(text);
	msg.setIconPixmap(QPixmap(":/rsrc/rcx-icon.png"));
	msg.exec();
}

QString MainWindow::current_pal_name(bool palette_switch_mode) const
{
	const int choice = (palette_switch_mode ? ui->cbxNewPal : ui->cbxKeyPal)->currentIndex();
	Q_ASSERT(choice >= 0 && choice < palettes_.size());
	return palettes_.at(choice).id;
}

QList<QRgb> const *MainWindow::current_pal_data(bool palette_switch_mode) const
{
	const int choice = (palette_switch_mode ? ui->cbxNewPal : ui->cbxKeyPal)->currentIndex();
	Q_ASSERT(choice >= 0 && choice < palettes_.size());
	return &(palettes_.at(choice).def);
}

bool MainWindow::confirm_existing_files(QStringList& paths)
{
	return QMessageBox::question(
			this,
			tr("Wesnoth RCX"),
			tr("The following files already exist. Do you want to overwrite them?<br><br>%1").arg(paths.join("<br>")),
			QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
}

QStringList MainWindow::do_save_single_recolor(QString &base)
{
	QString palname = current_pal_name();
	QList<QRgb> const *paldata = current_pal_data();
	QString newpalname = current_pal_name(true);
	QList<QRgb> const *newpaldata = current_pal_data(true);
	QMap<QString, rc_map> rc_job;

	QString path = base + "/" + QFileInfo(img_path_).completeBaseName();
	path += QString("-PAL-") + palname + "-" + newpalname + ".png";

	rc_job[path] = recolor_palettes(*paldata, *newpaldata);

	QStringList paths(path);

	if(QFileInfo(path).exists() && !confirm_existing_files(paths)) {
		throw canceled_job();
	}

	return do_run_jobs(rc_job);
}

QStringList MainWindow::do_save_color_ranges(QString &base)
{
	QString palname = current_pal_name();
	QList<QRgb> const *paldata = current_pal_data();
	QMap<QString, rc_map> rc_jobs;
	QListWidget *list = this->ui->listRanges;

	QStringList existing;

	for(int k = 0; k < list->count(); ++k) {
		if(list->item(k)->checkState() == Qt::Checked) {
			QString path = base + "/" + QFileInfo(img_path_).completeBaseName();
			path += QString("-RC-") + palname + "-";
			path += QString::number(k + 1) + "-" + color_ranges_.at(k).id + ".png";

			rc_jobs[path] = recolor_range(color_ranges_.at(k).def, *paldata);

			if(QFileInfo(path).exists()) {
				existing.push_back(path);
			}
		}
	}

	if(!existing.isEmpty() && !confirm_existing_files(existing)) {
		throw canceled_job();
	}

	return do_run_jobs(rc_jobs);
}

QStringList MainWindow::do_run_jobs(QMap<QString, rc_map> &jobs)
{
	QStringList failed, succeeded;
	QString propaganda = QString("Generated by Morning Star v%1").arg(mos_version);
	this->setEnabled(false);

	for(QMap<QString, rc_map>::const_iterator k = jobs.begin(); k != jobs.end(); ++k) {
		QImage rc;
		QImageWriter out;
		out.setFormat("png");
		out.setFileName(k.key());
		out.setText("", propaganda);

		if(rc_image(this->img_original_, rc, k.value()) && out.write(rc)) {
			succeeded.push_back(out.fileName());
		}
		else {
			failed.push_back(out.fileName());
		}
	}

	this->setEnabled(true);

	if(failed.isEmpty() != true) {
		throw failed;
	}

	return succeeded;
}

void MainWindow::on_action_Save_triggered()
{
	do_save();
}

void MainWindow::on_cbxKeyPal_currentIndexChanged(int /*index*/)
{
	refresh_previews();
}

void MainWindow::on_cbxNewPal_currentIndexChanged(int /*index*/)
{
	refresh_previews();
}

void MainWindow::on_listRanges_itemSelectionChanged()
{
	refresh_previews();
}

void MainWindow::on_cbxZoomFactor_currentIndexChanged(int index)
{
	switch(index) {
	case 0:
		zoom_ = 0.5f; break;
	case 2:
		zoom_ = 2.0f; break;
	case 3:
		zoom_ = 4.0f; break;
	case 4:
		zoom_ = 8.0f; break;
	default:
		zoom_ = 1.0f;
	}

	this->refresh_previews();
}

void MainWindow::on_tbZoomIn_clicked()
{
	QComboBox& zoombox = *(ui->cbxZoomFactor);
	const int ci = zoombox.currentIndex();
	if(ci + 1 < zoombox.count()) {
		zoombox.setCurrentIndex(ci + 1);
	}

	update_zoom_buttons();
}

void MainWindow::on_tbZoomOut_clicked()
{
	QComboBox& zoombox = *(ui->cbxZoomFactor);
	const int ci = zoombox.currentIndex();
	if(ci > 0) {
		zoombox.setCurrentIndex(ci - 1);
	}

	update_zoom_buttons();
}

void MainWindow::update_zoom_buttons()
{
	QComboBox& zoombox = *(ui->cbxZoomFactor);

	ui->tbZoomOut->setEnabled( zoombox.currentIndex() != 0 );
	ui->tbZoomIn->setEnabled(  zoombox.currentIndex() != zoombox.count() - 1 );
}

void MainWindow::on_actionColor_ranges_triggered()
{
	CustomRanges dlg(this, user_ranges_);
	dlg.exec();
	user_ranges_ = dlg.ranges();

	initialize_specs();
	update_ui_from_specs();
	refresh_previews();
}
