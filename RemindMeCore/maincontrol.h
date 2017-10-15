#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include <control.h>

class MainControl : public Control
{
	Q_OBJECT

	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
	explicit MainControl(QObject *parent = nullptr);

	void onShow() override;
	void onClose() override;

	QString text() const;

public slots:
	void setText(QString text);

signals:
	void textChanged(QString text);

private:
	QString _text;
};

#endif // MAINCONTROL_H
