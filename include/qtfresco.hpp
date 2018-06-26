#ifndef QTFRESCO_H
#define QTFRESCO_H

#include <QMainWindow>

class graphPlotter;
class QTimer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);

	~MainWindow();

	void setPointer(graphPlotter *ptr_){ ptr = ptr_; }
	
	void setInputFilename(const std::string &str_);
	
	void setOutputFilename(const std::string &str_);
	
	void setOutputPrefix(const std::string &str_);

	bool setDataFilename(const std::string &str_, const std::string &gname_="");
	
	void finalize();

private slots:
	void on_pushButton_run_clicked();

	void on_pushButton_save_clicked();

	void on_pushButton_reset_clicked();

	void on_pushButton_rescan_clicked();

	void on_pushButton_quit_clicked();

	void disableRadio();

	void enableRadio();

	void on_radioButton_tgraph_clicked();

	void on_radioButton_tcanvas_clicked();

	void on_radioButton_ascii_clicked();

	void on_radioButton_print_clicked();

	void on_radioButton_stdout_clicked();

	void on_radioButton_fortXX_clicked();

	void on_lineEdit_dataFilename_returnPressed();

	void on_comboBox_dataObjName_currentIndexChanged();

	void on_pushButton_drawData_clicked();

	void handleCleanup();

	void handleUpdate();

	void on_checkBox_debug_clicked();

private:
	Ui::MainWindow *ui;
	
	QTimer *idleTimer;
	
	graphPlotter *ptr;

	bool scanFrescoOutput();

	bool updateDataSelect();
};

#endif // QTFRESCO_H
