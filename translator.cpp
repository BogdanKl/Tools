#include "translator.h"
#include "QDomDocument"
#include <QDebug>
#include <QFile>
#include <math.h>

Translator::Translator()
{
}

QString Translator::startTranslate(QDomNode doc)
{
	return translate(doc, "", 1024, 780);
}

QString Translator::translate(QDomNode doc, QString const t, int mX, int mY)
{
	QString result = "";
	QDomElement elem;
	while (!doc.isNull())
	{
		elem = doc.toElement();
		if( elem.nodeName() == "line")
		{
			result += parseLine(elem, t, mX, mY);
		} else if (elem.nodeName() == "picture") {
			mX = elem.attribute("sizex").toInt();
			mY = elem.attribute("sizey").toInt();
			result += parseGraphics(elem, t);
		} else if (elem.nodeName() == "ellipse") {
			result += parseEllipse(elem, t, mX, mY);
		} else if (elem.nodeName() == "external") {
			result += parseImage(elem, t);
		} else if (elem.nodeName() == "node") {
			QFile resfile("result/"+elem.attribute("name")+"Class.qml");
			resfile.open(QIODevice::WriteOnly | QIODevice::Text);
			resfile.write(QString("import QtQuick 1.0 \n").toLatin1());
			resfile.write(QString("import CustomComponents 1.0 \n").toLatin1());
			resfile.write((translate(doc.firstChild(), "", mX, mY)).toLatin1());
		} else if (elem.nodeName() == "edge") {
			QFile resfile("result/"+elem.attribute("name")+"EdgeClass.qml");
			resfile.open(QIODevice::WriteOnly | QIODevice::Text);
			resfile.write(QString("import QtQuick 1.0 \n").toLatin1());
			resfile.write(QString("import CustomComponents 1.0 \n").toLatin1());
			resfile.write((translate(doc.firstChild(), "", mX, mY)).toLatin1());
		} else if (elem.nodeName() == "rectangle") {
			result += parseRectangle(elem, t, mX, mY);
		} else if (elem.nodeName() == "arc") {
			result += parseArc(elem, t, mX, mY);
		}
		if ((!doc.hasChildNodes()) && doc.nodeName() == "picture")
		{
			result += QString(t+"} \n");
		} else if (doc.hasChildNodes() && doc.nodeName() == "picture") {
			result += (translate(doc.firstChild(),t+"\t", mX, mY))+QString(t+"} \n");
		} else if (doc.hasChildNodes()){
			result += (translate(doc.firstChild(),t, mX, mY));
		}
		doc = doc.nextSibling();
	}
	return result;
}

QString Translator::parseEllipse(QDomElement elem, QString const t, int mX, int mY)
{
	QString result = "";
	QString elemx1 = elem.attribute("x1");
	QString elemy1 = elem.attribute("y1");
	QString elemx2 = elem.attribute("x2");
	QString elemy2 = elem.attribute("y2");
	QString color = elem.attribute("stroke") != "" ? (QString("\"") + elem.attribute("stroke") + QString("\"")) : (QString("\"") + "black" +QString("\""));
	QString style = elem.attribute("fill-style") != "" ? (QString("\"") +elem.attribute("fill-style") + QString("\"")): (QString("\"")+"solid" +QString("\""));
	QString penWidth = elem.attribute("stroke-width") != "" ? elem.attribute("stroke-width") : "1";
	int x1, y1, x2, y2, width, height;
	int x1mX, y1mY, widthmX, heightmY;
	x1 = elemx1.at(elemx1.length()-1) != 'a' ? elemx1.toInt() : elemx1.remove(elemx1.length()-1, 1).toInt();
	y1 = elemy1.at(elemy1.length()-1) != 'a' ? elemy1.toInt() : elemy1.remove(elemy1.length()-1, 1).toInt();
	x2 = elemx2.at(elemx2.length()-1) != 'a' ? elemx2.toInt() : elemx2.remove(elemx2.length()-1, 1).toInt();
	y2 = elemy2.at(elemy2.length()-1) != 'a' ? elemy2.toInt() : elemy2.remove(elemy2.length()-1, 1).toInt();
	width = x2 - x1;
	height = y2 - y1;
	widthmX = gcd(width, mX);
	x1 = qMin(x1, x2) + abs(x1-x2)/2;
	y1 = qMin(y1, y2) + abs(y1-y2)/2;
	x1mX = gcd(x1, mX - penWidth.toInt());
	y1mY = gcd(y1, mY - penWidth.toInt());
	heightmY = gcd(height, mY);
	result += t + QString("Ellipse { \n")
			+ t + QString("\t") + QString("x: ") + QString::number(x1/x1mX) + QString(" * ") + QString("parent.width") + QString(" / ") + QString::number(mX / x1mX) + QString("\n")
			+ t + QString("\t") +QString("y: ") + QString::number(y1/y1mY) + QString(" * ") + QString("parent.height") + QString(" / ") + QString::number(mY / y1mY) + QString("\n")
			+ t + QString("\t") + QString("width: ") + QString::number(width/widthmX)+ QString(" * ") + QString("parent.width") + QString(" / ") + QString::number(mX / widthmX) + QString("\n")
			+ t + QString("\t") + QString("height: ") +  QString::number(height/heightmY)+ QString(" * ") + QString("parent.height") + QString(" / ") + QString::number(mY / heightmY) + QString("\n")
			+ t + QString("\t") + QString("color: ") +QString("\"") + elem.attribute("fill") + QString("\"") + QString("\n")
			+ t + QString("\t") + QString("border.color: ") + color +QString("\n")
			+ t + QString("\t") + QString("border.style: ") + style + QString("\n")
			+ t + QString("\t") + QString("border.width: ") + penWidth +QString("\n")
			+ t + QString("} \n");
	return result;
}

QString Translator::parseLine(QDomElement elem, const QString t, int mX, int mY)
{
	QString result = "";
	result += t+ QString("Line { \n");
	int x1, y1, x2, y2;
	int x1mX, y1mY, x2mX, y2mY;
	qDebug() << "Line";
	QString elemx1 = elem.attribute("x1");
	QString elemy1 = elem.attribute("y1");
	QString elemx2 = elem.attribute("x2");
	QString elemy2 = elem.attribute("y2");
	QString penWidth = elem.attribute("stroke-width") != "" ? elem.attribute("stroke-width") : "1";
	if (elemx1.at(elemx1.length()-1) != 'a') {
		x1 = elemx1.toInt();
		if(mX - x1 < penWidth.toInt()) {
			x1 -= (penWidth.toInt() + 1);
		}
		x1mX = gcd(x1, mX);
		result += t + QString("\t") + QString("x1: ") + QString::number(x1 / x1mX) + QString(" * ") + QString("parent.width") + QString(" / ") +QString::number(mX / x1mX) + QString("\n");
	} else {
		result += t + QString("\t") + QString("x1: ") + elemx1.remove(elemx1.length()-1,1) + QString(" \n");
	}
	if (elemy1.at(elemy1.length()-1) != 'a') {
		y1 = elemy1.toInt();
		if(mY - y1 < penWidth.toInt())
		{
			y1 -= (penWidth.toInt() + 1);
		}
		y1mY = gcd(y1, mY);
		result += t + QString("\t") + QString("y1: ") + QString::number(y1 / y1mY) + QString(" * ") + QString("parent.height") + QString(" / ") +QString::number(mY / y1mY) + QString("\n");
	} else {
		result += t + QString("\t") + QString("y1: ") + elemy1.remove(elemy1.length()-1,1) + QString(" \n");
	}
	if (elemx2.at(elemx2.length()-1) != 'a') {
		x2 = elemx2.toInt();
		if(mX - x2 < penWidth.toInt())
		{
			qDebug() << "hm";
			x2 -= (penWidth.toInt() + 1);
		}
		x2mX = gcd(x2, mX);
		result += t + QString("\t") + QString("x2: ") + QString::number(x2 / x2mX) + QString(" * ") + QString("parent.width") + QString(" / ") +QString::number(mX / x2mX) + QString("\n");
	} else {
		result += t + QString("\t") + QString("x2: ") + elemx2.remove(elemx2.length()-1,1) + QString(" \n");
	}
	if (elemy2.at(elemy2.length()-1) != 'a') {
		y2 = elemy2.toInt();
		if(mY - y2 < penWidth.toInt())
		{
			y2 -= (penWidth.toInt() + 1);
		}
		y2mY = gcd(y2, mY);
		result += t + QString("\t") + QString("y2: ") + QString::number(y2 / y2mY) + QString(" * ") + QString("parent.height") + QString(" / ") +QString::number(mY / y2mY) + QString("\n");
	} else {
		result += t + QString("\t") + QString("y2: ") + elemy2.remove(elemy2.length()-1,1) + QString(" \n");
	}
	QString color = elem.attribute("fill") != "" ? (QString("\"") + elem.attribute("fill") + QString("\"")) : (QString("\"") + "black" +QString("\""));
	QString style = elem.attribute("stroke-style") != "" ? (QString("\"") +elem.attribute("stroke-style") + QString("\"")): (QString("\"")+"none" +QString("\""));
	result += t + QString("\t") + QString("color: ") + color +QString("\n")
		+ t + QString("\t") + QString("style: ") + style + QString("\n")
		+ t + QString("\t") + QString("width: ") + penWidth +QString("\n")
		+ t + QString("} \n");
	return result;
}

QString Translator::parseGraphics(QDomElement elem, const QString t)
{
	QString result = "";
	result += t + QString("Rectangle { \n")
			+ t + QString("\t") + QString("width: ") + elem.attribute("sizex") + QString("; height: ") + elem.attribute("sizey") + QString("\n")
			+ t + QString("\t") + QString("color: ") +QString("\"") +QString("transparent") + QString("\"") + QString("\n");
	return result;
}

QString Translator::parseRectangle(QDomElement elem, QString t, int mX, int mY)
{
	QString result = "";
	QString elemx1 = elem.attribute("x1");
	QString elemy1 = elem.attribute("y1");
	QString elemx2 = elem.attribute("x2");
	QString elemy2 = elem.attribute("y2");
	QString color = elem.attribute("fill") != "" ? (QString("\"") + elem.attribute("fill") + QString("\"")) : (QString("\"") + "black" +QString("\""));
	QString style = elem.attribute("fill-style") != "" ? (QString("\"") +elem.attribute("fill-style") + QString("\"")): (QString("\"")+"none" +QString("\""));
	QString penWidth = elem.attribute("stroke-width") != "" ? elem.attribute("stroke-width") : "1";
	int x1, y1, x2, y2, width, height;
	int x1mX, y1mY, widthmX, heightmY;
	x1 = elemx1.at(elemx1.length()-1) != 'a' ? elemx1.toInt() : elemx1.remove(elemx1.length()-1, 1).toInt();
	y1 = elemy1.at(elemy1.length()-1) != 'a' ? elemy1.toInt() : elemy1.remove(elemy1.length()-1, 1).toInt();
	x2 = elemx2.at(elemx2.length()-1) != 'a' ? elemx2.toInt() : elemx2.remove(elemx2.length()-1, 1).toInt();
	y2 = elemy2.at(elemy2.length()-1) != 'a' ? elemy2.toInt() : elemy2.remove(elemy2.length()-1, 1).toInt();
	width = abs(x2 - x1);
	height = abs(y2 - y1);
	widthmX = gcd(width, mX);
	x1mX = gcd(x1, mX - penWidth.toInt());
	y1mY = gcd(y1, mY - penWidth.toInt());
	heightmY = gcd(height, mY);
	result += t + QString("Rectangle { \n")
			+ t + QString("\t") + QString("anchors.fill: parent") + QString("\n")
			+ t + QString("\t") + QString("x: ") + QString::number(x1/x1mX) + QString(" * ") + QString("parent.width") + QString(" / ") + QString::number(mX / x1mX) + QString("\n")
			+ t + QString("\t") +QString("y: ") + QString::number(y1/y1mY) + QString(" * ") + QString("parent.height") + QString(" / ") + QString::number(mY / y1mY) + QString("\n")
			+ t + QString("\t") + QString("width: ") + QString::number(width/widthmX)+ QString(" * ") + QString("parent.width") + QString(" / ") + QString::number(mX / widthmX) + QString("\n")
			+ t + QString("\t") + QString("height: ") +  QString::number(height/heightmY)+ QString(" * ") + QString("parent.height") + QString(" / ") + QString::number(mY / heightmY) + QString("\n")
			+ t + QString("\t") + QString("border.color: ") + color +QString("\n")
//			+ t + QString("\t") + QString("border.style: ") + style + QString("\n")
			+ t + QString("\t") + QString("border.width: ") + penWidth +QString("\n")
			+ t + QString("} \n");
	return result;
}

QString Translator::parseArc(QDomElement elem, QString t, int mX, int mY)
{
	QString result = "";
	QString elemx1 = elem.attribute("x1");
	QString elemy1 = elem.attribute("y1");
	QString elemx2 = elem.attribute("x2");
	QString elemy2 = elem.attribute("y2");
	QString elemstart = elem.attribute("startAngle");
	QString elemspan = elem.attribute("spanAngle");
	QString color = elem.attribute("fill") != "" ? (QString("\"") + elem.attribute("fill") + QString("\"")) : (QString("\"") + "black" +QString("\""));
	QString style = elem.attribute("fill-style") != "" ? (QString("\"") +elem.attribute("fill-style") + QString("\"")): (QString("\"")+"none" +QString("\""));
	QString penWidth = elem.attribute("stroke-width") != "" ? elem.attribute("stroke-width") : "1";
	int x1, y1, x2, y2, width, height;
	int x1mX, y1mY, widthmX, heightmY;
	x1 = elemx1.at(elemx1.length()-1) != 'a' ? elemx1.toInt() : elemx1.remove(elemx1.length()-1, 1).toInt();
	y1 = elemy1.at(elemy1.length()-1) != 'a' ? elemy1.toInt() : elemy1.remove(elemy1.length()-1, 1).toInt();
	x2 = elemx2.at(elemx2.length()-1) != 'a' ? elemx2.toInt() : elemx2.remove(elemx2.length()-1, 1).toInt();
	y2 = elemy2.at(elemy2.length()-1) != 'a' ? elemy2.toInt() : elemy2.remove(elemy2.length()-1, 1).toInt();
	width = x2 - x1;
	height = y2 - y1;
	widthmX = gcd(width, mX);
	x1 = qMin(x1, x2) + abs(x1-x2)/2;
	y1 = qMin(y1, y2) + abs(y1-y2)/2;
	x1mX = gcd(x1, mX - penWidth.toInt());
	y1mY = gcd(y1, mY - penWidth.toInt());
	heightmY = gcd(height, mY);
	result += t + QString("Arc { \n")
			+ t + QString("\t") + QString("x: ") + QString::number(x1/x1mX) + QString(" * ") + QString("parent.width") + QString(" / ") + QString::number(mX / x1mX) + QString("\n")
			+ t + QString("\t") +QString("y: ") + QString::number(y1/y1mY) + QString(" * ") + QString("parent.height") + QString(" / ") + QString::number(mY / y1mY) + QString("\n")
			+ t + QString("\t") + QString("widthx: ") + QString::number(width/widthmX)+ QString(" * ") + QString("parent.width") + QString(" / ") + QString::number(mX / widthmX) + QString("\n")
			+ t + QString("\t") + QString("height: ") +  QString::number(height/heightmY)+ QString(" * ") + QString("parent.height") + QString(" / ") + QString::number(mY / heightmY) + QString("\n")
			+ t + QString("\t") + QString("startAngle: ") + elemstart + QString("; spanAngle: ") + elemspan +QString("\n")
			//+ t + QString("\t") + QString("color: ") +QString("\"") + QString("white") + QString("\"") + QString("\n")
			+ t + QString("\t") + QString("color: ") + color +QString("\n")
			+ t + QString("\t") + QString("style: ") + style + QString("\n")
			+ t + QString("\t") + QString("width: ") + penWidth +QString("\n")
			+ t + QString("} \n");
	return result;
}

QString Translator::parseImage(QDomElement elem, QString t)
{
	QString result = "";
	result += t + QString("Image { \n")
			+ t + QString("\t") +QString("source: ") + QString("\"")+ elem.attribute("file") +QString("\"") +QString("\n")
			+ t + QString("}\n");
	return result;
}

int Translator::gcd(int a, int b)
{
	int c;
	while (b) {
		c = a % b;
		a = b;
		b = c;
	}
	return abs(a);
}