#ifndef CUSTOMPALETTES_HPP
#define CUSTOMPALETTES_HPP

#include <QDialog>
//#include "palettewidget.hpp"

#include <QMap>
#include <QListWidgetItem>

namespace Ui {
    class CustomPalettes;
}

class CustomPalettes : public QDialog {
    Q_OBJECT
public:
	CustomPalettes(const QMap< QString, QList<QRgb> >& initialPalettes, QWidget *parent = 0);
	~CustomPalettes();

	const QMap<QString, QList<QRgb> >& getPalettes() const { return palettes_; }

protected:
    void changeEvent(QEvent *e);

private slots:
	void on_listPals_currentRowChanged(int currentRow);

	void on_listColors_currentRowChanged(int currentRow);

	void on_cmdRenPal_clicked();

	void on_tbEditColor_clicked();

	void on_listColors_itemChanged(QListWidgetItem *item);

	void on_cmdAddCol_clicked();

	void on_cmdDelCol_clicked();

	void on_cmdDelPal_clicked();

	void on_cmdAddPal_clicked();

	void on_listPals_itemChanged(QListWidgetItem *item);

	void on_cmdWml_clicked();

	void on_leColor_textEdited(const QString &arg1);

private:
    Ui::CustomPalettes *ui;
	//PaletteWidget *palw;

	QMap< QString, QList<QRgb> > palettes_;

	void addPaletteListEntry(const QString& name);
	void removePaletteListEntry(const QString& name);

	void renamePaletteListEntry(const QString& old_name, const QString& new_name) {
		removePaletteListEntry(old_name);
		addPaletteListEntry(new_name);
	}

	void populatePaletteView(const QList<QRgb>& pal);

	/** Update the palette UI starting from scratch. */
	void updatePaletteUI();

	void setColorEditControlsEnabled(bool enabled);
	void setPaletteViewEnabled(bool enabled);
	void setPaletteEditControlsEnabled(bool enabled);

	void clearPaletteView();

	QList<QRgb>& getCurrentPalette();

	void updatePaletteIcon();

	QString generateNewPaletteName() const;
};

#endif // CUSTOMPALETTES_HPP
