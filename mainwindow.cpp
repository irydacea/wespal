//
// codename Morning Star
//
// Copyright (C) 2008 - 2012 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
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

#include "appconfig.hpp"
#include "defs.hpp"
#include "custompalettes.hpp"
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

	class ObjectLock
	{
	public:
		ObjectLock(QObject& o)
			: o_(o)
			, initial_state_(o.blockSignals(true))
		{
		}

		~ObjectLock()
		{
			o_.blockSignals(initial_state_);
		}

	private:
		QObject& o_;
		bool initial_state_;
	};

	inline QString zoom_factor_to_str(float factor)
	{
		return QString("%d%%").arg(int(100.0f * factor));
	}
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

	color_ranges_(),
	palettes_(),

	user_color_ranges_(),
	user_palettes_(),

	ui(new Ui::MainWindow),

	img_path_(),
	img_original_(),
	img_transview_(),
	zoom_(1.0f),
	ignore_drops_(false),
	drag_use_rc_(false),
	drag_start_(false),
	drag_start_pos_(),
	recent_file_acts_(),
	zoom_factors_()
{
    ui->setupUi(this);

	mos_config_load(user_color_ranges_, user_palettes_);

	generateMergedRcDefinitions();
	processRcDefinitions();

	QAction* const act_whatsthis = QWhatsThis::createAction(this);
	ui->menu_Help->insertAction(ui->actionAbout_Morning_Star, act_whatsthis);
	ui->menu_Help->insertSeparator(ui->actionAbout_Morning_Star);

	QPushButton* const save = ui->buttonBox->button(QDialogButtonBox::Save);
	QPushButton* const close = ui->buttonBox->button(QDialogButtonBox::Close);

	save->setWhatsThis(tr("Saves the current recolor job."));
	close->setWhatsThis(tr("Abandons the current job and exits."));

	ui->action_Open->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton, 0, dynamic_cast<QWidget*>(ui->action_Open)));
	ui->action_Save->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton, 0, dynamic_cast<QWidget*>(ui->action_Save)));
	ui->action_Reload->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload, 0, dynamic_cast<QWidget*>(ui->action_Reload)));
	ui->action_Quit->setIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton, 0, dynamic_cast<QWidget*>(ui->action_Quit)));

	for(unsigned k = 0; k < mos_max_recent_files(); ++k) {
		QAction* act = new QAction(this);

		act->setVisible(false);

		QObject::connect(act, SIGNAL(triggered()), this, SLOT(on_action_Recent_triggered()));

		ui->menu_File->insertAction(ui->action_RecentPlaceholder, act);
		recent_file_acts_.push_back(act);
	}

	ui->action_RecentPlaceholder->setVisible(false);

	update_recent_files_menu();

	ui->radRc->setChecked(true);
	ui->staFunctionOpts->setCurrentIndex(0);
	toggle_page2(false);
	toggle_page1(true);

	zoom_factors_ << 0.5f << 1.0f << 2.0f << 4.0f << 8.0f;

	ui->zoomSlider->setMinimum(0);
	ui->zoomSlider->setMaximum(zoom_factors_.size() - 1);
	ui->zoomSlider->setValue(1);

	ui->previewOriginal->setBackgroundRole(QPalette::Base);
	ui->previewRc->setBackgroundRole(QPalette::Base);
	ui->previewOriginalContainer->setBackgroundRole(QPalette::Dark);
	ui->previewRcContainer->setBackgroundRole(QPalette::Dark);

	QObject::connect(
		ui->previewOriginalContainer->horizontalScrollBar(), SIGNAL(valueChanged(int)),
		ui->previewRcContainer->horizontalScrollBar(), SLOT(setValue(int)));
	QObject::connect(
		ui->previewRcContainer->horizontalScrollBar(), SIGNAL(valueChanged(int)),
		ui->previewOriginalContainer->horizontalScrollBar(), SLOT(setValue(int)));
	QObject::connect(
		ui->previewOriginalContainer->verticalScrollBar(), SIGNAL(valueChanged(int)),
		ui->previewRcContainer->verticalScrollBar(), SLOT(setValue(int)));
	QObject::connect(
		ui->previewRcContainer->verticalScrollBar(), SIGNAL(valueChanged(int)),
		ui->previewOriginalContainer->verticalScrollBar(), SLOT(setValue(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateMergedRcDefinitions()
{
	color_ranges_ = mosBuiltinColorRanges;
	color_ranges_.unite(user_color_ranges_);
	palettes_ = mosBuiltinColorPalettes;
	palettes_.unite(user_palettes_);
}

void MainWindow::insertRangeListItem(const QString &id, const QString &display_name)
{
	QListWidgetItem* lwi = new QListWidgetItem(ui->listRanges);

	lwi->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	lwi->setCheckState(Qt::Checked);
	lwi->setText(display_name);
	lwi->setData(Qt::UserRole, id);
}

void MainWindow::processRcDefinitions()
{
	QComboBox* cbOldPals = ui->cbxKeyPal;
	QComboBox* cbNewPals = ui->cbxNewPal;

	cbOldPals->clear();
	cbNewPals->clear();
	ui->listRanges->clear();

	//
	// We allow built-in definitions to be overridden by the user
	// by defining their own ranges/palettes with mainline ids; when
	// that happens, their data is internally merged, but these
	// overridden definitions continue to be displayed before regular
	// user-defined items.
	//
	// This makes the process of entering those definitions for the
	// UI less trivial than it should be. Additionally, we keep only
	// ids for user-defined data and use our own translatable names
	// for displaying built-ins, so we must attach extra data to item
	// widget entries to ensure things look good in the front-end.
	//

	//
	// Add built-in palettes.
	//

	QStringList paletteUiNames;
	// NOTE: these names must correspond to the entries in mosOrderedPaletteNames!
	paletteUiNames << tr("Magenta TC") << tr("Green flag TC") << tr("Red ellipse TC");

	const int builtinPaletteCount = paletteUiNames.size();
	Q_ASSERT(builtinPaletteCount == mosOrderedPaletteNames.size());

	for(int k = 0; k < builtinPaletteCount; ++k) {
		cbOldPals->addItem(paletteUiNames[k], mosOrderedPaletteNames[k]);
		cbNewPals->addItem(paletteUiNames[k], mosOrderedPaletteNames[k]);
	}

	//
	// User-defined palettes.
	//

	const QList<QString>& userPaletteIds = user_palettes_.uniqueKeys();
	foreach(const QString& pal_name, userPaletteIds) {
		if(mosBuiltinColorPalettes.find(pal_name) != mosBuiltinColorPalettes.end()) {
			// Skip redefinitions of built-in palettes, we only care about
			// ids and names at this point.
			continue;
		}
		cbOldPals->addItem(capitalize(pal_name), pal_name);
		cbNewPals->addItem(capitalize(pal_name), pal_name);
	}

	//
	// Built-in color ranges.
	//

	QStringList rangeUiNames;
	// NOTE: these names must correspond to the entries in mosOrderedRangeNames!
	rangeUiNames << tr("Red") << tr("Blue") << tr("Green")
				 << tr("Purple") << tr("Black") << tr("Brown")
				 << tr("Orange") << tr("White") << tr("Teal");

	// It is paramount to ensure built-in ranges are displayed in a specific order,
	// since Wesnoth associates digits from 1 to 9 to items in the sequence and we
	// don't want to break that convention here, for consistency's sake.

	const int builtinRangeCount = rangeUiNames.size();
	Q_ASSERT(builtinRangeCount == mosOrderedRangeNames.size());

	for(int k = 0; k < builtinRangeCount; ++k) {
		insertRangeListItem(mosOrderedRangeNames[k], rangeUiNames[k]);
	}

	//
	// User-defined color ranges
	//

	const QList<QString>& userRangeIds = user_color_ranges_.uniqueKeys();
	foreach(const QString& id, userRangeIds) {
		if(mosBuiltinColorRanges.find(id) != mosBuiltinColorRanges.end()) {
			// Skip redefinitions of built-in ranges, we only care about
			// ids and names at this point.
			continue;
		}
		insertRangeListItem(id, capitalize(id));
	}

	ui->listRanges->setCurrentRow(0);
}

void MainWindow::on_action_Recent_triggered()
{
	QAction* act = qobject_cast<QAction*>(sender());
	if(act) {
		this->do_open(act->data().toString());
	}
}

void MainWindow::update_recent_files_menu()
{
	const QStringList& recent = mos_recent_files();

	for(int k = 0; k < recent_file_acts_.size(); ++k) {
		QAction& act = *recent_file_acts_[k];
		if(k < recent.size()) {
			act.setText(QString("&%1 %2").arg(k + 1).arg(QFileInfo(recent[k]).fileName()));
			act.setData(recent[k]);
			act.setEnabled(true);
			act.setVisible(true);
		} else {
			act.setEnabled(false);
			act.setVisible(false);
		}
	}
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
		update_window_title(img_path_);
	}
	else {
		update_window_title("");
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

void MainWindow::on_action_Reload_triggered()
{
	do_reload();
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
			QMessageBox::critical(
				this, tr("Wesnoth RCX"), tr("Could not load %1.").arg(path_temp)
			);
		}

		if(img_original_.isNull()) {
			throw no_initial_file();
		}

		mos_remove_recent_file(path_temp);

		return;
	}

	img_path_ = path_temp;
	// We want to work on actual ARGB data
	img_original_ = img_temp.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	mos_add_recent_file(img_path_);
	update_recent_files_menu();
	update_window_title(img_path_);
	refresh_previews();
}

void MainWindow::do_reload()
{
	QImage img(img_path_);
	if(img.isNull()) {
		QMessageBox::critical(this, tr("Wesnoth RCX"), tr("Could not reload %1.").arg(img_path_));
		return;
	}

	img_original_ = img.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	refresh_previews();
}

void MainWindow::refresh_previews()
{
	if(this->img_original_.isNull() || this->signalsBlocked())
		return;

	rc_map cvtMap;
	const QList<QRgb>& palData = current_pal_data();

	if(ui->staFunctionOpts->currentIndex()) {
		const QList<QRgb>& targetPalData = current_pal_data(true);
		cvtMap = recolor_palettes(palData, targetPalData);
	} else {
		cvtMap = recolor_range(color_ranges_.value(ui->listRanges->currentIndex().data(Qt::UserRole).toString()), palData);
	}

	rc_image(img_original_, img_transview_, cvtMap);

	const QSize& scaled_size = img_original_.size() * zoom_;

	ui->previewOriginal->setPixmap(QPixmap::fromImage(img_original_).scaled(scaled_size));
	ui->previewRc->setPixmap(QPixmap::fromImage(img_transview_).scaled(scaled_size));

	ui->previewOriginal->resize(scaled_size);
	ui->previewRc->resize(scaled_size);

	ui->previewOriginal->parentWidget()->adjustSize();
	ui->previewRc->parentWidget()->adjustSize();
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
		QMessageBox::critical(
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
		tr("<h3>Wesnoth RCX %1</h3>").arg(mos_version);

	text += "<p>" + tr("Built with Qt %1.").arg(QT_VERSION_STR) + "<br>";
	text += tr("Copyright &copy; 2008 &#8211; 2012 by Ignacio Riquelme Morelle.") + "<br>";
	text += "&lt;shadowm@wesnoth.org&gt;</p>";

	text += "<p>" +
		tr("This program is free software; you can redistribute it and/or modify "
		"it under the terms of the GNU General Public License as published by "
		"the Free Software Foundation; either version 2 of the License, or "
		"(at your option) any later version.") + "</p><p>" +
		tr("This program is distributed in the hope that it will be useful, but "
		"<b>WITHOUT ANY WARRANTY</b>; without even the implied warranty of "
		"<b>MERCHANTABILITY</b> or <b>FITNESS FOR A PARTICULAR PURPOSE</b>. "
		"See the GNU General Public License for more details.") + "</p>";

	msg.setText(text);
	msg.setIconPixmap(QPixmap(":/rsrc/rcx-icon.png"));
	msg.setWindowIcon(this->windowIcon());
	msg.setWindowTitle(tr("Wesnoth RCX"));
	msg.exec();
}

QString MainWindow::current_pal_name(bool palette_switch_mode) const
{
	QComboBox* combow = palette_switch_mode ? ui->cbxNewPal : ui->cbxKeyPal;

	const int choice = combow->currentIndex();
	const QString& palette_name = combow->itemData(choice).toString();

	Q_ASSERT(!palette_name.isEmpty());

	return palette_name;
}

QList<QRgb> MainWindow::current_pal_data(bool palette_switch_mode) const
{
	return palettes_.value(current_pal_name(palette_switch_mode));
}

bool MainWindow::confirm_existing_files(const QStringList& paths)
{
	return QMessageBox::question(
			this,
			tr("Wesnoth RCX"),
			tr("The following files already exist. Do you want to overwrite them?<br><br>%1").arg(paths.join("<br>")),
			QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
}

QStringList MainWindow::do_save_single_recolor(QString &base)
{
	QMap<QString, rc_map> jobs;

	const QString& palId = current_pal_name();
	const QList<QRgb>& palData = current_pal_data();

	const QString& targetPalId = current_pal_name(true);
	const QList<QRgb>& targetPalData = current_pal_data(true);

	const QString& filePath = base + "/" + QFileInfo(img_path_).completeBaseName() +
			"-PAL-" + palId + "-" + targetPalId + ".png";

	jobs[filePath] = recolor_palettes(palData, targetPalData);

	if(QFileInfo(filePath).exists() && !confirm_existing_files(QStringList(filePath))) {
		throw canceled_job();
	}

	return do_run_jobs(jobs);
}

QStringList MainWindow::do_save_color_ranges(QString &base)
{
	QMap<QString, rc_map> jobs;

	const QString& palId = current_pal_name();
	const QList<QRgb>& palData = current_pal_data();

	QStringList needOverwriteFiles;

	for(int k = 0; k < ui->listRanges->count(); ++k) {
		QListWidgetItem* itemw = ui->listRanges->item(k);
		Q_ASSERT(itemw);

		if(itemw->checkState() == Qt::Checked) {
			const QString& rangeId = itemw->data(Qt::UserRole).toString();

			const QString& filePath = base + "/" + QFileInfo(img_path_).completeBaseName() +
					"-RC-" + palId + "-" + QString::number(k + 1) +
					"-" + rangeId + ".png";

			jobs[filePath] = recolor_range(color_ranges_.value(rangeId), palData);

			if(QFileInfo(filePath).exists()) {
				needOverwriteFiles.push_back(filePath);
			}
		}
	}

	if(!needOverwriteFiles.isEmpty() && !confirm_existing_files(needOverwriteFiles)) {
		throw canceled_job();
	}

	return do_run_jobs(jobs);
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

void MainWindow::on_listRanges_currentRowChanged(int /*currentRow*/)
{
	refresh_previews();
}


void MainWindow::on_zoomSlider_valueChanged(int value)
{
	Q_ASSERT(value >= 0 && value < zoom_factors_.size());
	zoom_ = zoom_factors_[value];

	update_zoom_buttons();
	refresh_previews();
}

void MainWindow::on_tbZoomIn_clicked()
{
	ui->zoomSlider->setValue(ui->zoomSlider->value() + 1);
}

void MainWindow::on_tbZoomOut_clicked()
{
	ui->zoomSlider->setValue(ui->zoomSlider->value() - 1);
}

void MainWindow::update_zoom_buttons()
{
	QSlider& zoomSlider= *ui->zoomSlider;

	ui->tbZoomOut->setEnabled(zoomSlider.value() != zoomSlider.minimum());
	ui->tbZoomIn->setEnabled(zoomSlider.value() != zoomSlider.maximum());
}

void MainWindow::on_actionColor_ranges_triggered()
{
	CustomRanges dlg(this, user_color_ranges_);
	dlg.exec();

	if(dlg.result() == QDialog::Rejected)
		return;

	user_color_ranges_ = dlg.ranges();

	{
		ObjectLock l(*this);
		generateMergedRcDefinitions();
		processRcDefinitions();
	}

	refresh_previews();

	mos_config_save(user_color_ranges_, user_palettes_);
}

void MainWindow::on_action_Palettes_triggered()
{
	CustomPalettes dlg(this);

	dlg.addMultiplePalettes(palettes_);
	dlg.exec();

	// TODO: implement saving palettes to configuration and
	//       refreshing the UI accordingly.
}
