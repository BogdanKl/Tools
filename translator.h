#pragma once

#include "QJsonDocument"
#include "QDomDocument"
#include "QDomElement"

class Translator
{
public:
	Translator();
	QString startTranslate(QDomNode doc);
	QString translate(QDomNode doc, const QString t, int mX, int mY);
private:
	QString parseEllipse(QDomElement elem, QString const t, int mX, int mY);
	QString parseLine(QDomElement elem, QString const t, int mX, int mY);
	QString parseRectangle(QDomElement elem, QString t, int mX, int mY);
	QString parseArc(QDomElement elem, QString t, int mX, int mY);
	QString parseText(QDomElement elem, QString t, int mX, int mY);
	QString parseImage(QDomElement elem, QString t, int mX, int mY);
	QString parsePath(QDomElement elem, QString t, int mX, int mY);
	QString parseGraphics(QDomElement elem, QString const t);
	int gcd(int a, int b);
};

