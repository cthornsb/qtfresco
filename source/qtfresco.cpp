#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "QtCore/QTimer"

#include "qtfresco.hpp"
#include "ui_qtfresco.hpp"

#include "graphPlotter.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	idleTimer(new QTimer(this)),
	ptr(NULL)
{
	ui->setupUi(this);
	
	connect(idleTimer, SIGNAL(timeout()), this, SLOT(handleUpdate()));
	idleTimer->start(100);
}

MainWindow::~MainWindow()
{
	if(ui->checkBox_cleanup->isChecked()) // Remove the intermediate file.
		remove("qtfresco.tmp.out");

	delete ui;
	delete idleTimer;

	// This must be last because TApplication will terminate the program.
	if(ptr) delete ptr;
}

void MainWindow::setInputFilename(const std::string &str_){
	ui->lineEdit_input->setText(QString::fromStdString(str_));
}

void MainWindow::setOutputFilename(const std::string &str_){
	ui->lineEdit_output->setText(QString::fromStdString(str_));
}

void MainWindow::setOutputPrefix(const std::string &str_){
	ui->lineEdit_prefix->setText(QString::fromStdString(str_));
}

void MainWindow::setDataFilename(const std::string &str_){
	ui->lineEdit_dataFilename->setText(QString::fromStdString(str_));
	updateDataSelect();
}

void MainWindow::finalize(){
	if(ui->lineEdit_output->text().isEmpty()) ui->lineEdit_output->setText("frescout");
	if(ui->lineEdit_prefix->text().isEmpty()) ui->lineEdit_prefix->setText("graph");
}

bool MainWindow::scanFrescoOutput(){
	double totalIntegral;
	if(ui->radioButton_stdout->isChecked()){ // Run frescout on stdout.
		if(!ptr->runFrescout(ui->lineEdit_searchStr->text().toStdString(), totalIntegral)){
			std::cout << " ERROR! Frescout method failed to find any cross-section data.\n";
			return false;
		}
	}
	else{ // Run readGrace on fort output file.
		std::stringstream stream;
		stream << "fort." << ui->spinBox_fortXX->value();
		if(!ptr->runReadGrace(stream.str(), totalIntegral)){
			std::cout << " ERROR! ReadGrace method failed to find any cross-section data from \"" << stream.str() << "\".\n";
			return false;
		}
	}

	// Draw the distribution if the calculation completed successfully.
	ptr->draw(ui->checkBox_drawPrev->isChecked());
	
	// Draw the external data TGraph.
	if(ui->checkBox_drawData->isChecked())
		ptr->drawData(ui->lineEdit_dataDrawOpt->text().toStdString());

	// Print the total integral in the window.
	std::stringstream stream; stream << totalIntegral;
	QString str = QString::fromStdString(stream.str());
	ui->lineEdit_integral->setText(str);
		
	// Write graph to file if user has requested to save all graphs.
	if(ui->checkBox_saveAll->isChecked())
		on_pushButton_save_clicked();

	return true;
}

void MainWindow::updateDataSelect(){
	ui->comboBox_dataObjName->clear();
	ui->comboBox_dataObjName->addItem("NONE");
	if(ptr->loadExternalDataFile(ui->lineEdit_dataFilename->text().toStdString())){ // Open an external TFile.
		std::vector<std::string> objNames;
		if(ptr->readExternalDataFile(objNames)){ // Read the TFile and search for graphs.
			ui->comboBox_dataObjName->setEnabled(true);
			for(std::vector<std::string>::iterator iter = objNames.begin(); iter != objNames.end(); iter++){ // Add all names to the combo list.
				ui->comboBox_dataObjName->addItem(QString::fromStdString(*iter));
			}
			ui->lineEdit_dataDrawOpt->setEnabled(true);
			ui->pushButton_drawData->setEnabled(true);
			ui->checkBox_drawData->setEnabled(true);
			ui->label_dataObjName->setEnabled(true);
		}
	}
	else{ // Failed to load the input file.
		ui->comboBox_dataObjName->setDisabled(true);
		ui->lineEdit_dataDrawOpt->setDisabled(true);
		ui->pushButton_drawData->setDisabled(true);
		ui->checkBox_drawData->setDisabled(true);
		ui->label_dataObjName->setDisabled(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
/// Push buttons
///////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButton_run_clicked()
{
	if(!ui->lineEdit_input->text().isEmpty()){
		ui->pushButton_save->setEnabled(true);
		ui->pushButton_rescan->setEnabled(true);
		ui->pushButton_reset->setEnabled(true);
		if(ptr->runFresco(ui->lineEdit_input->text().toStdString().c_str())){
			this->scanFrescoOutput();
		}
		else std::cout << " ERROR! Failed to run fresco.\n";
	}
	else std::cout << " ERROR! No fresco input filename specified.\n";
}

void MainWindow::on_pushButton_save_clicked()
{
	std::string outputFilenamePrefix = ui->lineEdit_output->text().toStdString();
	std::string outputGraphNamePrefix = ui->lineEdit_prefix->text().toStdString();

	if(ui->radioButton_tgraph->isChecked()){ // Write output TGraph to file.
		int retval = ptr->write(outputFilenamePrefix+".root", 0, outputGraphNamePrefix, ui->radioButton_recreate->isChecked());
		if(retval >= 0){
			ui->lcdNumber->display(retval);
			std::cout << " Wrote TGraph named \"" << outputGraphNamePrefix << "\" to output file \"" << outputFilenamePrefix << ".root\"\n";
		}
		else std::cout << " ERROR! Failed to write to output file \"" << outputFilenamePrefix << ".root\"\n";
	}
	else if(ui->radioButton_tcanvas->isChecked()){
		int retval = ptr->write(outputFilenamePrefix+".root", 1, outputGraphNamePrefix, ui->radioButton_recreate->isChecked());
		if(retval >= 0){
			ui->lcdNumber->display(retval);
			std::cout << " Wrote TCanvas named \"" << outputGraphNamePrefix << "\" to output file \"" << outputFilenamePrefix << ".root\"\n";
		}
		else std::cout << " ERROR! Failed to write to output file \"" << outputFilenamePrefix << ".root\"\n";
	}
	else if(ui->radioButton_ascii->isChecked()){
		if(ptr->write(outputFilenamePrefix+".dat", 2) >= 0)
			std::cout << " Wrote distribution to output file \"" << outputFilenamePrefix << ".dat\"\n";
		else 
			std::cout << " ERROR! Failed to write to output file \"" << outputFilenamePrefix << ".dat\"\n";
	}
	else if(ui->radioButton_print->isChecked()){
		if(ptr->write(outputFilenamePrefix, 3) >= 0)
			std::cout << " Printed TCanvas to output file \"" << outputFilenamePrefix << "\"\n";
		else 
			std::cout << " ERROR! Failed to print to output file \"" << outputFilenamePrefix << "\"\n";
	}
}

void MainWindow::on_pushButton_rescan_clicked()
{
	this->scanFrescoOutput();
}

void MainWindow::on_pushButton_reset_clicked()
{
	ptr->reset();
	ui->pushButton_save->setDisabled(true);
	ui->pushButton_rescan->setDisabled(true);
	ui->pushButton_reset->setDisabled(true);
}

void MainWindow::on_pushButton_quit_clicked()
{
	// This is a hack, but it avoids seg-faults due to TApplication.
	//QCoreApplication::quit();
	delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// Radio buttons
///////////////////////////////////////////////////////////////////////////////

void MainWindow::disableRadio()
{
	ui->radioButton_update->setChecked(false);
	ui->radioButton_recreate->setChecked(true);

	ui->radioButton_update->setDisabled(true);
	ui->radioButton_recreate->setDisabled(true);
	
	ui->lineEdit_prefix->setDisabled(true);
	ui->lcdNumber->setDisabled(true);
	
	ui->checkBox_saveAll->setChecked(false);
	ui->checkBox_saveAll->setDisabled(true);
}

void MainWindow::enableRadio()
{
	ui->radioButton_update->setEnabled(true);
	ui->radioButton_recreate->setEnabled(true);
	
	ui->lineEdit_prefix->setEnabled(true);
	ui->lcdNumber->setEnabled(true);
	
	ui->checkBox_saveAll->setEnabled(true);
}

void MainWindow::on_radioButton_tgraph_clicked()
{
	enableRadio();
}

void MainWindow::on_radioButton_tcanvas_clicked()
{
	enableRadio();
}

void MainWindow::on_radioButton_ascii_clicked()
{
	disableRadio();
}

void MainWindow::on_radioButton_print_clicked()
{
	disableRadio();
}

void MainWindow::on_radioButton_stdout_clicked()
{
	ui->label_searchStr->setEnabled(true);
	ui->lineEdit_searchStr->setEnabled(true);
	ui->label_fortXX->setDisabled(true);
	ui->spinBox_fortXX->setDisabled(true);
}

void MainWindow::on_radioButton_fortXX_clicked()
{
	ui->label_searchStr->setDisabled(true);
	ui->lineEdit_searchStr->setDisabled(true);
	ui->label_fortXX->setEnabled(true);
	ui->spinBox_fortXX->setEnabled(true);
}

void MainWindow::on_lineEdit_dataFilename_returnPressed(){
	updateDataSelect();
}

void MainWindow::on_comboBox_dataObjName_currentIndexChanged(){
	if(ui->comboBox_dataObjName->currentText() != "NONE") // User has selected an object from the file.
		ptr->setExternalDataGraph(ui->comboBox_dataObjName->currentText().toStdString());
	else // De-select all objects
		ptr->setExternalDataGraph();
}

void MainWindow::on_pushButton_drawData_clicked(){
	ptr->drawData(ui->lineEdit_dataDrawOpt->text().toStdString());
}

void MainWindow::handleCleanup(){
	delete ptr;
}

void MainWindow::handleUpdate(){
	ptr->idleTask();
}

void MainWindow::on_checkBox_debug_clicked()
{
	ptr->toggleDebug();
}
