#include <QApplication>
#include <QJsonDocument>
#include <QDomDocument>
#include <translator.h>
#include <QDebug>
#include <QFile>
//#include "qmlType/declarativeEllipse.h"
//#include "qmlType/declarativeLine.h"
//#include "qmlType/declarativePen.h"
//#include <QtDeclarative/QDeclarativeEngine>
//#include <QtDeclarative/QDeclarativeItem>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	/*qmlRegisterType<qmlTypes::DeclarativeLine>("CustomComponents", 1, 0, "Line");
	qmlRegisterType<qmlTypes::DeclarativeEllipse>("CustomComponents", 1, 0, "Ellipse");
	qmlRegisterType<qmlTypes::DeclarativePen>("CustomComponents", 1, 0,"Border");*/
	//QDeclarativeComponent component(QDeclarativeEngine());
	//QDomDocument doc("example.xml");
	QFile file("be.xml");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return 0;

	QDomDocument doc;
	if (!doc.setContent(&file)) {
		file.close();
		return 0;
	}
	Translator t;
	QFile files("test.qml");
	files.open(QIODevice::WriteOnly | QIODevice::Text);
	files.write(QString("import QtQuick 2.0 \n").toLatin1());
	files.write(QString("import CustomComponents 1.0 \n").toLatin1());
	files.write(t.startTranslate(doc.firstChild()).toLatin1());
	//qDebug() << t.translate(doc.firstChild(), "");
}
