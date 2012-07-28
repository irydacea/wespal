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
    CustomPalettes(QWidget *parent = 0);
    ~CustomPalettes();

	void addPalette(const QString& name, const QList<QRgb>& colors) {
		palettes_.insert(name, colors);
		addPaletteListEntry(name);
	}

	void addMultiplePalettes(const QMap< QString, QList<QRgb> >& palettes) {
		for(QMap< QString, QList<QRgb> >::const_iterator i = palettes.constBegin();
			i != palettes.constEnd(); ++i) {
			palettes_.insert(i.key(), i.value());
			addPaletteListEntry(i.key());
		}
	}

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
};

#endif // CUSTOMPALETTES_HPP
