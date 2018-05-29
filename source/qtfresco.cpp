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
    delete ui;
    delete idleTimer;
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

void MainWindow::finalize(){
	if(ui->lineEdit_output->text().isEmpty()) ui->lineEdit_output->setText("frescout");
	if(ui->lineEdit_prefix->text().isEmpty()) ui->lineEdit_prefix->setText("graph");
}

///////////////////////////////////////////////////////////////////////////////
/// Push buttons
///////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pushButton_run_clicked()
{
    if(!ui->lineEdit_input->text().isEmpty()){
        double totalIntegral;
        if(ptr->runFresco(ui->lineEdit_input->text().toStdString().c_str()) && ptr->runFrescout(totalIntegral, ui->checkBox_cleanup->isChecked())){
            // Draw the distribution if the calculation completed successfully.
			ptr->redraw(ui->checkBox_drawPrev->isChecked());
        
            // Print the total integral in the window.
            std::stringstream stream; stream << totalIntegral;
            QString str = QString::fromStdString(stream.str());
            ui->lineEdit_integral->setText(str);
            
            // Write graph to file if user has requested to save all graphs.
            if(ui->checkBox_saveAll->isChecked())
            	on_pushButton_save_clicked();
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

void MainWindow::on_pushButton_redraw_clicked()
{
	ptr->redraw();
}

void MainWindow::on_pushButton_reset_clicked()
{
    ptr->reset();
}

void MainWindow::on_pushButton_quit_clicked()
{
	QCoreApplication::quit();
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
