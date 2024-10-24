#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_sort_visualizer.h"
#include <random>
#include "thread_pool.h"
#include "sorts.h"
#include <stdlib.h>
#include "array_frame.h"

#if __has_include(<boost/sort/sort.hpp>)
#include <boost/sort/sort.hpp>
#else
#endif

enum algorithms
{
	std_sort,
	std_stable_sort,
	std_heap_sort,

#if __has_include(<boost/sort/sort.hpp>)
	boost_pdqsort,
	boost_pdqsort_branchless,
	boost_integer_sort,
#endif

	dual_pivot_quick_sort,
	shell_sort,
	d_heap_sort,
	timsort,
};

static const QMap<algorithms, QString> algo_name_map({

#if __has_include(<boost/sort/sort.hpp>)
	{ algorithms::boost_integer_sort,		"boost::integer_sort" },
	{ algorithms::boost_pdqsort,			"boost::pdqsort" },
	{ algorithms::boost_pdqsort_branchless,	"boost::pdqsort_branchless" },
#endif

	{ algorithms::std_heap_sort,			"std::sort_heap" },
	{ algorithms::std_sort,					"std::sort" },
	{ algorithms::std_stable_sort,			"std::stable_sort" },
	{ algorithms::dual_pivot_quick_sort,	"dual pivot quick sort" },
	{ algorithms::d_heap_sort,				"d-ary heap sort (d = 4)" },
	{ algorithms::shell_sort,				"shell sort" },
	{ algorithms::timsort,					"timsort" },
	});

QString to_str(const algorithms & algorithm);
algorithms get_algorithm(const QString & name);

class sort_visualizer : public QMainWindow
{
	Q_OBJECT

public:
	sort_visualizer(QWidget * parent = Q_NULLPTR);
	~sort_visualizer();

private slots:
	void on_sort_button_clicked();
	void on_shuffle_button_clicked();
	void on_pause_button_clicked();
	void on_step_button_clicked();
	void on_break_button_clicked();

	void on_algorithms_list_widget_currentItemChanged(QListWidgetItem * new_item, QListWidgetItem * old_item);

	void on_delay_spin_box_valueChanged(int value);
	void on_array_size_spin_box_valueChanged(int value);

private:

	bool is_running;
	algorithms current_algorithm;

	thread_pool threads;
	Ui::sort_visualizerClass ui;

	void fill_array(size_t);
};

