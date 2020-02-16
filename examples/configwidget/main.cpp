#include <QtWidgets/QApplication>
#include "qconfigwidget.h"
#include "qconfigwizard.h"
int main(int argc, char *argv[])
{
	//Q_INIT_RESOURCE(configdialog);

	QApplication app(argc, argv);
	app.setApplicationDisplayName("Qt Example");

	QConfigWidget widget;
	QConfigWizard wizard;

	widget.show();
	wizard.show();

	return app.exec();
}

