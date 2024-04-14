/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2011 - 2024 by Iris Morelle <iris@irydacea.me>
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

#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

#include "appconfig.hpp"
#include "codesnippetdialog.hpp"
#include "colorlistinputdialog.hpp"
#include "defs.hpp"
#include "paletteitem.hpp"
#include "util.hpp"

#include <QCloseEvent>
#include <QColorDialog>
#include <QMessageBox>
#include <QMenu>

SettingsDialog::SettingsDialog(const QList<qreal>& zoomValues,
							   const QImage& referenceImage,
							   QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::SettingsDialog)
	, zoomValues_(zoomValues)
	, palettes_(MosCurrentConfig().customPalettes())
	, ranges_(MosCurrentConfig().customColorRanges())
	, referenceImage_(referenceImage)
	, paletteRecolorMenu_(nullptr)
	, paletteRecolorActions_()
{
	//
	// Window initialisation
	//

	ui->setupUi(this);

	// Set theme icons

	auto listAddIcon = QIcon::fromTheme("list-add", QIcon{":/list-add-16.png"});
	ui->colorRangeAdd->setIcon(listAddIcon);
	ui->paletteAdd->setIcon(listAddIcon);

	auto listRemoveIcon = QIcon::fromTheme("list-remove", QIcon{":/list-remove-16.png"});
	ui->colorRangeDel->setIcon(listRemoveIcon);
	ui->paletteDel->setIcon(listRemoveIcon);

	auto editCopyIcon = QIcon::fromTheme("edit-copy", QIcon{":/edit-copy-16.png"});
	ui->actionDuplicateColorRange->setIcon(editCopyIcon);
	ui->actionDuplicatePalette->setIcon(editCopyIcon);

	auto editRenameIcon = QIcon::fromTheme("edit-rename");
	ui->actionRenameColorRange->setIcon(editRenameIcon);
	ui->actionRenamePalette->setIcon(editRenameIcon);

	// Both initColorRangesPage() and initPalettesPage() touch this. It's also
	// needed later on during event handling.
	paletteRecolorMenu_ = new QMenu(this);

	connect(this, SIGNAL(accepted()), this, SLOT(onDialogAccepted()));

	//
	// Page-specific initialisation
	//

	initGeneralPage();
	// We set up the palettes page BEFORE color ranges so the palette recolor
	// menu is properly built.
	initPalettesPage();
	initColorRangesPage();

	ui->tabWidget->setCurrentIndex(0);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::changeEvent(QEvent* event)
{
	QDialog::changeEvent(event);

	switch (event->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void SettingsDialog::onDialogAccepted()
{
	auto& config = MosCurrentConfig();

	qreal defaultZoom = ui->defaultZoomList->currentData().toReal();

	config.setRememberMainWindowSize(ui->rememberWindowSizeCheckbox->isChecked());
	config.setRememberImageViewMode(ui->rememberImageViewModeCheckbox->isChecked());
	config.setDefaultZoom(defaultZoom);
	config.setCustomColorRanges(ranges_);
	config.setCustomPalettes(palettes_);
}

//
// Utility methods
//

void SettingsDialog::showCodeSnippet(const QString& title, const QString& text)
{
	CodeSnippetDialog dlg{text, this};

	dlg.setWindowTitle(title);
	dlg.exec();
}

QString SettingsDialog::itemData(const QListWidgetItem* item)
{
	Q_ASSERT(item);
	return item ? item->data(Qt::UserRole).toString() : QString{};
}

void SettingsDialog::updateColorButton(QAbstractButton* button, const QColor& color)
{
	if (!button)
		return;

	static const QSize colorIconSize(48, 16);

	button->setIconSize(colorIconSize);
	button->setIcon(createColorIcon(color, colorIconSize, button));
}

void SettingsDialog::setupSingleColorEditSlots(QLineEdit* lineEdit,
											   QAbstractButton* button,
											   SettingsDialog::ColorReceiverFunction func)
{
	// LineEdit changed
	connect(lineEdit, &QLineEdit::textChanged, this, [this, lineEdit, button, func]() {
		QColor color{lineEdit->text()};

		if (!color.isValid())
			return;

		updateColorButton(button, color);

		func(color);
	});

	// Button clicked
	connect(button, &QAbstractButton::clicked, this, [this, lineEdit]() {
		QColor color{lineEdit->text()};

		color = QColorDialog::getColor(color, this);

		if (color.isValid()) {
			lineEdit->setText(color.name());
		}
	});
}

template<>
SettingsDialog::ColorRangeMap& SettingsDialog::artifactCollection()
{
	return ranges_;
}

template<>
SettingsDialog::ColorListMap& SettingsDialog::artifactCollection()
{
	return palettes_;
}

template<>
QListWidget* SettingsDialog::artifactSelector<ColorRange>()
{
	return ui->colorRangeList;
}

template<>
QListWidget* SettingsDialog::artifactSelector<ColorList>()
{
	return ui->paletteList;
}

template<typename ArtifactT>
ArtifactT& SettingsDialog::currentArtifact()
{
	auto* itemWidget = artifactSelector<ArtifactT>()->currentItem();

	if (!itemWidget) {
		static ArtifactT fallback;
		return fallback;
	}

	const auto& name = itemData(itemWidget);

	auto& collection = artifactCollection<ArtifactT>();
	auto it = collection.find(name);

	if (it == collection.end())
		it = collection.insert(name, {});

	return it.value();
}

template<typename ArtifactT>
void SettingsDialog::artifactItemRenameInteractive(QListWidgetItem* listItem)
{
	auto* selector = artifactSelector<ArtifactT>();

	Q_ASSERT(listItem && selector);

	if (!listItem || !selector)
		return;

	const auto& newName = listItem->text();
	const auto& oldName = itemData(listItem);

	if (newName == oldName)
		return;

	auto& collection = artifactCollection<ArtifactT>();

	auto oldIt = collection.find(oldName);
	auto newIt = collection.find(newName);

	Q_ASSERT(oldIt != collection.end());
	Q_ASSERT(oldIt != newIt);

	if (oldIt == collection.end())
		return;

	if (newIt != collection.end()) {
		QString message = tr("The item \"%1\" already exists. Do you wish to overwrite it?").arg(newName);
		if (!MosUi::prompt(this, message)) {
			listItem->setText(oldName);
			return;
		}

		for (int i = 0; i < selector->count(); ++i)
		{
			auto* lwi = selector->item(i);

			if (itemData(lwi) == newName) {
				delete selector->takeItem(i);
				break;
			}
		}
	} else {
		newIt = collection.insert(newName, {});
	}

	newIt.value() = std::move(oldIt.value());
	collection.erase(oldIt);

	listItem->setData(Qt::UserRole, newName);
}

//
// General page
//

void SettingsDialog::initGeneralPage()
{
	auto& config = MosCurrentConfig();

	ui->rememberWindowSizeCheckbox->setChecked(config.rememberMainWindowSize());
	ui->rememberImageViewModeCheckbox->setChecked(config.rememberImageViewMode());

	auto defaultZoom = config.defaultZoom();

	QString zoomLabelFmt{"%1%"};

	for (auto zoom : zoomValues_)
	{
		ui->defaultZoomList->addItem(zoomLabelFmt.arg(qRound(zoom * 100)), zoom);

		if (defaultZoom == zoom) {
			ui->defaultZoomList->setCurrentIndex(ui->defaultZoomList->count() - 1);
		}
	}
}

//
// Color ranges page
//

void SettingsDialog::initColorRangesPage()
{
	auto* colorRangeMenu = new QMenu(this);

	colorRangeMenu->addAction(ui->actionDuplicateColorRange);
	colorRangeMenu->addAction(ui->actionRenameColorRange);

	ui->colorRangeMenuButton->setMenu(colorRangeMenu);

	// This is a copy of the range names. The loop below MAY modify the list
	// of ranges, so we cannot use iterators that may become invalid.
	const auto rangeNames = ranges_.keys();

	for (const auto& name : rangeNames)
	{
		addRangeListEntry(name);
	}

	if (!ranges_.empty())
		ui->colorRangeList->setCurrentRow(0);

	updateRangeEditControls();

	// TODO: Map marker color support
	ui->repColorLabel->setVisible(false);
	ui->repColorEdit->setVisible(false);
	ui->repColorPick->setVisible(false);

	// Set up slots

	connect(ui->colorRangeList, SIGNAL(currentRowChanged(int)), this, SLOT(onColorRangeRowChanged(int)));
	connect(ui->colorRangeList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onColorRangeItemChanged(QListWidgetItem*)));

	connect(ui->colorRangeAdd, SIGNAL(clicked()), this, SLOT(onColorRangeAdd()));
	connect(ui->colorRangeDel, SIGNAL(clicked()), this, SLOT(onColorRangeDelete()));

	connect(ui->actionRenameColorRange, SIGNAL(triggered()), this, SLOT(onColorRangeRename()));
	connect(ui->actionDuplicateColorRange, SIGNAL(triggered()), this, SLOT(onColorRangeDuplicate()));

	connect(ui->colorRangeFromList, SIGNAL(clicked()), this, SLOT(onColorRangeFromList()));
	connect(ui->colorRangeWml, SIGNAL(clicked()), this, SLOT(onColorRangeToWml()));

	setupSingleColorEditSlots(ui->avgColorEdit, ui->avgColorPick, [&](const QColor& color) {
		currentArtifact<ColorRange>().setMid(color.rgb());
	});

	setupSingleColorEditSlots(ui->minColorEdit, ui->minColorPick, [&](const QColor& color) {
		currentArtifact<ColorRange>().setMin(color.rgb());
	});

	setupSingleColorEditSlots(ui->maxColorEdit, ui->maxColorPick, [&](const QColor& color) {
		currentArtifact<ColorRange>().setMax(color.rgb());
	});

	// TODO: Map marker color support
#if 0
	setupSingleColorEditSlots(ui->repColorEdit, ui->repColorPick, [&](const QColor& color) {
		currentArtifact<ColorRange>().setRep(color.rgb());
	});
#endif
}

void SettingsDialog::updateRangeEditControls()
{
	const bool haveRanges = ui->colorRangeList->count() > 0;

	ui->colorRangeEditor->setEnabled(haveRanges);
	ui->colorRangeDel->setEnabled(haveRanges);
	ui->colorRangeMenuButton->setEnabled(haveRanges);

	// Do not send textChanged signals, since that would alter
	// the ranges_ map (adding a range with an empty key in the
	// noMoreRanges == true case) and either cause problems or
	// waste everyone's time doing color parsing. We'll update
	// the color picker buttons directly instead.
	const ObjectLock lock{this};

	if (haveRanges) {
		auto& range = currentArtifact<ColorRange>();

		const QColor avg = range.mid();
		ui->avgColorEdit->setText(avg.name());
		updateColorButton(ui->avgColorPick, avg);

		const QColor min = range.min();
		ui->minColorEdit->setText(min.name());
		updateColorButton(ui->minColorPick, min);

		const QColor max = range.max();
		ui->maxColorEdit->setText(max.name());
		updateColorButton(ui->maxColorPick, max);

		// TODO: Map marker color support
#if 0
		const QColor rep = range.rep();
		ui->repColorEdit->setText(rep.name());
		updateColorButton(ui->repColorPick, rep);
#endif
	} else {
		const QColor& blank = palette().color(QPalette::Button);

		ui->avgColorEdit->clear();
		updateColorButton(ui->avgColorPick, blank);

		ui->minColorEdit->clear();
		updateColorButton(ui->minColorPick, blank);

		ui->maxColorEdit->clear();
		updateColorButton(ui->maxColorPick, blank);

		// TODO: Map marker color support
#if 0
		ui->repColorEdit->clear();
		updateColorButton(ui->repColorPick, blank);
#endif
	}
}

void SettingsDialog::addRangeListEntry(const QString& name)
{
	auto* listItem = new QListWidgetItem(name, ui->colorRangeList);

	const auto& colorRange = ranges_.value(name);

	listItem->setData(Qt::UserRole, name);
	listItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	// TODO: Map marker color support
	listItem->setIcon(createColorIcon(colorRange.mid(), ui->colorRangeList));

	Q_ASSERT(ranges_.contains(name));

	// Take care of custom ranges for initPalettesPage().
	addPaletteRecolorMenuEntry(name, colorRange, capitalize(name));
}

void SettingsDialog::onColorRangeRowChanged(int /*row*/)
{
	updateRangeEditControls();
}

void SettingsDialog::onColorRangeItemChanged(QListWidgetItem* item)
{
	if (!item)
		return;

	const auto& newName = item->text();
	const auto& oldName = itemData(item);

	const auto colorRange = ranges_.value(oldName);

	artifactItemRenameInteractive<ColorRange>(item);

	deletePaletteRecolorMenuEntry(oldName);
	addPaletteRecolorMenuEntry(newName, colorRange, capitalize(newName));
}

void SettingsDialog::onColorRangeAdd()
{
	const auto& name = generateColorRangeName();
	ranges_.insert(name, {});
	addRangeListEntry(name);

	ui->colorRangeList->setCurrentRow(ui->colorRangeList->count() - 1);
	ui->colorRangeList->editItem(ui->colorRangeList->currentItem());
}

void SettingsDialog::onColorRangeDelete()
{
	auto remaining = ui->colorRangeList->count();

	if (remaining == 0)
		return;

	// If there are at least two remaining items (including the one that's
	// about to be deleted), it's safe to allow some signals to go through.
	// Otherwise, we need to disable them.

	ConditionalObjectLock lockList{ui->colorRangeList, remaining == 1};

	auto* listItem = ui->colorRangeList->takeItem(ui->colorRangeList->currentRow());
	Q_ASSERT(listItem);
	const auto& name = itemData(listItem);
	delete listItem;

	if (remaining == 1) {
		// No more ranges!
		updateRangeEditControls();
	}

	if (auto it = ranges_.find(name); it != ranges_.end()) {
		ranges_.erase(it);
	}

	deletePaletteRecolorMenuEntry(name);
}

void SettingsDialog::onColorRangeRename()
{
	// The rest is taken care of by the onColorRangeItemChanged slot.
	ui->colorRangeList->editItem(ui->colorRangeList->currentItem());
}

void SettingsDialog::onColorRangeDuplicate()
{
	auto* listItem = ui->colorRangeList->currentItem();

	if (!listItem)
		return;

	const auto& name = generateColorRangeName(itemData(listItem));
	ranges_.insert(name, currentArtifact<ColorRange>());
	addRangeListEntry(name);

	ui->colorRangeList->setCurrentRow(ui->colorRangeList->count() - 1);
	ui->colorRangeList->editItem(ui->colorRangeList->currentItem());
}

void SettingsDialog::onColorRangeFromList()
{
	auto* listItem = ui->colorRangeList->currentItem();

	if (!listItem)
		return;

	ColorListInputDialog dlg{this};
	dlg.exec();

	const auto& colors = dlg.getColorList();

	if (!colors.empty()) {
		auto& current = currentArtifact<ColorRange>();

		if (colors.size() >= 1) {
			current.setMid(colors[0]);
			listItem->setIcon(createColorIcon(colors[0], ui->colorRangeList));
		}
		if (colors.size() >= 2) {
			current.setMax(colors[1]);
		}
		if (colors.size() >= 3) {
			current.setMin(colors[2]);
		}
		// TODO: Map marker color support
#if 0
		if (colors.size() >= 4) {
			current.setRep(colors[3]);
		}
#endif

		updateRangeEditControls();
	}
}

void SettingsDialog::onColorRangeToWml()
{
	auto* listItem = ui->colorRangeList->currentItem();

	if (!listItem)
		return;

	const auto& rangeName = itemData(listItem);
	const auto& range = ranges_.value(rangeName);
	const auto& wml = wmlFromColorRange(rangeName, range);

	showCodeSnippet(tr("Color Range WML"),
					wmlFromColorRange(rangeName, range));
}

//
// Color palettes page
//

void SettingsDialog::initPalettesPage()
{

	ui->paletteColorsList->setItemDelegate(new PaletteItemDelegate(ui->paletteList));

	auto* paletteMenu = new QMenu(this);

	paletteMenu->addAction(ui->actionDuplicatePalette);
	paletteMenu->addAction(ui->actionRenamePalette);

	ui->paletteMenuButton->setMenu(paletteMenu);

	// initColorRangesPage() initialises the recolor menu for us.
	Q_ASSERT(paletteRecolorMenu_);

	// We want to split the ranges menu in two with built-in ranges first,
	// custom ranges at the end.

	for (qsizetype k = 0; k < wesnoth::builtinColorRanges.objectCount(); ++k)
	{
		const auto& id = wesnoth::builtinColorRanges.orderedNames()[k];
		const auto& uiName = wesnoth::builtinColorRanges.orderedTranslatableNames()[k];
		addPaletteRecolorMenuEntry(id, wesnoth::builtinColorRanges[id], uiName);
	}

	paletteRecolorMenu_->addSeparator();

	// Custom and overridden color ranges are taken care of by initColorRangesPage().

	ui->paletteRecolor->setMenu(paletteRecolorMenu_);

	for (auto i = palettes_.constBegin(); i != palettes_.constEnd(); ++i)
	{
		addPaletteListEntry(i.key());
	}

	// Set up slots

	connect(ui->paletteList, SIGNAL(currentRowChanged(int)), this, SLOT(onPaletteRowChanged(int)));
	connect(ui->paletteList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onPaletteItemChanged(QListWidgetItem*)));

	connect(ui->paletteAdd, SIGNAL(clicked()), this, SLOT(onPaletteAdd()));
	connect(ui->paletteDel, SIGNAL(clicked()), this, SLOT(onPaletteDelete()));

	connect(ui->actionRenamePalette, SIGNAL(triggered()), this, SLOT(onPaletteRename()));
	connect(ui->actionDuplicatePalette, SIGNAL(triggered()), this, SLOT(onPaletteDuplicate()));
	// onPaletteRecolor is connected via addPaletteRecolorMenuEntry().

	connect(ui->paletteColorsFromList, SIGNAL(clicked()), this, SLOT(onPaletteFromList()));
	connect(ui->paletteColorsFromImage, SIGNAL(clicked()), this, SLOT(onPaletteFromImage()));
	connect(ui->paletteWml, SIGNAL(clicked()), this, SLOT(onPaletteToWml()));

	connect(ui->paletteColorsList, SIGNAL(currentRowChanged(int)), this, SLOT(onColorRowChanged(int)));
	connect(ui->paletteColorsList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onColorItemChanged(QListWidgetItem*)));

	connect(ui->paletteColorAdd, SIGNAL(clicked()), this, SLOT(onColorAdd()));
	connect(ui->paletteColorDel, SIGNAL(clicked()), this, SLOT(onColorDelete()));

	setupSingleColorEditSlots(ui->paletteColorEdit, ui->paletteColorPick, [&](const QColor& color) {
		auto* listItem = ui->paletteColorsList->currentItem();

		if (!listItem)
			return;

		listItem->setData(Qt::UserRole, color.rgb());
	});

	// Finalise build

	if (palettes_.empty()) {
		setPaletteViewEnabled(false);
	} else {
		// Notify the palette view widget
		ui->paletteList->setCurrentRow(0);
	}
}

void SettingsDialog::addPaletteRecolorMenuEntry(const QString& id,
												const ColorRange& colorRange,
												const QString& label)
{
	if (!paletteRecolorActions_.contains(id)) {
		auto* action = paletteRecolorMenu_->addAction(label);

		action->setData(id);
		// TODO: Map marker color support
		action->setIcon(createColorIcon(colorRange.mid(), paletteRecolorMenu_));
		action->setIconVisibleInMenu(true);

		connect(action, SIGNAL(triggered()), this, SLOT(onPaletteRecolor()));

		paletteRecolorActions_.insert(id, action);
	} else {
		// Update an existing entry
		paletteRecolorActions_[id]->setIcon(createColorIcon(colorRange.mid(), paletteRecolorMenu_));
	}
}

void SettingsDialog::deletePaletteRecolorMenuEntry(const QString& name)
{
	if (!paletteRecolorActions_.contains(name))
		return;

	auto* recolorAction = paletteRecolorActions_[name];

	if (wesnoth::builtinColorRanges.hasName(name)) {
		// Revert color of affected recolor menu items to Wesnoth built-ins if
		// they are overrides of those.
		const auto& builtin = wesnoth::builtinColorRanges[name];
		recolorAction->setIcon(createColorIcon(builtin.mid(), paletteRecolorMenu_));
	} else {
		paletteRecolorMenu_->removeAction(recolorAction);
		paletteRecolorActions_.remove(name);
		delete recolorAction;
	}
}

void SettingsDialog::addPaletteListEntry(const QString& name)
{
	auto* listItem = new QListWidgetItem(name, ui->paletteList);

	listItem->setData(Qt::UserRole, name);
	listItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);

	Q_ASSERT(palettes_.contains(name));

	const auto& palette = palettes_.value(name);

	updatePaletteIcon(listItem);
}

void SettingsDialog::removePaletteListEntry(const QString& name)
{
	const auto& matching = ui->paletteList->findItems(name, Qt::MatchFixedString);

	Q_ASSERT(matching.size() <= 1);

	for (auto* item : matching)
	{
		delete ui->paletteList->takeItem(ui->paletteList->row(item));
	}
}

void SettingsDialog::renamePaletteListEntry(const QString& oldName,
											const QString& newName)
{
	removePaletteListEntry(oldName);
	addPaletteListEntry(newName);
}

void SettingsDialog::populatePaletteView(const ColorList& palette)
{
	{
		// Make sure to not emit signals while setting up rows.
		const ObjectLock lock{ui->paletteColorsList};

		ui->paletteColorsList->clear();

		for (auto rgb : palette) {
			auto* listItem = new QListWidgetItem({}, ui->paletteColorsList);
			listItem->setData(Qt::UserRole, rgb);
			listItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
		}
	}

	// The hex editor box needs to be notified now.
	ui->paletteColorsList->setCurrentRow(0);
}

void SettingsDialog::clearPaletteView()
{
	ui->paletteColorsList->clear();
	ui->paletteColorEdit->clear();

	setPaletteViewEnabled(false);
}

void SettingsDialog::setPaletteViewEnabled(bool enabled)
{
	ui->paletteDel->setEnabled(enabled);
	ui->paletteMenuButton->setEnabled(enabled);

	setPaletteEditControlsEnabled(enabled);
}

void SettingsDialog::setPaletteEditControlsEnabled(bool enabled)
{
	ui->paletteColorToolsPanel->setEnabled(enabled);
	ui->paletteColorsFromImage->setEnabled(enabled && !referenceImage_.isNull());

	const bool haveColors = ui->paletteColorsList->count() != 0;

	ui->paletteColorEditorPanel->setEnabled(enabled && haveColors);
	ui->paletteColorDel->setEnabled(enabled && haveColors);

	if (!haveColors) {
		ui->paletteColorEdit->clear();
	}
}

void SettingsDialog::updatePaletteIcon(QListWidgetItem* listItem)
{
	if (!listItem)
		listItem = ui->paletteList->currentItem();

	Q_ASSERT(listItem);

	const auto& palette = palettes_.value(itemData(listItem));
	const auto& icon = palette.empty()
					   ? createColorIcon(Qt::white, ui->paletteList)
					   : createColorIcon(palette.front(), ui->paletteList);

	listItem->setIcon(icon);
}

void SettingsDialog::appendToCurrentPalette(const ColorList& colors)
{
	if (colors.empty())
		return;

	auto& pal = currentArtifact<ColorList>();

	const bool firstColorChanged = pal.empty();

	pal.append(colors);

	// Force refresh the current palette colors view.
	populatePaletteView(pal);
	setPaletteEditControlsEnabled(true);

	if (firstColorChanged) {
		updatePaletteIcon();
	}
}

void SettingsDialog::onPaletteRowChanged(int row)
{
	auto* listItem = ui->paletteList->item(row);

	Q_ASSERT(listItem);
	if (!listItem)
		return;

	const auto& name = listItem->text();
	auto it = palettes_.find(name);

	if (it == palettes_.end()) {
		QMessageBox::critical(this,
							  tr("Wespal"),
							  tr("The palette \"%1\" does not exist.").arg(name));
		removePaletteListEntry(name);
		return;
	}

	populatePaletteView(it.value());

	setPaletteEditControlsEnabled(true);
}

void SettingsDialog::onPaletteItemChanged(QListWidgetItem* item)
{
	artifactItemRenameInteractive<ColorList>(item);
}

void SettingsDialog::onPaletteAdd()
{
	{
		ObjectLock lockPals{ui->paletteList};

		const QString& palName = generatePaletteName();
		palettes_[palName].append(qRgb(0, 0, 0));
		addPaletteListEntry(palName);
	}

	ui->paletteList->setCurrentRow(ui->paletteList->count() - 1);
	ui->paletteList->editItem(ui->paletteList->currentItem());
}

void SettingsDialog::onPaletteDelete()
{
	auto remaining = ui->paletteList->count();

	if (remaining == 0)
		return;

	// If there are at least two remaining items (including the one that's
	// about to be deleted), it's safe to allow some signals to go through.
	// Otherwise, we need to disable them.

	ConditionalObjectLock lockPals{ui->paletteList, remaining == 1};
	ConditionalObjectLock lockColors{ui->paletteColorsList, remaining == 1};

	auto* listItem = ui->paletteList->takeItem(ui->paletteList->currentRow());
	Q_ASSERT(listItem);
	const auto& name = itemData(listItem);
	delete listItem;

	if (remaining == 1) {
		// No more palettes!
		clearPaletteView();
	}

	if (auto it = palettes_.find(name); it != palettes_.end()) {
		palettes_.erase(it);
	}
}

void SettingsDialog::onPaletteRename()
{
	// The rest is taken care of by the onPaletteItemChanged slot.
	ui->paletteList->editItem(ui->paletteList->currentItem());
}

void SettingsDialog::onPaletteDuplicate()
{
	auto* listItem = ui->paletteList->currentItem();

	if (!listItem)
		return;

	const auto& oldName = itemData(listItem);

	{
		ObjectLock lockPals{ui->paletteList};

		const auto& name = generatePaletteName(oldName);
		palettes_[name] = currentArtifact<ColorList>();
		addPaletteListEntry(name);
	}

	ui->paletteList->setCurrentRow(ui->paletteList->count() - 1);
	setPaletteViewEnabled(true);
	ui->paletteList->editItem(ui->paletteList->currentItem());
}

void SettingsDialog::onPaletteRecolor()
{
	auto* action = qobject_cast<QAction*>(sender());

	if (!action)
		return;

	ColorRange colorRange;
	const auto& name = action->data().toString();

	// Prefer an overridden range before built-ins
	if (auto it = ranges_.find(name); it != ranges_.end()) {
		colorRange = it.value();
	} else if (wesnoth::builtinColorRanges.hasName(name)) {
		colorRange = wesnoth::builtinColorRanges[name];
	}

	auto* listItem = ui->paletteList->currentItem();

	if (!listItem)
		return;

	auto& pal = currentArtifact<ColorList>();

	const auto& cvtMap = colorRange.applyToPalette(pal);
	// The actual recoloring must be done manually here.
	for (auto& color : pal)
	{
		auto cvtIt = cvtMap.find(color);
		if (cvtIt != cvtMap.constEnd()) {
			color = cvtIt.value();
		}
	}

	// Force refresh the current palette colors view.
	populatePaletteView(pal);

	updatePaletteIcon();
}

void SettingsDialog::onPaletteFromList()
{
	ColorListInputDialog dlg{this};

	dlg.exec();

	if (dlg.result() == QDialog::Rejected)
		return;

	appendToCurrentPalette(dlg.getColorList());
}

void SettingsDialog::onPaletteFromImage()
{
	if (referenceImage_.isNull())
		return;

	const auto& colors = uniqueColorsFromImage(referenceImage_);

	appendToCurrentPalette(ColorList{colors.begin(), colors.end()});
}

void SettingsDialog::onPaletteToWml()
{
	auto* listItem = ui->paletteList->currentItem();

	if (!listItem)
		return;

	const auto& paletteName = itemData(listItem);
	const auto& pal = palettes_.value(paletteName);

	showCodeSnippet(tr("Color Palette WML"),
					wmlFromColorList(paletteName, pal));
}

void SettingsDialog::onColorRowChanged(int row)
{
	const QColor currentColor = ui->paletteColorsList->item(row)->data(Qt::UserRole).toInt();

	ui->paletteColorEdit->setText(currentColor.name());
}

void SettingsDialog::onColorItemChanged(QListWidgetItem* item)
{
	if (!item)
		return;

	const QColor currentColor = item->data(Qt::UserRole).toInt();

	// HACK: the following is a hack to prevent paletteColorEdit's changes
	// from being overwritten when it has triggered this itemChanged
	// signal in the first place, e.g. in the middle of the user
	// entering a hex code or color name in the field. Just one symptom
	// of terrible design that needs to be fixed later.

	const QColor textColor = ui->paletteColorEdit->text();

	if (textColor != currentColor)
		ui->paletteColorEdit->setText(currentColor.name());

	// END HACK

	// Update palette definition.

	auto& pal = currentArtifact<ColorList>();
	const auto index = ui->paletteColorsList->currentRow();

	Q_ASSERT(index < pal.size());
	if (index >= pal.size())
		return;

	pal[index] = currentColor.rgb();

	// If this is the first row we also update the palette icon.

	if (index == 0) {
		updatePaletteIcon();
	}
}

void SettingsDialog::onColorAdd()
{
	// Make sure to actually add the item only later, once
	// it is safe to notify other widgets!

	auto* newItem = new QListWidgetItem("");

	// We might already have a first row with the first
	// color of the palette. If we don't we use pure black.

	auto* firstItem = ui->paletteColorsList->item(0);

	const QRgb rgb = firstItem ? firstItem->data(Qt::UserRole).toInt() : qRgb(0,0,0);

	newItem->setData(Qt::UserRole, rgb);
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);

	// Update palette definition.

	auto& pal = currentArtifact<ColorList>();
	pal.push_back(rgb);

	// Notify widgets.

	ui->paletteColorsList->addItem(newItem);
	ui->paletteColorsList->setCurrentItem(newItem);

	setPaletteEditControlsEnabled(true);

	// If this is the first row we also update the palette icon.

	if (firstItem == nullptr) {
		updatePaletteIcon();
	}
}

void SettingsDialog::onColorDelete()
{
	auto remaining = ui->paletteColorsList->count();

	if (remaining == 0)
		return;

	// If there are at least two remaining items (including the one that's
	// about to be deleted), it's safe to allow some signals to go through.
	// Otherwise, we need to disable them.

	ConditionalObjectLock lock{ui->paletteColorsList, remaining == 1};

	const int index = ui->paletteColorsList->currentRow();

	delete ui->paletteColorsList->takeItem(ui->paletteColorsList->currentRow());

	if (remaining == 1) {
		// No more colors!
		setPaletteEditControlsEnabled(true);
	}

	// Update palette definition.

	auto& pal = currentArtifact<ColorList>();

	Q_ASSERT(index < pal.count());
	pal.removeAt(index);

	updatePaletteIcon();
}
