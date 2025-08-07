#include <QApplication>
#include <QLabel>
#include <QWidget>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QLabel hello("<center>Hello World!</center>");
	hello.setWindowTitle("My First QT");
	hello.resize(100, 100);
	hello.show();

	return app.exec();
}
