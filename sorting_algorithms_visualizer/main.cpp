#include "sort_visualizer.h"
#include <QtWidgets/QApplication>
#include <vector>
#include <iostream>
#include <set>
#include <numeric>
#include <algorithm>



int main(int argc, char * argv[])
{
	QApplication app(argc, argv);
	sort_visualizer w;
	w.show();
	return app.exec();
}