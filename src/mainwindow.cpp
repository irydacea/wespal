/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2008 - 2025 by Iris Morelle <iris@irydacea.me>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "appconfig.hpp"
#include "codesnippetdialog.hpp"
#include "defs.hpp"
#include "mainwindow.hpp"
#include "paletteitem.hpp"
#include "settingsdialog.hpp"
#include "ui_mainwindow.h"
#include "util.hpp"

#include <QActionGroup>
#include <QButtonGroup>
#include <QClipboard>
#include <QColorDialog>
#include <QDesktopServices>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>
#include <QMimeData>
#include <QScrollBar>
#include <QSplitter>
#include <QStringBuilder>
#include <QWhatsThis>

namespace {

struct canceled_job    {};

enum RcModePage {
	RcModeColorRangePage = 0,
	RcModePaletteSwapPage,
	RcModeColorBlendPage,
	RcModeColorShiftPage,
};

enum WorkAreaPage {
	WorkAreaStartPage = 0,
	WorkAreaSplitRc,
	WorkAreaCompositeRc,
};

} // end unnamed namespace

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)

	, ui(new Ui::MainWindow)

	, colorRanges_()
	, palettes_()

	, userColorRanges_(MosCurrentConfig().customColorRanges())
	, userPalettes_(MosCurrentConfig().customPalettes())

	, imagePath_()
	, searchDirPath_()
	, saveDirPath_()

	, originalImage_()
	, transformedImage_()

	, viewMode_()
	, rcMode_()

	, zoom_(MosCurrentConfig().defaultZoom())

	, blendColor_(Qt::green)
	, blendFactor_(0.5)

	, colorShiftRed_(64)
	, colorShiftGreen_(64)
	, colorShiftBlue_(64)

	, ignoreDrops_(false)
	, dragUseRecolored_(false)
	, dragStart_(false)
	, dragStartPos_()

	, panStart_(false)
	, panStartPos_()

	, recentFileActions_()
	, zoomActions_()
	, viewModeActions_()

	, compositeShortcutsGroup_(nullptr)

	, supportedImageFileFormats_(MosPlatform::supportedImageFileFormats())
{
	//
	// Window initialisation
	//

	ui->setupUi(this);

#ifdef Q_OS_MACOS
	// smol sliders c:
	ui->viewSlider->setAttribute(Qt::WA_MacMiniSize);
	ui->zoomSlider->setAttribute(Qt::WA_MacMiniSize);
#endif

	if (MosCurrentConfig().rememberMainWindowSize()) {
		const auto& lastWindowSize = MosCurrentConfig().mainWindowSize();

		if (lastWindowSize.isValid()) {
			resize(lastWindowSize);
		}
	}

	auto* workAreaSplitter = new QSplitter(this);
	ui->workAreaRootLayout->addWidget(workAreaSplitter);
	workAreaSplitter->addWidget(ui->workAreaImagePanel);
	workAreaSplitter->addWidget(ui->workAreaOptionsPanel);

	// Attempt to use the Left format icon as a fallback for the Code button
	// (mainly for macOS and Windows, although its original theme icon isn't
	// technically standard anyway).
	if (ui->cmdGenerateWml->icon().isNull())
		ui->cmdGenerateWml->setIcon(QIcon::fromTheme("format-justify-left"));

	if (!ui->cmdGenerateWml->icon().isNull()) {
		ui->cmdGenerateWml->setToolTip(tr("Code"));
		ui->cmdGenerateWml->setText({});
	}

	//
	// Wesnoth recoloring system data
	//

	generateMergedRcDefinitions();

	// Set icon sizes before generating entries in processRcDefinitions()
	ui->cbxKeyPal->setIconSize(defaultColorIconSize);
	ui->cbxNewPal->setIconSize(defaultColorIconSize);
	ui->listRanges->setIconSize(defaultColorIconSize);

	processRcDefinitions();

	//
	// General menu setup
	//

	const auto& helpMenuActions = ui->menu_Help->actions();
	auto* firstHelpAction = helpMenuActions.empty()
							? nullptr
							: helpMenuActions[0];

	auto* whatsThisAction = QWhatsThis::createAction(this);

	ui->menu_Help->insertAction(firstHelpAction, whatsThisAction);
	ui->menu_Help->insertSeparator(firstHelpAction);

	connect(ui->actionChangeLog, &QAction::triggered, this, []() {
		MosUi::openReleaseNotes();
	});
	connect(ui->actionReportBugs, &QAction::triggered, this, []() {
		MosUi::openIssueTracker();
	});

	ui->action_Open->setShortcut(QKeySequence::Open);
	ui->action_Save->setShortcut(QKeySequence::Save);
	ui->action_Reload->setShortcut(QKeySequence::Refresh);
	ui->action_Close->setShortcut(QKeySequence::Close);
	ui->action_Quit->setShortcut(QKeySequence::Quit);

	ui->actionCopy->setShortcut(QKeySequence::Copy);
	ui->actionPaste->setShortcut(QKeySequence::Paste);
	// Intentionally skip this. Qt automatically sets the appropriate shortcut
	// on macOS, and on other platforms it seems unaware of the standard one
	// (at least according to the QKeySequence enum's documentation).
	//ui->actionAppSettings->setShortcut(QKeySequence::Preferences);

	ui->actionZoomIn->setShortcut(QKeySequence::ZoomIn);
	ui->actionZoomOut->setShortcut(QKeySequence::ZoomOut);

	// Set native icons
	if (auto qStyle = style(); qStyle) {
#ifndef Q_OS_MACOS
		ui->cmdOpen->setIcon(qStyle->standardIcon(
								 QStyle::SP_DialogOpenButton,
								 nullptr,
								 ui->cmdOpen));
#endif
		// The .ui has menu icons which are normally provided by the theme on
		// Freedesktop.org-compliant platforms. If the Close icon is missing
		// then that's a good indication that we need to query QStyle for
		// fallback versions.
		if (ui->action_Close->icon().isNull()) {
			ui->action_Open->setIcon(qStyle->standardIcon(QStyle::SP_DialogOpenButton, nullptr, ui->menu_File));
			ui->action_Save->setIcon(qStyle->standardIcon(QStyle::SP_DialogSaveButton, nullptr, ui->menu_File));
			ui->action_Reload->setIcon(qStyle->standardIcon(QStyle::SP_BrowserReload, nullptr, ui->menu_File));
			ui->action_Close->setIcon(qStyle->standardIcon(QStyle::SP_DialogCloseButton, nullptr, ui->menu_File));
			ui->action_Quit->setIcon(qStyle->standardIcon(QStyle::SP_DialogCancelButton, nullptr, ui->menu_File));
		}
	}

	//
	// Button box
	//

	auto* saveButton = ui->buttonBox->button(QDialogButtonBox::Save);

	saveButton->setWhatsThis(tr("Saves the current recolor job."));

	//
	// MRU menu & list widget
	//

	auto maxMruEntries = MosCurrentConfig().recentFiles().max();

	recentFileActions_.reserve(maxMruEntries);

	for (unsigned k = 0; k < maxMruEntries; ++k) {
		auto* act = new QAction(this);

		act->setEnabled(false);
		act->setVisible(false);
		act->setIconVisibleInMenu(true);

		connect(act, SIGNAL(triggered()), this, SLOT(handleRecent()));

		ui->menuMru->insertAction(ui->action_MruPlaceholder, act);
		recentFileActions_.push_back(act);
	}

	ui->action_MruPlaceholder->setVisible(false);

	ui->listMru->setIconSize(MosConfig::MruEntry::thumbnailSize() * 0.66);
	ui->listMru->setWordWrap(true);
	ui->listMru->setWrapping(false);

	connect(ui->listMru, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(handleRecent()));

	updateRecentFilesMenu();

	//
	// Color range options page
	//

	auto selectAllIcon = QIcon::fromTheme("edit-select-all");
	auto selectSingleIcon = QIcon::fromTheme("edit-select-none");

	if (!selectAllIcon.isNull() && !selectSingleIcon.isNull()) {
		ui->cmdRcSelectAll->setIcon(selectAllIcon);
		ui->cmdRcSelectSingle->setIcon(selectSingleIcon);

		ui->cmdRcSelectAll->setToolTip(tr("All"));
		ui->cmdRcSelectSingle->setToolTip(tr("Single"));

		ui->cmdRcSelectAll->setText({});
		ui->cmdRcSelectSingle->setText({});
	}

	connect(ui->cmdRcSelectAll, &QAbstractButton::clicked, this,
			[this]() { onRcSelectButtonClicked(true); });
	connect(ui->cmdRcSelectSingle, &QAbstractButton::clicked, this,
			[this]() { onRcSelectButtonClicked(false); });

	//
	// Color blend options page
	//

	connect(ui->sliderBlendFactor, SIGNAL(valueChanged(int)), ui->spinBlendFactor, SLOT(setValue(int)));
	connect(ui->spinBlendFactor, SIGNAL(valueChanged(int)), ui->sliderBlendFactor, SLOT(setValue(int)));

	connect(ui->spinBlendFactor, SIGNAL(valueChanged(int)), this, SLOT(onColorBlendFactorChanged(int)));

	connect(ui->leBlendColor, SIGNAL(textChanged(QString)), this, SLOT(onColorBlendLineEditChanged(QString)));
	connect(ui->tbBlendColor, SIGNAL(clicked()), this, SLOT(onColorBlendButtonClicked()));

	ui->spinBlendFactor->setValue(blendFactor_ * 100);
	ui->leBlendColor->setText(blendColor_.name()); // Sets the icon too

	//
	// Color shift options page
	//

	connect(ui->sliderRedShift, SIGNAL(valueChanged(int)), ui->spinRedShift, SLOT(setValue(int)));
	connect(ui->spinRedShift, SIGNAL(valueChanged(int)), ui->sliderRedShift, SLOT(setValue(int)));

	connect(ui->sliderGreenShift, SIGNAL(valueChanged(int)), ui->spinGreenShift, SLOT(setValue(int)));
	connect(ui->spinGreenShift, SIGNAL(valueChanged(int)), ui->sliderGreenShift, SLOT(setValue(int)));

	connect(ui->sliderBlueShift, SIGNAL(valueChanged(int)), ui->spinBlueShift, SLOT(setValue(int)));
	connect(ui->spinBlueShift, SIGNAL(valueChanged(int)), ui->sliderBlueShift, SLOT(setValue(int)));

	connect(ui->spinRedShift, &QSpinBox::valueChanged, this,
			std::bind(&MainWindow::onColorShiftValueChanged, this, ColorShiftRed, std::placeholders::_1));
	connect(ui->spinGreenShift, &QSpinBox::valueChanged, this,
			std::bind(&MainWindow::onColorShiftValueChanged, this, ColorShiftGreen, std::placeholders::_1));
	connect(ui->spinBlueShift, &QSpinBox::valueChanged, this,
			std::bind(&MainWindow::onColorShiftValueChanged, this, ColorShiftBlue, std::placeholders::_1));

	ui->spinRedShift->setValue(colorShiftRed_);
	ui->spinGreenShift->setValue(colorShiftGreen_);
	ui->spinBlueShift->setValue(colorShiftBlue_);

	//
	// Zoom slider & menu items
	//

	ui->zoomSlider->setMinimum(0);
	ui->zoomSlider->setMaximum(int(zoomFactors_.size()) - 1);
	ui->zoomSlider->setValue(1);

	auto* zoomMenuActs = new QActionGroup(this);
	std::array zoomMenuItems = {
		ui->actionZoom50,
		ui->actionZoom100,
		ui->actionZoom200,
		ui->actionZoom400,
		ui->actionZoom800,
		ui->actionZoom1600,
	};

	static_assert(zoomFactors_.size() == zoomMenuItems.size());

	for (unsigned k = 0; k < zoomMenuItems.size(); ++k)
	{
		auto* item = zoomMenuItems[k];

		zoomMenuActs->addAction(item);
		item->setData(k);
		item->setChecked(zoomFactors_[k] == zoom_);

		if (zoomFactors_[k] == zoom_)
			ui->zoomSlider->setValue(k);

		connect(item, &QAction::triggered, this, [this](bool) {
			QAction* const act = qobject_cast<QAction*>(sender());

			if (!act)
				return;

			ui->zoomSlider->setValue(act->data().toInt());
		});
	}

	zoomActions_ = zoomMenuActs->actions();

	//
	// View mode menu items
	//

	// NOTE: Our view mode will be committed later at the end of setup
	auto viewMode = MosCurrentConfig().rememberImageViewMode()
					? MosCurrentConfig().imageViewMode()
					: MosConfig::ImageViewMode();

	auto* viewMenuActs = new QActionGroup(this);
	std::array viewMenuItems = {
		ui->actionViewVSplit,
		ui->actionViewHSplit,
		ui->actionViewSwipe,
		ui->actionViewOnionSkin,
	};

	static_assert(MosConfig::ImageViewSize == viewMenuItems.size());

	for (unsigned k = 0; k < viewMenuItems.size(); ++k)
	{
		auto* item = viewMenuItems[k];

		viewMenuActs->addAction(item);
		item->setData(k);
		item->setChecked(k == viewMode);

		connect(item, &QAction::triggered, this, [this](bool) {
			QAction* const act = qobject_cast<QAction*>(sender());

			if (!act)
				return;

			setViewMode(ViewMode(act->data().toUInt()));
		});
	}

	viewModeActions_ = viewMenuActs->actions();

	//
	// Background color menu
	//

	const auto& bgColorName = MosCurrentConfig().previewBackgroundColor();
	auto* bgColorActs = new QActionGroup(this);

	// The Custom Color entry goes into the list first so that it is preemptively
	// selected first on the next loop in case the color saved in preferences isn't
	// any of the predefined ones.
	QList<QPair<QAction*, QString>> bgColorActsItems = {
		{ ui->actionPreviewBgBlack,   QColor(Qt::black).name()     },
		{ ui->actionPreviewBgDark,    QColor(Qt::darkGray).name()  },
		{ ui->actionPreviewBgDefault, {}                           },
		{ ui->actionPreviewBgLight,   QColor(Qt::lightGray).name() },
		{ ui->actionPreviewBgWhite,   QColor(Qt::white).name()     },
		{ ui->actionPreviewBgCustom,  bgColorName                  },
	};

	bool stockColorSelected = false;

	for (const auto& [action, data] : bgColorActsItems)
	{
		bgColorActs->addAction(action);
		action->setData(data);

		connect(action, SIGNAL(triggered(bool)), this, SLOT(handlePreviewBgOption(bool)));

		// We must find the menu item for the color we read from the app
		// config and mark it as checked, and update the preview background
		// color manually since setChecked() won't raise the triggered()
		// signal.
		if (data == bgColorName) {
			// Avoid marking both Custom and a stock color
			if (!stockColorSelected)
				action->setChecked(true);
			stockColorSelected = true;
			setPreviewBackgroundColor(bgColorName);
		}
	}

	// Make custom bg color icon always visible even on macOS
	ui->actionPreviewBgCustom->setIconVisibleInMenu(true);
	updateCustomPreviewBgIcon();

	//
	// Image preview widgets
	//

	ui->previewOriginalContainer->viewport()->setBackgroundRole(QPalette::Dark);
	ui->previewRcContainer->viewport()->setBackgroundRole(QPalette::Dark);
	ui->previewCompositeContainer->viewport()->setBackgroundRole(QPalette::Dark);

	// FIXME: hack to prevent Oxygen/Breeze stealing our drag events when
	// dragging windows from empty areas is enabled.
	//
	// <https://marc.info/?l=kde-devel&m=130530904703913&w=2>
	//
	// (As of KDE Frameworks 6, Breeze is especially aggressive about this and
	// we can't work around it by setting the property on the individual
	// widgets anymore -- although it may also be a Wayland-specific thing.)
	//
	// We should probably figure out a better way to do this later, as well
	// as the preview panels themselves; the proper way according to the
	// Oxygen dev is to prevent (at the widget level) propagation of the event
	// to the window widget.

	setProperty("_kde_no_window_grab", true);

	// Ensure split view scrollbars are in sync on both sides

	auto* originalHScroll = ui->previewOriginalContainer->horizontalScrollBar();
	auto* rcHScroll = ui->previewRcContainer->horizontalScrollBar();

	auto* originalVScroll = ui->previewOriginalContainer->verticalScrollBar();
	auto* rcVScroll = ui->previewRcContainer->verticalScrollBar();

	connect(originalHScroll, SIGNAL(valueChanged(int)), rcHScroll, SLOT(setValue(int)));
	connect(rcHScroll, SIGNAL(valueChanged(int)), originalHScroll, SLOT(setValue(int)));

	connect(originalVScroll, SIGNAL(valueChanged(int)), rcVScroll, SLOT(setValue(int)));
	connect(rcVScroll, SIGNAL(valueChanged(int)), originalVScroll, SLOT(setValue(int)));

	compositeShortcutsGroup_ = new QButtonGroup(this);

	compositeShortcutsGroup_->setExclusive(true);
	compositeShortcutsGroup_->addButton(ui->compositeOriginalOnlyToggle);
	compositeShortcutsGroup_->addButton(ui->compositeRcOnlyToggle);

	if (!ui->compositeOriginalOnlyToggle->icon().isNull())
		ui->compositeOriginalOnlyToggle->setText({});
	if (!ui->compositeRcOnlyToggle->icon().isNull())
		ui->compositeRcOnlyToggle->setText({});

	ui->previewComposite->setDisplayRatio(1.0);
	ui->compositeRcOnlyToggle->setChecked(true);

	// Hide the shortcuts panel manually since setViewMode() below won't do
	// anything if viewMode == viewMode_ (yes we could fix that but it's kinda
	// unnecessary to do the layout dance for everything else).
	if (viewMode == MosConfig::ImageViewVSplit || viewMode == MosConfig::ImageViewHSplit)
		ui->compositeShortcutsPanel->setVisible(false);

	//
	// Finalise initial workarea setup
	//

	connect(QGuiApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)),
			this, SLOT(onClipboardChanged(QClipboard::Mode)));
	onClipboardChanged(QClipboard::Clipboard); // Need to do an initial poll

	ui->radRc->setChecked(true);
	setRcMode(RcColorRange);
	setViewMode(viewMode);
	enableWorkArea(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateWindowTitle(bool hasImage,
								   const QString& filename,
								   ImageOrigin origin)
{
	QString appTitle = tr("Wespal");
	QString displayString;

	if (hasImage) {
		if (filename.isEmpty()) {
			setWindowFilePath({});
			switch (origin)
			{
				case ImageOriginDrop:
					displayString = tr("Dropped image");
					break;
				case ImageOriginClipboard:
					displayString = tr("Clipboard image");
					break;
				default:
					displayString = tr("Unknown image");
					break;
			}
		} else {
			setWindowFilePath(filename);
			displayString = QFileInfo(filename).fileName();
		}

		setWindowTitle(displayString % QString::fromUtf8(" \342\200\224 ") % appTitle);
	} else {
		setWindowFilePath({});
		setWindowTitle(appTitle);
	}
}

void MainWindow::generateMergedRcDefinitions()
{
	colorRanges_ = wesnoth::builtinColorRanges.objects();
	colorRanges_.insert(userColorRanges_);
	palettes_ = wesnoth::builtinPalettes.objects();
	palettes_.insert(userPalettes_);
}

void MainWindow::insertRangeListItem(const QString &id, const QString &display_name, const ColorRange& colorRange)
{
	auto* lwi = new QListWidgetItem(ui->listRanges);
	auto colorIcon = createColorRangeIcon(colorRange, ui->listRanges);

	lwi->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	lwi->setCheckState(Qt::Checked);
	lwi->setText(display_name);
	lwi->setIcon(colorIcon);
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

	for (int k = 0; k < wesnoth::builtinPalettes.objectCount(); ++k)
	{
		const auto& palName = wesnoth::builtinPalettes.orderedNames()[k];
		const auto& uiName = wesnoth::builtinPalettes.orderedTranslatableNames()[k];
		auto colorIcon = createPaletteIcon(wesnoth::builtinPalettes[palName], defaultColorIconSize, cbOldPals);
		cbOldPals->addItem(colorIcon, uiName, palName);
		cbNewPals->addItem(colorIcon, uiName, palName);
	}

	//
	// User-defined palettes.
	//

	for (const auto& [palName, palette] : userPalettes_.asKeyValueRange())
	{
		if (wesnoth::builtinPalettes.hasName(palName)) {
			// Skip redefinitions of built-in palettes, we only care about
			// ids and names at this point.
			continue;
		}
		auto colorIcon = createPaletteIcon(palette, defaultColorIconSize, cbOldPals);
		cbOldPals->addItem(colorIcon, capitalize(palName), palName);
		cbNewPals->addItem(colorIcon, capitalize(palName), palName);
	}

	//
	// Built-in color ranges.
	//

	// It is paramount to ensure built-in ranges are displayed in a specific order,
	// since Wesnoth associates digits from 1 to 9 to items in the sequence and we
	// don't want to break that convention here, for consistency's sake.

	for (int k = 0; k < wesnoth::builtinColorRanges.objectCount(); ++k)
	{
		auto& rangeName = wesnoth::builtinColorRanges.orderedNames()[k];
		auto& uiName = wesnoth::builtinColorRanges.orderedTranslatableNames()[k];
		insertRangeListItem(rangeName, uiName, wesnoth::builtinColorRanges[rangeName]);
	}

	//
	// User-defined color ranges
	//

	for (const auto& [id, colorRange] : userColorRanges_.asKeyValueRange())
	{
		if (wesnoth::builtinColorRanges.hasName(id)) {
			// Skip redefinitions of built-in ranges, we only care about
			// ids and names at this point.
			continue;
		}
		insertRangeListItem(id, capitalize(id), colorRange);
	}

	ui->listRanges->setCurrentRow(0);
}

void MainWindow::handleRecent()
{
	if (auto* act = qobject_cast<QAction*>(sender()); act) {
		openFile(act->data().toString());
		return;
	}

	if (auto* listMru = qobject_cast<QListWidget*>(sender()); listMru) {
		openFile(listMru->currentItem()->data(Qt::UserRole).toString());
		//return;
	}
}

void MainWindow::updateRecentFilesMenu()
{
	int k = 0;

	ui->listMru->clear();

	for (const auto& entry : MosCurrentConfig().recentFiles())
	{
		if (k >= recentFileActions_.size())
			break;

		auto& act = *recentFileActions_[k];

		const auto& filePath = entry.filePath();
		const auto& fileName = QFileInfo(filePath).fileName();
		const auto& label =
				QString("&%1 %2").arg(k + 1).arg(fileName);
		const auto& thumbnail =
				QPixmap::fromImage(entry.thumbnail());
		const auto& miniThumbnail =
				QPixmap::fromImage(entry.miniThumbnail());

		act.setText(label);
		act.setIcon(miniThumbnail);
		act.setData(filePath);

		act.setEnabled(true);
		act.setVisible(true);

		auto* listItem = new QListWidgetItem(ui->listMru);

		listItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		listItem->setText(fileName);
		listItem->setToolTip(filePath);
		listItem->setIcon(thumbnail);
		listItem->setData(Qt::UserRole, filePath);

		++k;
	}

	// Disable and hide any extra menu items (e.g. after MruList has coalesced
	// duplicates).

	for (; k < recentFileActions_.size(); ++k)
	{
		auto& act = *recentFileActions_[k];

		act.setEnabled(false);
		act.setVisible(false);
	}

	// Disable the menu entirely if there are no MRU items.

	ui->menuMru->setEnabled(!MosCurrentConfig().recentFiles().empty());

	// The MRU panel gets fully hidden to avoid confusion due to its styling.

	ui->panelMru->setVisible(!MosCurrentConfig().recentFiles().empty());
}

void MainWindow::changeEvent(QEvent* event)
{
	QMainWindow::changeEvent(event);

	switch (event->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
    }
}

void MainWindow::closeEvent(QCloseEvent*)
{
	if (MosCurrentConfig().rememberMainWindowSize()) {
		MosCurrentConfig().setMainWindowSize(size());
	}
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
	if (event->angleDelta().x() == 0 && event->modifiers() & Qt::ControlModifier)
	{
		//
		// Ctrl+Scroll wheel zoom
		//

		if (event->angleDelta().y() > 0) {
			adjustZoom(ZoomIn);
		} else if (event->angleDelta().y() < 0) {
			adjustZoom(ZoomOut);
		}

		event->accept();
	}
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
	const auto& globalPos = event->globalPosition();

	const auto& compositeRelPos = ui->previewCompositeContainer->mapFromGlobal(globalPos).toPoint();
	const auto& ogRelPos = ui->previewOriginalContainer->mapFromGlobal(globalPos).toPoint();
	const auto& rcRelPos = ui->previewRcContainer->mapFromGlobal(globalPos).toPoint();

	if (event->button() == Qt::LeftButton && hasImage())
	{
		//
		// Drag-to-copy action
		//

		dragStartPos_ = event->position();

		switch (viewMode_)
		{
			case MosConfig::ImageViewSwipe:
			case MosConfig::ImageViewOnionSkin:
				dragUseRecolored_ = true;
				dragStart_ = ui->previewCompositeContainer->rect().contains(compositeRelPos);
				break;
			default:
				dragUseRecolored_ = ui->previewRcContainer->rect().contains(rcRelPos);
				dragStart_ = dragUseRecolored_ || ui->previewOriginalContainer->rect().contains(ogRelPos);
				break;
		}
	}
	else if (event->button() == Qt::MiddleButton && hasImage())
	{
		//
		// Image preview panning
		//

		panStartPos_ = event->position();

		switch (viewMode_)
		{
			case MosConfig::ImageViewSwipe:
			case MosConfig::ImageViewOnionSkin:
				panStart_ = ui->previewCompositeContainer->rect().contains(compositeRelPos);
				break;
			default:
				panStart_ = ui->previewOriginalContainer->rect().contains(ogRelPos) ||
							ui->previewRcContainer->rect().contains(rcRelPos);;
				break;
		}
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (dragStart_ && (event->buttons() & Qt::LeftButton))
	{
		//
		// Drag-to-copy action
		//

		auto delta = event->position() - dragStartPos_;

		if (delta.manhattanLength() < QApplication::startDragDistance())
			return;

		QImage& source = dragUseRecolored_ ? transformedImage_ : originalImage_;

		static constexpr QSize maxDragPixmapSize{128, 128};
		auto dragPixmapSize = source.size();

		if (dragPixmapSize.width() > maxDragPixmapSize.width() ||
			dragPixmapSize.height() > maxDragPixmapSize.height())
		{
			dragPixmapSize.scale(maxDragPixmapSize, Qt::KeepAspectRatio);
		}

		QPixmap dragPixmap{dragPixmapSize};

		dragPixmap.fill(Qt::transparent);

		{
			QPainter painter{&dragPixmap};
			painter.setOpacity(0.80);
			painter.drawImage(QRect{{0, 0}, dragPixmapSize}, source);
		}

		auto* drag = new QDrag(this);
		auto* mime = new QMimeData();

		mime->setImageData(source);
		drag->setMimeData(mime);
		drag->setPixmap(dragPixmap);
		drag->setHotSpot({dragPixmapSize.width() / 2, dragPixmapSize.height()});

		ignoreDrops_ = true;
		drag->exec(Qt::CopyAction);
		ignoreDrops_ = dragStart_ = false;
	}
	else if (panStart_ && (event->buttons() & Qt::MiddleButton))
	{
		//
		// Image preview panning
		//

		QScrollArea* target;

		switch (viewMode_)
		{
			case MosConfig::ImageViewSwipe:
			case MosConfig::ImageViewOnionSkin:
				target = ui->previewCompositeContainer;
				break;
			default:
				target = ui->previewOriginalContainer;
				break;
		}

		// Because the split view container scrollbars are connected together
		// via signals, we only ever need to adjust the scrollbars for one of
		// the view containers instead of both sets, yay~ ^-^

		auto* vScroll = target->verticalScrollBar();
		auto* hScroll = target->horizontalScrollBar();

		auto delta = panStartPos_ - event->position();

		vScroll->setValue(vScroll->value() + delta.y());
		hScroll->setValue(hScroll->value() + delta.x());

		panStartPos_ = event->position();
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent*)
{
	if (panStart_) {
		panStart_ = false;
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData() && (event->mimeData()->hasImage() || event->mimeData()->hasUrls())) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent* event)
{
	if (ignoreDrops_ || !event->mimeData())
		return;

	QImage newimg;
	QString newpath;

	event->acceptProposedAction();

	if (event->mimeData()->hasImage()) {
		newimg = qvariant_cast<QImage>(event->mimeData()->imageData());
	} else if (event->mimeData()->hasUrls()) {
		newpath = event->mimeData()->urls().front().path();
		newimg.load(newpath);
	}

	if (newimg.isNull()) {
		return;
	}

	originalImage_ = newimg.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	if (newpath.isEmpty() != true) {
		imagePath_ = newpath;
		updateWindowTitle(true, imagePath_, ImageOriginDrop);
	} else {
		imagePath_ = tr("Dropped image") % ".png";
		updateWindowTitle(true, {}, ImageOriginDrop);
	}

	refreshPreviews();
	enableWorkArea(true);
}

void MainWindow::on_radRc_clicked()
{
	setRcMode(RcColorRange);
}

void MainWindow::on_radPal_clicked()
{
	setRcMode(RcPaletteSwap);
}

void MainWindow::on_radBlend_clicked()
{
	setRcMode(RcColorBlend);
}

void MainWindow::on_radShift_clicked()
{
	setRcMode(RcColorShift);
}

void MainWindow::on_actionAbout_Morning_Star_triggered()
{
	doAboutDialog();
}

void MainWindow::on_buttonBox_clicked(QAbstractButton* button)
{
	QDialogButtonBox::StandardButton btype = ui->buttonBox->standardButton(button);

	switch (btype) {
		case QDialogButtonBox::Save:
			doSaveFile();
			break;
		case QDialogButtonBox::Close:
			if (!hasImage()) {
				close();
			} else {
				doCloseFile();
			}
			break;
		default:
			;
	}
}

void MainWindow::on_action_Open_triggered()
{
	openFile();
}

void MainWindow::on_action_Quit_triggered()
{
	doCloseFile();
	close();
}

void MainWindow::on_action_Reload_triggered()
{
	doReloadFile();
}

void MainWindow::openFile(const QString& fileName)
{
	QString selectedPath;
	QString initialDirPath = searchDirPath_;

	if (!fileName.isEmpty()) {
		selectedPath = fileName;
	} else {
		if (initialDirPath.isEmpty()) {
			if (imagePath_.isEmpty()) {
				initialDirPath = MosPlatform::desktopPicturesFolderPath();
			} else {
				// This shouldn't normally happen since we set searchDirPath_
				// after successfully opening the image and it persists
				// between file/drop/paste operations...
				initialDirPath = QFileInfo{imagePath_}.absolutePath();
			}
		}

		selectedPath = QFileDialog::getOpenFileName(
			this,
			tr("Choose source image"),
			searchDirPath_,
			supportedImageFileFormats_
		);
	}

	if (selectedPath.isEmpty() && !hasImage()) {
		return;
	}

	QImage selectedImage{selectedPath};

	if (selectedImage.isNull()) {
		if (!selectedPath.isEmpty()) {
			MosUi::error(
				this, tr("Could not load %1.").arg(selectedPath));
		}

		// TODO
		//MosCurrentConfig().removeRecentFile(path_temp);

		return;
	}

	imagePath_ = selectedPath;

	// Persist the parent dir path as the search path for future file
	// operations
	searchDirPath_ = QFileInfo{selectedPath}.absolutePath();

	// We want to work on actual ARGB data
	originalImage_ = selectedImage.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	MosCurrentConfig().addRecentFile(imagePath_, originalImage_);
	updateRecentFilesMenu();
	updateWindowTitle(true, imagePath_);
	refreshPreviews();

	enableWorkArea(true);
}

void MainWindow::doReloadFile()
{
	QImage img{imagePath_};
	if (img.isNull()) {
		MosUi::error(this, tr("Could not reload %1.").arg(imagePath_));
		return;
	}

	originalImage_ = img.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	refreshPreviews();
}

void MainWindow::refreshPreviews(bool skipRerender)
{
	if (!hasImage() || signalsBlocked())
		return;

	if (!skipRerender) {
		switch (rcMode_)
		{
			case RcPaletteSwap:
			case RcColorRange: {
				const auto& keyPalette = currentPalette();
				ColorMap conversionMap;

				if (rcMode_ == RcPaletteSwap) {
					const auto& newPalette = currentPalette(true);
					conversionMap = generateColorMap(keyPalette, newPalette);
				} else {
					const auto& colorRange = colorRanges_.value(ui->listRanges->currentIndex().data(Qt::UserRole).toString());
					conversionMap = colorRange.applyToPalette(keyPalette);
				}

				transformedImage_ = recolorImage(originalImage_, conversionMap);

				break;
			}
			case RcColorBlend: {
				transformedImage_ = colorBlendImage(originalImage_, blendColor_, blendFactor_);

				break;
			}
			case RcColorShift: {
				transformedImage_ = colorShiftImage(originalImage_, colorShiftRed_, colorShiftGreen_, colorShiftBlue_);

				break;
			}
		}
	}

	switch (viewMode_)
	{
		case MosConfig::ImageViewSwipe:
		case MosConfig::ImageViewOnionSkin:
			ui->previewComposite->setImages(originalImage_, transformedImage_);
			resetPreviewLayout(ui->previewCompositeContainer, ui->previewComposite);

			ui->previewOriginal->clear();
			ui->previewRc->clear();
			break;

		default:
			ui->previewComposite->clear();

			ui->previewOriginal->setImage(originalImage_);
			ui->previewRc->setImage(transformedImage_);
			resetPreviewLayout(ui->previewOriginalContainer, ui->previewOriginal);
			resetPreviewLayout(ui->previewRcContainer, ui->previewRc);
	}
}

void MainWindow::resetPreviewLayout(QAbstractScrollArea* scrollArea,
									QWidget* previewWidget)
{
	if (!scrollArea || !previewWidget)
		return;

	const QSize& scaledSize = originalImage_.size() * zoom_;

	previewWidget->resize(scaledSize);
	previewWidget->parentWidget()->adjustSize();

	auto* hScroll = scrollArea->horizontalScrollBar();
	auto* vScroll = scrollArea->verticalScrollBar();

	hScroll->setValue(hScroll->maximum() / 2);
	vScroll->setValue(vScroll->maximum() / 2);
}

void MainWindow::doSaveFile()
{
	QString initialDirPath = saveDirPath_;

	if (initialDirPath.isEmpty())
		initialDirPath = !imagePath_.isEmpty()
						 ? QFileInfo{imagePath_}.absolutePath()
						 : MosPlatform::desktopPicturesFolderPath();

	QString base = QFileDialog::getExistingDirectory(
					   this,
					   tr("Choose an output directory"),
					   initialDirPath,
					   QFileDialog::ShowDirsOnly);

	if (base.isEmpty())
		return;

	QStringList succeeded;

	try {
		switch (rcMode_) {
			case RcColorBlend:
				succeeded = doSaveColorBlend(base);
				break;
			case RcColorShift:
				succeeded = doSaveColorShift(base);
				break;
			case RcPaletteSwap:
				succeeded = doSaveSingleRecolor(base);
				break;
			default:
				succeeded = doSaveColorRanges(base);
				break;
		}

		saveDirPath_ = base;

		MosUi::message(this, tr("The output files have been saved successfully."), succeeded);
	} catch (const canceled_job&) {
		;
	} catch (const QStringList& failed) {
		MosUi::error(this, tr("Some files could not be saved correctly."), failed);
	}
}

void MainWindow::doCloseFile()
{
	enableWorkArea(false);

	originalImage_ = transformedImage_ = QImage{};

	ui->previewOriginal->clear();
	ui->previewComposite->clear();
}

void MainWindow::doAboutDialog()
{
	MosUi::about(this);
}

void MainWindow::setViewMode(MainWindow::ViewMode newViewMode)
{
	if (viewMode_ == newViewMode)
		return;

	viewMode_ = newViewMode;

	switch (viewMode_)
	{
		case MosConfig::ImageViewSwipe:
		case MosConfig::ImageViewOnionSkin: {
			auto compositeDisplayMode = viewMode_ == MosConfig::ImageViewSwipe
										? CompositeImageLabel::CompositeDisplaySliding
										: CompositeImageLabel::CompositeDisplayOnionSkin;
			ui->staWorkAreaParent->setCurrentIndex(WorkAreaCompositeRc);
			ui->previewComposite->setDisplayMode(compositeDisplayMode);
			ui->compositeShortcutsPanel->setVisible(true);
			break;
		}
		default: {
			auto* newLayout = viewMode_ == MosConfig::ImageViewHSplit
							  ? static_cast<QLayout*>(new QHBoxLayout)
							  : static_cast<QLayout*>(new QVBoxLayout);
			auto* oldLayout = ui->pageWorkAreaSplit->layout();

			oldLayout->removeWidget(ui->previewOriginalContainer);
			oldLayout->removeWidget(ui->previewRcContainer);

			delete ui->pageWorkAreaSplit->layout();
			ui->pageWorkAreaSplit->setLayout(newLayout);

			newLayout->setContentsMargins(0, 0, 0, 0);
			newLayout->setSpacing(2);
			newLayout->addWidget(ui->previewOriginalContainer);
			newLayout->addWidget(ui->previewRcContainer);

			ui->staWorkAreaParent->setCurrentIndex(WorkAreaSplitRc);

			ui->compositeShortcutsPanel->setVisible(false);
			break;
		}
	}

	if (MosCurrentConfig().rememberImageViewMode()) {
		MosCurrentConfig().setImageViewMode(viewMode_);
	}

	// Ensure the combobox selection is correct if we came from the class ctor
	// (it will call us again but since the values are identical it's a no-op)
	ui->cbxViewMode->setCurrentIndex(viewMode_);

	// Update preview widgets if applicable
	refreshPreviews(true);
}

void MainWindow::setRcMode(MainWindow::RcMode newRcMode)
{
	if (rcMode_ == newRcMode)
		return;

	rcMode_ = newRcMode;

	switch (rcMode_)
	{
		case RcColorBlend:
			ui->groupRcKey->setVisible(false);
			ui->staFunctionOpts->setCurrentIndex(RcModeColorBlendPage);
			ui->cbxKeyPal->setEnabled(false);
			ui->lblKeyPal->setEnabled(false);
			break;
		case RcColorShift:
			ui->groupRcKey->setVisible(false);
			ui->staFunctionOpts->setCurrentIndex(RcModeColorShiftPage);
			ui->cbxKeyPal->setEnabled(false);
			ui->lblKeyPal->setEnabled(false);
			break;
		case RcPaletteSwap:
			ui->groupRcKey->setVisible(true);
			ui->staFunctionOpts->setCurrentIndex(RcModePaletteSwapPage);
			ui->cbxKeyPal->setEnabled(true);
			ui->lblKeyPal->setEnabled(true);
			ui->listRanges->setEnabled(false);
			ui->cbxNewPal->setEnabled(true);
			ui->lblNewPal->setEnabled(true);
			break;
		default:
			ui->groupRcKey->setVisible(true);
			ui->staFunctionOpts->setCurrentIndex(RcModeColorRangePage);
			ui->cbxKeyPal->setEnabled(true);
			ui->lblKeyPal->setEnabled(true);
			ui->listRanges->setEnabled(true);
			ui->cbxNewPal->setEnabled(false);
			ui->lblNewPal->setEnabled(false);
			break;
	}

	refreshPreviews();
}

void MainWindow::enableWorkArea(bool enable)
{
	if (!enable) {
		updateWindowTitle(false);
	}

	auto elements = std::make_tuple(
		ui->action_Reload,
		ui->action_Close,
		ui->action_Save,
		ui->actionCopy,
		ui->actionCopyOriginal,
		ui->actionBase64,
		ui->radPal, ui->radRc, ui->radBlend, ui->radShift,
		ui->lblKeyPal, ui->cbxKeyPal,
		ui->staFunctionOpts,
		ui->lblNewPal, ui->cbxNewPal,
		ui->listRanges,
		ui->zoomSlider,
		ui->menu_Zoom,
		ui->compositeShortcutsPanel,
		ui->cbxViewMode,
		ui->cmdGenerateWml,
		ui->buttonBox->button(QDialogButtonBox::Save));

	std::apply([enable](auto&&... widget) {
		(widget->setEnabled(enable), ...);
	}, elements);

	for (auto* action : viewModeActions_) {
		action->setEnabled(enable);
	}

	if (!enable) {
		ui->staWorkAreaParent->setCurrentIndex(WorkAreaStartPage);
	} else switch (viewMode_) {
		case MosConfig::ImageViewSwipe:
		case MosConfig::ImageViewOnionSkin:
			ui->staWorkAreaParent->setCurrentIndex(WorkAreaCompositeRc);
			break;
		default:
			ui->staWorkAreaParent->setCurrentIndex(WorkAreaSplitRc);
			break;
	}

	auto* closeButton = ui->buttonBox->button(QDialogButtonBox::Close);

	if (enable) {
		closeButton->setText(tr("Close"));
		closeButton->setWhatsThis(tr("Closes the current image."));
	} else {
		closeButton->setText(tr("Quit"));
		closeButton->setWhatsThis(tr("Quits Wespal."));
	}
}

QString MainWindow::currentPaletteName(bool paletteSwitchMode) const
{
	auto* combow = paletteSwitchMode
				   ? ui->cbxNewPal
				   : ui->cbxKeyPal;

	const int choice = combow->currentIndex();
	const auto& palette_name = combow->itemData(choice).toString();

	Q_ASSERT(!palette_name.isEmpty());

	return palette_name;
}

ColorList MainWindow::currentPalette(bool paletteSwitchMode) const
{
	return palettes_.value(currentPaletteName(paletteSwitchMode));
}

bool MainWindow::confirmFileOverwrite(const QStringList& paths)
{
	return MosUi::prompt(this, tr("The chosen directory already contains files with the same names required for output. Do you wish to overwrite them and continue?"), paths);
}

QStringList MainWindow::doSaveCurrentTransform(const QString& dirPath, const QString& suffix)
{
	const auto& nameStem = QFileInfo(imagePath_).completeBaseName();

	QString fileName = nameStem % '-' % suffix % ".png";
	QString filePath = dirPath % '/' % fileName;

	if (QFileInfo::exists(filePath) && !confirmFileOverwrite({filePath})) {
		throw canceled_job();
	}

	ScopedCursor sc{*this, {Qt::WaitCursor}};

	setEnabled(false);

	if (!MosIO::writePng(transformedImage_, filePath, MosCurrentConfig().pngVanityPlate())) {
		throw QStringList{fileName};
	}

	setEnabled(true);

	return QStringList{fileName};
}

QStringList MainWindow::doSaveColorRanges(const QString &base)
{
	QMap<QString, ColorMap> jobs;

	const auto& palId = currentPaletteName();
	const auto& palData = currentPalette();

	QStringList needOverwriteFiles;

	for (int k = 0; k < ui->listRanges->count(); ++k)
	{
		QListWidgetItem* itemw = ui->listRanges->item(k);
		Q_ASSERT(itemw);

		if (itemw->checkState() == Qt::Checked) {
			const QString& rangeId = itemw->data(Qt::UserRole).toString();

			const QString& filePath = base + "/" + QFileInfo(imagePath_).completeBaseName() +
					"-RC-" + palId + "-" + QString::number(k + 1) +
					"-" + rangeId + ".png";

			const auto& colorRange = colorRanges_.value(rangeId);
			jobs[filePath] = colorRange.applyToPalette(palData);

			if (QFileInfo::exists(filePath)) {
				needOverwriteFiles.push_back(cleanFileName(filePath));
			}
		}
	}

	if (!needOverwriteFiles.isEmpty() && !confirmFileOverwrite(needOverwriteFiles)) {
		throw canceled_job();
	}

	return doRunJobs(jobs);
}

QStringList MainWindow::doSaveSingleRecolor(const QString& dirPath)
{
	const auto& palId = currentPaletteName();
	const auto& targetPalId = currentPaletteName(true);

	QString suffix = QString{"PAL-%1-%2"}
					 .arg(palId, targetPalId);

	return doSaveCurrentTransform(dirPath, suffix);
}

QStringList MainWindow::doSaveColorBlend(const QString& dirPath)
{
	auto colorName = blendColor_.name();
	colorName.remove(0, 1);

	QString suffix = QString{"BLEND-%1-%2"}
					 .arg(colorName)
					 .arg(blendFactor_ * 100);

	return doSaveCurrentTransform(dirPath, suffix);
}

QStringList MainWindow::doSaveColorShift(const QString& dirPath)
{
	QString suffix = QString{"CS-%1-%2-%3"}
					   .arg(colorShiftRed_)
					   .arg(colorShiftGreen_)
					   .arg(colorShiftBlue_);

	return doSaveCurrentTransform(dirPath, suffix);
}

QStringList MainWindow::doRunJobs(const QMap<QString, ColorMap>& jobs)
{
	QStringList failed, succeeded;

	ScopedCursor sc{*this, {Qt::WaitCursor}};

	setEnabled(false);

	for (const auto& [fileName, colorMap] : jobs.asKeyValueRange())
	{
		const auto& plainName = cleanFileName(fileName);
		auto rc = recolorImage(originalImage_, colorMap);

		if (MosIO::writePng(rc, fileName, MosCurrentConfig().pngVanityPlate())) {
			succeeded.push_back(plainName);
		} else {
			failed.push_back(plainName);
		}
	}

	setEnabled(true);

	if (failed.isEmpty() != true) {
		throw failed;
	}

	return succeeded;
}

void MainWindow::on_action_Save_triggered()
{
	doSaveFile();
}

void MainWindow::on_cbxKeyPal_currentIndexChanged(int /*index*/)
{
	refreshPreviews();
}

void MainWindow::on_cbxNewPal_currentIndexChanged(int /*index*/)
{
	refreshPreviews();
}

void MainWindow::on_listRanges_currentRowChanged(int /*currentRow*/)
{
	refreshPreviews();
}

void MainWindow::on_zoomSlider_valueChanged(int value)
{
	qreal newZoom = zoomFactors_[qBound(0, value, int(zoomFactors_.size() - 1))];
	if (zoom_ == newZoom)
		return;

	zoom_ = newZoom;
	refreshPreviews();

	for (auto* action : zoomActions_)
	{
		if (action->data() == value && !action->isChecked()) {
			action->setChecked(true);
		}
	}
}

void MainWindow::on_zoomSlider_sliderMoved(int value)
{
	qreal newZoom = zoomFactors_[qBound(0, value, int(zoomFactors_.size() - 1))];

	MosUi::displaySliderTextToolTip(ui->zoomSlider, value, QString{"%1%"}.arg(qRound(newZoom * 100.0)));
}

void MainWindow::adjustZoom(ZoomDirection direction)
{
	switch (direction)
	{
		case ZoomOut:
			ui->zoomSlider->setValue(ui->zoomSlider->value() - 1);
			break;
		case ZoomIn:
			ui->zoomSlider->setValue(ui->zoomSlider->value() + 1);
	}
}

void MainWindow::handlePreviewBgOption(bool checked)
{
	if (!checked)
		return;

	QAction* const act = qobject_cast<QAction*>(sender());

	if (!act)
		return;

	if (act == ui->actionPreviewBgCustom) {
		// We want to give the user the option to customize the preview color
		// first.
		doCustomPreviewBgSelect();
	}

	setPreviewBackgroundColor(act->data().toString());
}

void MainWindow::doCustomPreviewBgSelect()
{
	auto* act = ui->actionPreviewBgCustom;
	QColor userColor = QColorDialog::getColor(QColor(act->data().toString()), this);
	if (userColor.isValid()) {
		act->setData(userColor.name());
		updateCustomPreviewBgIcon();
	}
}

void MainWindow::updateCustomPreviewBgIcon()
{
	auto* act = ui->actionPreviewBgCustom;
	act->setIcon(createColorIcon({act->data().toString()}, this));
}

void MainWindow::setPreviewBackgroundColor(const QString& colorName)
{
	if (!colorName.isEmpty()) {
		const QString ss = "* { background-color: " % colorName % "; }";
		ui->previewOriginalContainer->viewport()->setStyleSheet(ss);
		ui->previewRcContainer->viewport()->setStyleSheet(ss);
		ui->previewCompositeContainer->viewport()->setStyleSheet(ss);
	} else {
		ui->previewOriginalContainer->viewport()->setStyleSheet({});
		ui->previewRcContainer->viewport()->setStyleSheet({});
		ui->previewCompositeContainer->viewport()->setStyleSheet({});
	}

	MosCurrentConfig().setPreviewBackgroundColor(colorName);
}

void MainWindow::updateColorButton(QAbstractButton* button, const QColor& color)
{
	if (!button)
		return;

	static const QSize colorIconSize(48, 16);

	button->setIconSize(colorIconSize);
	button->setIcon(createColorIcon(color, colorIconSize, button));
}

void MainWindow::on_cmdOpen_clicked()
{
	openFile();
}

void MainWindow::on_action_ClearMru_triggered()
{
	MosCurrentConfig().clearRecentFiles();
	updateRecentFilesMenu();
}

void MainWindow::on_action_Close_triggered()
{
	doCloseFile();
}

void MainWindow::on_cbxViewMode_currentIndexChanged(int index)
{
	auto newMode = ViewMode(index);
	if (newMode == viewMode_)
		return;

	setViewMode(newMode);

	for (auto* action : viewModeActions_)
	{
		if (action->data() == newMode && !action->isChecked()) {
			action->setChecked(true);
		}
	}
}

void MainWindow::on_viewSlider_valueChanged(int value)
{
	ui->previewComposite->setDisplayRatio(qreal(value) / qreal(ui->viewSlider->maximum()));

	// We handle the toggles' checked property ourselves rather than using a
	// group because otherwise we can't uncheck them when the slider is not on
	// one of the bound values.

	if (value == ui->viewSlider->maximum()) {
		compositeShortcutsGroup_->setExclusive(true);
		ui->compositeRcOnlyToggle->setChecked(true);
	} else if (value == ui->viewSlider->minimum()) {
		compositeShortcutsGroup_->setExclusive(true);
		ui->compositeOriginalOnlyToggle->setChecked(true);
	} else {
		compositeShortcutsGroup_->setExclusive(false);
		ui->compositeRcOnlyToggle->setChecked(false);
		ui->compositeOriginalOnlyToggle->setChecked(false);
	}
}

void MainWindow::on_viewSlider_sliderMoved(int value)
{
	MosUi::displaySliderTextToolTip(ui->viewSlider, value);
}

void MainWindow::on_actionZoomIn_triggered()
{
	adjustZoom(ZoomIn);
}

void MainWindow::on_actionZoomOut_triggered()
{
	adjustZoom(ZoomOut);
}

void MainWindow::on_actionAppSettings_triggered()
{
	SettingsDialog dlg{
		QList<qreal>(zoomFactors_.cbegin(), zoomFactors_.cend()),
		originalImage_,
		this
	};

	dlg.exec();

	if (dlg.result() == QDialog::Rejected)
		return;

	userColorRanges_ = MosCurrentConfig().customColorRanges();
	userPalettes_ = MosCurrentConfig().customPalettes();

	{
		ObjectLock l{this};
		generateMergedRcDefinitions();
		processRcDefinitions();
	}

	refreshPreviews();
}


void MainWindow::on_compositeOriginalOnlyToggle_toggled(bool checked)
{
	if (checked) {
		ui->viewSlider->setValue(ui->viewSlider->minimum());
	}
}

void MainWindow::on_compositeRcOnlyToggle_toggled(bool checked)
{
	if (checked) {
		ui->viewSlider->setValue(ui->viewSlider->maximum());
	}
}

void MainWindow::on_actionBase64_triggered()
{
	if (originalImage_.isNull())
		return;

	const auto& ogBase64 = MosIO::writeBase64Png(originalImage_, true);
	const auto& rcBase64 = MosIO::writeBase64Png(transformedImage_, true);

	CodeSnippetDialog dlg{this};

	dlg.setAllowWmlSave(false);
	dlg.setRawDataMode(true);
	dlg.setWindowTitle(tr("Generate Base64"));

	dlg.addSnippet(tr("Recolored Image"), rcBase64);
	dlg.addSnippet(tr("Original Image"), ogBase64);

	dlg.exec();
}

void MainWindow::on_cmdGenerateWml_clicked()
{
	QString str;
	QTextStream out{&str};

	const auto& baseName = cleanFileName(imagePath_);
	const auto& paletteName = currentPaletteName();

	switch (rcMode_)
	{
		case RcColorRange:
			for (int k = 0; k < ui->listRanges->count(); ++k)
			{
				auto* listItem = ui->listRanges->item(k);

				if (listItem->checkState() == Qt::Unchecked)
					continue;

				if (!str.isEmpty())
					out << '\n';

				const auto& rangeName = listItem->data(Qt::UserRole).toString();

				out << baseName << "~RC(" << paletteName << '>' << rangeName << ')';
			}
			break;
		case RcPaletteSwap: {
			const auto& targetPaletteName = currentPaletteName(true);
			out << baseName << "~PAL(" << paletteName << '>' << targetPaletteName << ')';
			break;
		}
		case RcColorBlend:
			out << baseName << "~BLEND("
				<< blendColor_.red() << ','
				<< blendColor_.green() << ','
				<< blendColor_.blue() << ','
				<< blendFactor_ << ')';
			break;
		case RcColorShift:
			out << baseName << "~CS("
				<< colorShiftRed_ << ','
				<< colorShiftGreen_ << ','
				<< colorShiftBlue_ << ')';
			break;
	}

	CodeSnippetDialog dlg{str, this};

	dlg.setAllowWmlSave(false);
	dlg.setRawDataMode(true);
	dlg.setWindowTitle(tr("Generate WML"));

	dlg.exec();
}

void MainWindow::onColorBlendFactorChanged(int value)
{
	blendFactor_ = qreal(qBound(0, value, 100)) / 100.0;

	refreshPreviews();
}

void MainWindow::onColorBlendLineEditChanged(const QString& value)
{
	QColor color{value};

	if (!color.isValid())
		return;

	updateColorButton(ui->tbBlendColor, color);

	blendColor_ = color;

	refreshPreviews();
}

void MainWindow::onColorBlendButtonClicked()
{
	QColor color{blendColor_};

	color = QColorDialog::getColor(blendColor_, this);

	if (color.isValid()) {
		ui->leBlendColor->setText(color.name());
	}
}

void MainWindow::onColorShiftValueChanged(MainWindow::ColorShiftChannel ch, int value)
{
	switch (ch)
	{
		case ColorShiftRed:
			colorShiftRed_ = qBound(-255, value, 255);
			break;
		case ColorShiftGreen:
			colorShiftGreen_ = qBound(-255, value, 255);
			break;
		case ColorShiftBlue:
			colorShiftBlue_ = qBound(-255, value, 255);
			break;
	}

	refreshPreviews();
}

void MainWindow::onRcSelectButtonClicked(bool check)
{
	for (int i = 0; i < ui->listRanges->count(); ++i) {
		ui->listRanges->item(i)->setCheckState(check || ui->listRanges->item(i)->isSelected() ? Qt::Checked : Qt::Unchecked);
	}
}

void MainWindow::on_actionCopy_triggered()
{
	auto* clipboard = QGuiApplication::clipboard();

	if (!clipboard || transformedImage_.isNull())
		return;

	clipboard->setImage(transformedImage_);
}

void MainWindow::on_actionCopyOriginal_triggered()
{
	auto* clipboard = QGuiApplication::clipboard();

	if (!clipboard || originalImage_.isNull())
		return;

	clipboard->setImage(originalImage_);
}

void MainWindow::on_actionPaste_triggered()
{
	auto* clipboard = QGuiApplication::clipboard();

	if (!clipboard || clipboard->image().isNull())
		return;

	// Normalize image format from unknown source
	originalImage_ = clipboard->image().convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	imagePath_ = tr("Clipboard image") % ".png";
	updateWindowTitle(true, {}, ImageOriginClipboard);

	refreshPreviews();
	enableWorkArea(true);
}

void MainWindow::onClipboardChanged(QClipboard::Mode mode)
{
	if (mode != QClipboard::Clipboard)
		return;

	auto* clipboard = QGuiApplication::clipboard();

	ui->actionPaste->setEnabled(clipboard && !clipboard->image().isNull());
}
